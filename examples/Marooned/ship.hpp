#ifndef SHIP_HPP_
#define SHIP_HPP_

#include <list>
#include <random>

#include "abcg.hpp"
#include "gameData.hpp"

class ClMapa;
class OpenGLWindow;
class StarLayers;
class Pessoas;

class Ship {
  public:
    void initializeGL(GLuint program);
    void paintGL(const GameData &gameData);
    void terminateGL();

    void update(const GameData &gameData, float deltaTime);
    void setRotation(float rotation) { m_rotation = rotation; }
    void physics(float deltaTime, int lado);

  private:
    friend ClMapa;
    friend OpenGLWindow;
    friend StarLayers;
    friend Pessoas;

    float grav = 0.25f;

    glm::vec2 posicao{};

    GLuint m_program{};
    GLint m_translationLoc{};
    GLint m_colorLoc{};
    GLint m_scaleLoc{};
    GLint m_rotationLoc{};

    GLuint m_vao{};
    GLuint m_vbo{};
    GLuint m_ebo{};

    glm::vec4 m_color{1};
    float m_rotation{};
    float m_scale{0.125f};
    glm::vec2 m_translation{glm::vec2(0)};
    glm::vec2 m_velocity{glm::vec2(0)};

    abcg::ElapsedTimer m_colisao;
    abcg::ElapsedTimer m_trailBlinkTimer;

    std::default_random_engine m_randomEngine;
};

#endif