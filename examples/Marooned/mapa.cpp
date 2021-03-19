#include <fmt/core.h>
#include <algorithm>
#include <array>
#include <list>


#include "mapa.hpp"

#include "mapa.hpp"

#include <cppitertools/itertools.hpp>
#include <glm/gtx/fast_trigonometry.hpp>

void ClMapa::initializeGL(GLuint program, int quantity) {
    terminateGL();
    int nColunas, nLinhas, i;
    int cont = 0;
    int pontoZeroX, pontoZeroY;
    int o = d_mapa.find("}{");
    int p = d_mapa.find("}\n");

    // Start pseudo-random number generator
    auto seed{std::chrono::steady_clock::now().time_since_epoch().count()};
    m_randomEngine.seed(seed);

    m_program = program;
    m_colorLoc = glGetUniformLocation(m_program, "color");
    m_rotationLoc = glGetUniformLocation(m_program, "rotation");
    m_scaleLoc = glGetUniformLocation(m_program, "scale");
    m_translationLoc = glGetUniformLocation(m_program, "translation");
    
    nColunas = std::stoi(d_mapa.substr(1,o-1));
    nLinhas = std::stoi(d_mapa.substr(o+2, p-1));
    d_mapa = d_mapa.substr(p+2);
    
    fmt::print(stdout, "Tamanho --->( {} , {} )\n", nColunas, nLinhas);

    cont = std::count(d_mapa.begin(), d_mapa.end(), 'X');

    fmt::print(stdout, "Numero de X -->{}\n", cont);

    // Create asteroids
    m_bloco.clear();
    m_bloco.resize(cont);

    pontoZeroX = d_mapa.find('*', 0);
    pontoZeroY = pontoZeroX/nColunas;
    pontoZeroX %= nColunas;

    fmt::print(stdout, "Ponto zero --->( {} , {} )\n", pontoZeroX, pontoZeroY);

    i = 0;
    for (auto &bloco : m_bloco) {
        float x, y;
        i = d_mapa.find('X', i);
        bloco = createBloco();
        x = 2.0f*((i%nColunas)-pontoZeroX)*escala;
        y = -2.0f*((i/nColunas)-pontoZeroY)*escala;

        bloco.m_translation = {x, y};
        i++;
    }
    glm::vec4 limites{ 
        -(float)pontoZeroX*escala, (float)(nColunas-pontoZeroX)*escala,
        (float)pontoZeroY*escala, -(float)(nLinhas-pontoZeroY)*escala
    };

    fmt::print(stdout, "{} - {} / {} - {} \n", limites.x, limites.y, limites.x, limites.y);
}

void ClMapa::paintGL() {
  glUseProgram(m_program);

  for (auto &bloco : m_bloco) {
    glBindVertexArray(bloco.m_vao);

    glUniform4fv(m_colorLoc, 1, &bloco.m_color.r);
    glUniform1f(m_scaleLoc, bloco.m_scale);
    glUniform1f(m_rotationLoc, bloco.m_rotation);

    glUniform2f(m_translationLoc, bloco.m_translation.x, bloco.m_translation.y);

    glDrawArrays(GL_TRIANGLE_FAN, 0, bloco.m_polygonSides + 2);

    glBindVertexArray(0);
  }

  glUseProgram(0);
}

void ClMapa::terminateGL() {
  for (auto bloco : m_bloco) {
    glDeleteBuffers(1, &bloco.m_vbo);
    glDeleteVertexArrays(1, &bloco.m_vao);
  }
}

void ClMapa::update(const Ship &ship, float deltaTime) {
    for (auto &bloco : m_bloco) {
        bloco.m_translation -= ship.m_velocity * deltaTime;
        bloco.m_rotation = glm::wrapAngle(
            bloco.m_rotation + bloco.m_angularVelocity * deltaTime);
        bloco.m_translation += bloco.m_velocity * deltaTime;

        // Wrap-around
        // if (mapa.m_translation.x < -1.0f) mapa.m_translation.x += 2.0f;
        // if (mapa.m_translation.x > +1.0f) mapa.m_translation.x -= 2.0f;
        // if (mapa.m_translation.y < -1.0f) mapa.m_translation.y += 2.0f;
        // if (mapa.m_translation.y > +1.0f) mapa.m_translation.y -= 2.0f;
    }
}

ClMapa::Bloco ClMapa::createBloco(glm::vec2 translation,
                                              float scale) {
    Bloco bloco;

    auto &re{m_randomEngine};  // Shortcut

    // Choose a random color (actually, a grayscale)
    std::uniform_real_distribution<float> randomIntensity(0.5f, 1.0f);
    bloco.m_color = glm::vec4(1) * randomIntensity(re);

    bloco.m_color.a = 1.0f;
    bloco.m_rotation = 0.0f;
    bloco.m_scale = scale;
    bloco.m_translation = translation;

  
    // mapa.m_angularVelocity = m_randomDist(re);
    bloco.m_angularVelocity = 0.0f;

    // Choose a random direction
    bloco.m_velocity = glm::vec2{0.0f, 0.0f};


    std::vector<glm::vec2> positions(0);  // Vetor para a posição
    positions.emplace_back(-1, 1);
    positions.emplace_back(1, 1);
    positions.emplace_back(1, -1);
    positions.emplace_back(-1, -1);


    // Generate VBO
    glGenBuffers(1, &bloco.m_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, bloco.m_vbo);
    glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(glm::vec2),
                positions.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Get location of attributes in the program
    GLint positionAttribute{glGetAttribLocation(m_program, "inPosition")};

    // Create VAO
    glGenVertexArrays(1, &bloco.m_vao);

    // Bind vertex attributes to current VAO
    glBindVertexArray(bloco.m_vao);

    glBindBuffer(GL_ARRAY_BUFFER, bloco.m_vbo);
    glEnableVertexAttribArray(positionAttribute);
    glVertexAttribPointer(positionAttribute, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // End of binding to current VAO
    glBindVertexArray(0);

    return bloco;
}
