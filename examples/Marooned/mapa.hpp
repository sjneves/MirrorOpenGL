#ifndef MAPA_HPP_
#define MAPA_HPP_

#include <list>
#include <random>

#include "abcg.hpp"
#include "gameData.hpp"
#include "ship.hpp"
#include "pessoas.hpp"

class OpenGLWindow;

class ClMapa {
  public:
    void initializeGL(GLuint program, GameData gamedate);
    void paintGL();
    void terminateGL();

    void update(Ship &ship, float deltaTime);

    int nColunas, nLinhas;

    glm::vec2 limiteX{};
    glm::vec2 limiteY{};

    struct Bloco {
      GLuint m_vao{};
      GLuint m_vbo{};

      glm::vec4 m_color{1};
      bool m_hit{false};
      int m_polygonSides = 4;
      float m_rotation{};
      float m_scale{};
      glm::vec2 m_translation{glm::vec2(0)};
      glm::vec2 m_velocity{glm::vec2(0)};
    };

    int checkCollisions(Ship &ship, Bloco bloco);
    std::list<Bloco> getListaBloco();

  private:
    friend OpenGLWindow;

    std::string d_mapa{ "{100}{18}\n"
                                    "|                                                                                                 |\n"
                                    "|                                                       1                                         |\n"
                                    "|                                               XX XX XXXX                                        |\n"
                                    "|                                               X X X X                                           |\n"
                                    "|                X     X                        X   X XXXX                                        |\n"
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
                                    "|                3                       4                                                   3    |\n"
                                    "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX\n"};

    GLuint m_program{};
    GLint m_colorLoc{};
    GLint m_rotationLoc{};
    GLint m_translationLoc{};
    GLint m_scaleLoc{};

    float escala = 0.05f;

    GameData m_gamedata;

    std::list<Bloco> m_bloco;

    std::default_random_engine m_randomEngine;
    std::uniform_real_distribution<float> m_randomDist{-1.0f, 1.0f};

    ClMapa::Bloco createBloco(glm::vec2 translation = glm::vec2(0),
                                        float scale = 0.05f);
};

#endif