#ifndef OPENGLWINDOW_HPP_
#define OPENGLWINDOW_HPP_

#include <imgui.h>

#include <random>

#include "abcg.hpp"
#include "mapa.hpp"
#include "ship.hpp"
#include "starlayers.hpp"
#include "gameData.hpp"

class OpenGLWindow : public abcg::OpenGLWindow {
  protected:
    void handleEvent(SDL_Event& event) override;
    void initializeGL() override;
    void paintGL() override;
    void paintUI() override;
    void resizeGL(int width, int height) override;
    void terminateGL() override;

  private:
    GLuint m_starsProgram{};
    GLuint m_objectsProgram{};

    int m_viewportWidth{};
    int m_viewportHeight{};

    GameData m_gameData;

    ClMapa m_mapa;
    Ship m_ship;
    StarLayers m_starLayers;
    Pessoas m_pessoas;

    abcg::ElapsedTimer m_restartWaitTimer;

    ImFont* m_font{};

    std::default_random_engine m_randomEngine;

    void restart();
    void update();
};

#endif