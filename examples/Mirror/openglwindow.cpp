#include "openglwindow.hpp"

#include <imgui.h>
#include <fmt/core.h>

#include <cppitertools/itertools.hpp>
#include <glm/gtc/matrix_inverse.hpp>

#include "imfilebrowser.h"

#define PI 3.14159265358979323846

void OpenGLWindow::handleEvent(SDL_Event& event) {
  if (event.type == SDL_KEYDOWN) {
    if (event.key.keysym.sym == SDLK_UP || event.key.keysym.sym == SDLK_w)
      m_camera.setDollySpeed(2.0f);
    if (event.key.keysym.sym == SDLK_DOWN || event.key.keysym.sym == SDLK_s)
      m_camera.setDollySpeed(-2.0f);
    if (event.key.keysym.sym == SDLK_LEFT || event.key.keysym.sym == SDLK_a)
      m_camera.setPanSpeed(-1.0f);
    if (event.key.keysym.sym == SDLK_RIGHT || event.key.keysym.sym == SDLK_d)
      m_camera.setPanSpeed(1.0f);
    if (event.key.keysym.sym == SDLK_q) m_camera.setTruckSpeed(-1.0f);
    if (event.key.keysym.sym == SDLK_e) m_camera.setTruckSpeed(1.0f);
  }
  if (event.type == SDL_KEYUP) {
    if ((event.key.keysym.sym == SDLK_UP || event.key.keysym.sym == SDLK_w) &&
        m_camera.getDollySpeed() > 0)
      m_camera.setDollySpeed(0.0f);
    if ((event.key.keysym.sym == SDLK_DOWN || event.key.keysym.sym == SDLK_s) &&
        m_camera.getDollySpeed() < 0)
      m_camera.setDollySpeed(0.0f);
    if ((event.key.keysym.sym == SDLK_LEFT || event.key.keysym.sym == SDLK_a) &&
        m_camera.getPanSpeed() < 0)
      m_camera.setPanSpeed(0.0f);
    if ((event.key.keysym.sym == SDLK_RIGHT || event.key.keysym.sym == SDLK_d) &&
        m_camera.getPanSpeed() > 0)
      m_camera.setPanSpeed(0.0f);
    if (event.key.keysym.sym == SDLK_q && m_camera.getTruckSpeed() < 0) m_camera.setTruckSpeed(0.0f);
    if (event.key.keysym.sym == SDLK_e && m_camera.getTruckSpeed() > 0) m_camera.setTruckSpeed(0.0f);
  }
}

void OpenGLWindow::initializeGL() {
  glClearColor(0, 0, 0, 1);
  glEnable(GL_DEPTH_TEST);

  // Create programs
  for (const auto& name : m_shaderNames) {
    auto path{getAssetsPath() + "shaders/" + name};
    auto program{createProgramFromFile(path + ".vert", path + ".frag")};
    m_programs.push_back(program);
  }

  // Load default model
  loadModel(getAssetsPath() + "bunny.obj");

  // Load cubemap
  m_model.loadCubeTexture(getAssetsPath() + "maps/cube/");

  // Initial trackball spin
  m_trackBallModel.setAxis(glm::normalize(glm::vec3(1, 1, 1)));
  m_trackBallModel.setVelocity(0.0f);

  m_viewportWidth = getWindowSettings().width;
  m_viewportHeight = getWindowSettings().height;
  m_camera.computeProjectionMatrix(m_viewportWidth, m_viewportHeight);

  m_model.setMirrorPz(mirrorPz);

  initializeSkybox();
}

void OpenGLWindow::initializeSkybox() {
  // Create skybox program
  auto path{getAssetsPath() + "shaders/" + m_skyShaderName};
  m_skyProgram = createProgramFromFile(path + ".vert", path + ".frag");

  // Generate VBO
  glGenBuffers(1, &m_skyVBO);
  glBindBuffer(GL_ARRAY_BUFFER, m_skyVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(m_skyPositions), m_skyPositions.data(),
               GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  // Get location of attributes in the program
  GLint positionAttribute{glGetAttribLocation(m_skyProgram, "inPosition")};

  // Create VAO
  glGenVertexArrays(1, &m_skyVAO);

  // Bind vertex attributes to current VAO
  glBindVertexArray(m_skyVAO);

  glBindBuffer(GL_ARRAY_BUFFER, m_skyVBO);
  glEnableVertexAttribArray(positionAttribute);
  glVertexAttribPointer(positionAttribute, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  // End of binding to current VAO
  glBindVertexArray(0);
}

void OpenGLWindow::loadModel(std::string_view path) {
  m_model.loadDiffuseTexture(getAssetsPath() + "maps/bs.png");
  m_model.loadNormalTexture(getAssetsPath() + "maps/bs_normal.png");
  m_model.loadFromFile(path);
  m_model.setupVAO(m_programs.at(m_currentProgramIndex));
  m_trianglesToDraw = m_model.getNumTriangles();

  // Use material properties from the loaded model
  m_Ka = m_model.getKa();
  m_Kd = m_model.getKd();
  m_Ks = m_model.getKs();
  m_shininess = m_model.getShininess();
}

void OpenGLWindow::paintGL() {
  update();
  glDisable(GL_CULL_FACE);
  glFrontFace(GL_CCW);
  

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glViewport(0, 0, m_viewportWidth, m_viewportHeight);

  // Use currently selected program
  const auto program{m_programs.at(m_currentProgramIndex)};
  glUseProgram(program);
  fmt::print(stdout, "{}\r", m_shaderNames[m_currentProgramIndex]);

  // Get location of uniform variables
  GLint viewMatrixLoc{glGetUniformLocation(program, "viewMatrix")};
  GLint projMatrixLoc{glGetUniformLocation(program, "projMatrix")};
  GLint modelMatrixLoc{glGetUniformLocation(program, "modelMatrix")};
  GLint normalMatrixLoc{glGetUniformLocation(program, "normalMatrix")};
  GLint lightDirLoc{glGetUniformLocation(program, "lightDirWorldSpace")};
  GLint shininessLoc{glGetUniformLocation(program, "shininess")};
  GLint colorLoc{glGetUniformLocation(program, "inColor")};
  GLint IaLoc{glGetUniformLocation(program, "Ia")};
  GLint IdLoc{glGetUniformLocation(program, "Id")};
  GLint IsLoc{glGetUniformLocation(program, "Is")};
  GLint KaLoc{glGetUniformLocation(program, "Ka")};
  GLint KdLoc{glGetUniformLocation(program, "Kd")};
  GLint KsLoc{glGetUniformLocation(program, "Ks")};
  GLint diffuseTexLoc{glGetUniformLocation(program, "diffuseTex")};
  GLint normalTexLoc{glGetUniformLocation(program, "normalTex")};
  GLint cubeTexLoc{glGetUniformLocation(program, "cubeTex")};
  GLint mappingModeLoc{glGetUniformLocation(program, "mappingMode")};
  GLint texMatrixLoc{glGetUniformLocation(program, "texMatrix")};

  m_viewMatrix = m_camera.getViewMatrix();
  m_projMatrix = m_camera.getProjMatrix();

  // Set uniform variables used by every scene object
  glUniformMatrix4fv(viewMatrixLoc, 1, GL_FALSE, &m_viewMatrix[0][0]);
  glUniformMatrix4fv(projMatrixLoc, 1, GL_FALSE, &m_projMatrix[0][0]);
  glUniform1i(diffuseTexLoc, 0);
  glUniform1i(normalTexLoc, 1);
  glUniform1i(cubeTexLoc, 2);
  glUniform1i(mappingModeLoc, m_mappingMode);

  glm::mat3 texMatrix{m_trackBallLight.getRotation()};
  glUniformMatrix3fv(texMatrixLoc, 1, GL_TRUE, &texMatrix[0][0]);

  auto lightDirRotated{m_trackBallLight.getRotation() * m_lightDir};
  glUniform4fv(lightDirLoc, 1, &lightDirRotated.x);
  glUniform4fv(IaLoc, 1, &m_Ia.x);
  glUniform4fv(IdLoc, 1, &m_Id.x);
  glUniform4fv(IsLoc, 1, &m_Is.x);

  // Set uniform variables of the current object
  glUniformMatrix4fv(modelMatrixLoc, 1, GL_FALSE, &m_modelMatrix[0][0]);

  auto modelViewMatrix{glm::mat3(m_viewMatrix * m_modelMatrix)};
  glm::mat3 normalMatrix{glm::inverseTranspose(modelViewMatrix)};
  glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, &normalMatrix[0][0]);

  glUniform1f(shininessLoc, m_shininess);
  glUniform4fv(KaLoc, 1, &m_Ka.x);
  glUniform4fv(KdLoc, 1, &m_Kd.x);
  glUniform4fv(KsLoc, 1, &m_Ks.x);
  // m_model.render(m_trianglesToDraw);
  m_model.render(glm::vec3(0.0f, 0.0f, 0.0f), 0.0f, 1.0f, modelMatrixLoc, m_modelMatrix, colorLoc);
  m_model.render(glm::vec3(-2.0f, 0.0f, 0.0f), PI/2, 1.0f, modelMatrixLoc, m_modelMatrix, colorLoc);
  m_model.render(glm::vec3(2.0f, 0.0f, 0.0f), 3*PI/2, 1.0f, modelMatrixLoc, m_modelMatrix, colorLoc);

  m_model.render(glm::vec3(m_camera.getPosition().x, 0.0f, 2*mirrorPz-m_camera.getPosition().z), 
              m_camera.getAng() - 0.55f, 1.0f, modelMatrixLoc, m_modelMatrix, colorLoc);

  renderSkybox();
}

void OpenGLWindow::renderSkybox() {
  glUseProgram(m_skyProgram);

  // Get location of uniform variables
  GLint viewMatrixLoc{glGetUniformLocation(m_skyProgram, "viewMatrix")};
  GLint projMatrixLoc{glGetUniformLocation(m_skyProgram, "projMatrix")};
  GLint skyTexLoc{glGetUniformLocation(m_skyProgram, "skyTex")};

  // Set uniform variables
  auto viewMatrix{m_trackBallLight.getRotation()};
  glUniformMatrix4fv(viewMatrixLoc, 1, GL_FALSE, &viewMatrix[0][0]);
  glUniformMatrix4fv(projMatrixLoc, 1, GL_FALSE, &m_projMatrix[0][0]);
  glUniform1i(skyTexLoc, 0);

  glBindVertexArray(m_skyVAO);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_CUBE_MAP, m_model.getCubeTexture());

  glEnable(GL_CULL_FACE);
  glFrontFace(GL_CW);
  glDepthFunc(GL_LEQUAL);
  glDrawArrays(GL_TRIANGLES, 0, m_skyPositions.size());
  glDepthFunc(GL_LESS);

  glBindVertexArray(0);
  glUseProgram(0);
}

void OpenGLWindow::paintUI() {
  abcg::OpenGLWindow::paintUI();
}

void OpenGLWindow::resizeGL(int width, int height) {
  m_viewportWidth = width;
  m_viewportHeight = height;

  m_trackBallModel.resizeViewport(width, height);
  m_trackBallLight.resizeViewport(width, height);
}

void OpenGLWindow::terminateGL() {
  for (const auto& program : m_programs) {
    glDeleteProgram(program);
  }
  terminateSkybox();
}

void OpenGLWindow::terminateSkybox() {
  glDeleteProgram(m_skyProgram);
  glDeleteBuffers(1, &m_skyVBO);
  glDeleteVertexArrays(1, &m_skyVAO);
}

void OpenGLWindow::update() {
  float deltaTime{static_cast<float>(getDeltaTime())};

  // Update LookAt camera
  m_camera.dolly(deltaTime);
  m_camera.truck(deltaTime);
  m_camera.pan(deltaTime);

  // m_modelMatrix = m_trackBallModel.getRotation();

  m_eyePosition = glm::vec3(0.0f, 0.0f, 2.0f + m_zoom);
  m_viewMatrix = glm::lookAt(m_eyePosition, glm::vec3(0.0f, 0.0f, 0.0f),
                             glm::vec3(0.0f, 1.0f, 0.0f));
}