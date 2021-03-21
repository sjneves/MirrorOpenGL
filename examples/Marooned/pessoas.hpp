#ifndef _HPP_PESSOAS
#define _HPP_PESSOAS

#include <list>
#include <vector>
#include <random>

#include "abcg.hpp"
#include "ship.hpp"
#include "gameData.hpp"

class Pessoas{
    public:
        void initializeGL(GLuint program);
        void paintGL();
        void terminateGL();
        void update(Ship ship, float deltaTime,GameData *gameData);

        struct s_pessoa
        {
            GLuint m_vao{};
            GLuint m_vbo{};
            GLuint m_ebo{};
            float m_scale{};
            float m_rotation{0};
            glm::vec4 m_color{1.0f, 1.0f, 0.0f, 1.0f};

            glm::vec2 posicao{};
            glm::vec2 iniPosicao{};

            float movimento{};

            bool morte = false;
            bool resgate = false;
        };

        void addPessoa(int px, int py, float escala, std::string mapa);

    private:
        int nPessoas;

        std::default_random_engine m_randomEngine;

        GLuint m_vao{};
        GLuint m_vbo{};
        GLuint m_ebo{};

        glm::vec4 m_color{0.0f, 1.0f, 1.0f, 1.0f};
        GLuint m_program{};
        GLint m_translationLoc{};
        GLint m_colorLoc{};
        GLint m_scaleLoc{};
        GLint m_rotationLoc{};

        float m_scale{};
        float mapa_scale{0};
        float m_rotation{0};
        glm::vec2 m_translation{glm::vec2(0)};
        glm::vec2 m_velocity{glm::vec2(0)};

        abcg::ElapsedTimer m_animacao;

        std::vector<s_pessoa> listpessoas{};
};

#endif