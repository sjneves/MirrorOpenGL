#include <list>
#include <fmt/core.h>
#include <random>

#include "ship.hpp"
#include "mapa.hpp"

#include <glm/gtx/fast_trigonometry.hpp>
#include <glm/gtx/rotate_vector.hpp>

void Ship::initializeGL(GLuint program) {
  terminateGL();
  auto seed{std::chrono::steady_clock::now().time_since_epoch().count()};
  m_randomEngine.seed(seed);
  auto &re{m_randomEngine};

  std::uniform_real_distribution<float> randomG(0.15f, 0.75f);
  grav = randomG(re);

  m_program = program;
  m_colorLoc = glGetUniformLocation(m_program, "color");
  m_rotationLoc = glGetUniformLocation(m_program, "rotation");
  m_scaleLoc = glGetUniformLocation(m_program, "scale");
  m_translationLoc = glGetUniformLocation(m_program, "translation");

  m_rotation = 0.0f;
  m_translation = glm::vec2(0);
  m_velocity = glm::vec2(0);

  // clang-format off
  std::array<glm::vec2, 24> positions{
      // Ship body
      glm::vec2{-02.5f, +12.5f}, glm::vec2{-15.5f, +02.5f},
      glm::vec2{-15.5f, -15.5f}, glm::vec2{-09.5f, -07.5f},
      glm::vec2{-03.5f, -12.5f}, glm::vec2{+03.5f, -12.5f},
      glm::vec2{+09.5f, -07.5f}, glm::vec2{+15.5f, -15.5f},
      glm::vec2{+15.5f, +02.5f}, glm::vec2{+02.5f, +12.5f},

      // Cannon left
      glm::vec2{-12.5f, +10.5f}, glm::vec2{-12.5f, +04.0f},
      glm::vec2{-09.5f, +04.0f}, glm::vec2{-09.5f, +10.5f},

      // Cannon right
      glm::vec2{+09.5f, +10.5f}, glm::vec2{+09.5f, +04.0f},
      glm::vec2{+12.5f, +04.0f}, glm::vec2{+12.5f, +10.5f},
      
      // Thruster trail (left)
      glm::vec2{-12.0f, -07.5f}, 
      glm::vec2{-09.5f, -18.0f}, 
      glm::vec2{-07.0f, -07.5f},

      // Thruster trail (right)
      glm::vec2{+07.0f, -07.5f}, 
      glm::vec2{+09.5f, -18.0f}, 
      glm::vec2{+12.0f, -07.5f},
      };

  // Normalize
  for (auto &position :positions) {
    position /= glm::vec2{15.5f, 15.5f};
  }

  std::array indices{0, 1, 3,
                     1, 2, 3,
                     0, 3, 4,
                     0, 4, 5,
                     9, 0, 5,
                     9, 5, 6,
                     9, 6, 8,
                     8, 6, 7,
                     // Cannons
                     10, 11, 12,
                     10, 12, 13,
                     14, 15, 16,
                     14, 16, 17,
                     // Thruster trails
                     18, 19, 20,
                     21, 22, 23};
  // clang-format on

  // Generate VBO
  glGenBuffers(1, &m_vbo);
  glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions.data(),
               GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  // Generate EBO
  glGenBuffers(1, &m_ebo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices.data(),
               GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  // Get location of attributes in the program
  GLint positionAttribute{glGetAttribLocation(m_program, "inPosition")};

  // Create VAO
  glGenVertexArrays(1, &m_vao);

  // Bind vertex attributes to current VAO
  glBindVertexArray(m_vao);

  glEnableVertexAttribArray(positionAttribute);
  glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
  glVertexAttribPointer(positionAttribute, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);

  // End of binding to current VAO
  glBindVertexArray(0);
}

void Ship::paintGL(const GameData &gameData) {
  if (gameData.m_state != State::Playing) return;

  glUseProgram(m_program);

  glBindVertexArray(m_vao);

  glUniform1f(m_scaleLoc, m_scale);
  glUniform1f(m_rotationLoc, m_rotation);
  glUniform2fv(m_translationLoc, 1, &m_translation.x);

  // Restart thruster blink timer every 100 ms
  if (m_trailBlinkTimer.elapsed() > 100.0 / 1000.0) m_trailBlinkTimer.restart();

  if (gameData.m_input[static_cast<size_t>(Input::Up)]) {
    // Show thruster trail during 50 ms
    if (m_trailBlinkTimer.elapsed() < 50.0 / 1000.0) {
      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

      // 50% transparent
      glUniform4f(m_colorLoc, 1, 1, 1, 0.5f);

      glDrawElements(GL_TRIANGLES, 14 * 3, GL_UNSIGNED_INT, nullptr);

      glDisable(GL_BLEND);
    }
  }

  glUniform4fv(m_colorLoc, 1, &m_color.r);
  glDrawElements(GL_TRIANGLES, 12 * 3, GL_UNSIGNED_INT, nullptr);

  glBindVertexArray(0);

  glUseProgram(0);
}

void Ship::terminateGL() {
  glDeleteBuffers(1, &m_vbo);
  glDeleteBuffers(1, &m_ebo);
  glDeleteVertexArrays(1, &m_vao);
}

void Ship::update(const GameData &gameData, float deltaTime) {
  // Gravidade
  // physics(deltaTime);
  m_velocity -= glm::vec2{0.0f, grav*deltaTime};
  // fmt::print(stdout, "vel {}\n", m_velocity.y);

  if(abs(m_velocity.y)>0.2f){
    // fmt::print(stdout, "parado\n");
    m_color = glm::vec4{1.0f, 0.0f, 0.0f, 1.0f};
  }else{
    m_color = glm::vec4{1.0f, 1.0f, 1.0f, 1.0f};
  }

  // Rotate
  if (gameData.m_input[static_cast<size_t>(Input::Left)])
    m_rotation = glm::wrapAngle(m_rotation + 4.0f * deltaTime);
  if (gameData.m_input[static_cast<size_t>(Input::Right)])
    m_rotation = glm::wrapAngle(m_rotation - 4.0f * deltaTime);

  // Apply thrust
  if (gameData.m_input[static_cast<size_t>(Input::Up)] &&
      gameData.m_state == State::Playing) {
    // Thrust in the forward vector
    glm::vec2 forward = glm::rotate(glm::vec2{0.0f, 1.0f}, m_rotation);
    m_velocity += forward * deltaTime;
  }
  // if (gameData.m_input[static_cast<size_t>(Input::Down)] &&
  //     gameData.m_state == State::Playing) {
  //   // Thrust in the forward vector
  //   glm::vec2 forward = glm::rotate(glm::vec2{0.0f, 1.0f}, m_rotation);
  //   m_velocity -= forward * deltaTime;
  // }

}

void Ship::physics(float deltaTime, int lado){
  if(m_colisao.elapsed() < 0.10f){
    m_velocity += glm::vec2{0.0f, 0.005f}  * deltaTime;
    return;
  }
  if(lado==1){
    // if nao colisao esquerdo acelera esquerto baixo 1/2 gravidade
    m_velocity *= glm::vec2{1.0f, -1.5f} * deltaTime + glm::vec2{0.0f, grav};
    m_colisao.restart();
  }else if(lado==2){
    // if nao colisao direito acelera direito baixo 1/2 gravidade
    m_velocity *= glm::vec2{1.0f, -1.5f} * deltaTime + glm::vec2{0.0f, grav};
    m_colisao.restart();
  }

  // if nenhuma cosisao acelera gravidade
  

  // A ideia é, no momento da colisão acelerar a nave para cima com metade da velocidade q ela estava no momento da colisão
}
