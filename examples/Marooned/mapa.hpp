#ifndef MAPA_HPP_
#define MAPA_HPP_

#include <list>
#include <random>

#include "abcg.hpp"
#include "gameData.hpp"
#include "ship.hpp"

class OpenGLWindow;

class ClMapa {
 public:
  void initializeGL(GLuint program, int quantity);
  void paintGL();
  void terminateGL();

  void update(const Ship &ship, float deltaTime);

  glm::vec2 limiteX{};
  glm::vec2 limiteY{};

 private:
  friend OpenGLWindow;

  std::string d_mapa{ "{100}{18}\n"
                                  "|                                                                                                 |\n"
                                  "|                                                                                                 |\n"
                                  "|                                                                                                 |\n"
                                  "|                                                                                                 |\n"
                                  "|                X     X                                                                          |\n"
                                  "|                   *                ###                                                          |\n"
                                  "|                X     X             ###                                                          |\n"
                                  "|                                                                                                 |\n"
                                  "|                                                                                                 |\n"
                                  "|                                                                                                 |\n"
                                  "|                                                                                                 |\n"
                                  "|                                                                                                 |\n"
                                  "|                                                                                                 |\n"
                                  "|                                                                                                 |\n"
                                  "|                                                                                                 |\n"
                                  "|                                                                                                 |\n"
                                  "|                                                                                                 |\n"
                                  "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX\n"};

  GLuint m_program{};
  GLint m_colorLoc{};
  GLint m_rotationLoc{};
  GLint m_translationLoc{};
  GLint m_scaleLoc{};

  float escala = 0.05f;

  struct Bloco {
    GLuint m_vao{};
    GLuint m_vbo{};

    float m_angularVelocity{};
    glm::vec4 m_color{1};
    bool m_hit{false};
    int m_polygonSides = 4;
    float m_rotation{};
    float m_scale{};
    glm::vec2 m_translation{glm::vec2(0)};
    glm::vec2 m_velocity{glm::vec2(0)};
  };

  std::list<Bloco> m_bloco;

  std::default_random_engine m_randomEngine;
  std::uniform_real_distribution<float> m_randomDist{-1.0f, 1.0f};

  ClMapa::Bloco createBloco(glm::vec2 translation = glm::vec2(0),
                                     float scale = 0.05f);
};

#endif