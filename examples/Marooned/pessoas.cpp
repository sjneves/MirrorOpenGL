#include <fmt/core.h>
#include <random>

#include "pessoas.hpp"
#include "gameData.hpp"

void Pessoas::initializeGL(GLuint program){
    terminateGL();
    auto seed{std::chrono::steady_clock::now().time_since_epoch().count()};
    m_randomEngine.seed(seed);

    m_program = program;
    m_colorLoc = glGetUniformLocation(m_program, "color");
    m_rotationLoc = glGetUniformLocation(m_program, "rotation");
    m_scaleLoc = glGetUniformLocation(m_program, "scale");
    m_translationLoc = glGetUniformLocation(m_program, "translation");

    m_rotation = 0.0f;
    m_translation = glm::vec2(0);
    m_velocity = glm::vec2(0);

}
void Pessoas::paintGL(){
    glUseProgram(m_program);

    for (auto &pessoa : listpessoas) {
        glBindVertexArray(pessoa.m_vao);

        glUniform4f(m_colorLoc,pessoa.m_color.r,pessoa.m_color.g,pessoa.m_color.b,pessoa.m_color.a);
        glUniform1f(m_scaleLoc, pessoa.m_scale);
        glUniform1f(m_rotationLoc, pessoa.m_rotation);

        glUniform2f(m_translationLoc, pessoa.posicao.x, pessoa.posicao.y);

        glDrawElements(GL_TRIANGLES, 12 * 3, GL_UNSIGNED_INT, nullptr);

        glBindVertexArray(0);
    }

    glUseProgram(0);
}
void Pessoas::terminateGL(){
    glDeleteBuffers(1, &m_vbo);
    glDeleteBuffers(1, &m_ebo);
    glDeleteVertexArrays(1, &m_vao);
}
void Pessoas::update(Ship ship, float deltaTime, GameData *gameData){
    auto &re{m_randomEngine};
    float dist;

    for (auto &pessoa : listpessoas) {
        dist = glm::distance(ship.m_translation, pessoa.posicao);
        if(abs(dist)<0.3){
            if(abs(ship.m_velocity.y)<0.1 && abs(pessoa.posicao.y)<ship.m_scale*1.1f){
                if(pessoa.posicao.x<0){
                    pessoa.posicao.x+= pessoa.movimento*deltaTime;
                }else{
                    pessoa.posicao.x-= pessoa.movimento*deltaTime;
                }
                if(gameData->m_input[static_cast<size_t>(Input::Up)] && abs(pessoa.posicao.x)<ship.m_scale*0.95f){
                    fmt::print(stdout, "Vc Matou uma pessoa !!\n");
                    gameData->m_acao = Acao::Morte;
                    pessoa.morte = true;
                    
                }
                if(abs(pessoa.posicao.x)<ship.m_scale*0.05f){
                    fmt::print(stdout, "Vc salvou uma pessoa !!\n");
                    gameData->m_acao = Acao::Resgate;
                    pessoa.resgate = true;
                }
            }
            pessoa.m_color = glm::vec4{1.0f, 1.0f, 0.0f, 1.0f};
        }else{
            pessoa.m_color = glm::vec4{0.0f, 1.0f, 1.0f, 1.0f};
        }

        pessoa.posicao -= ship.m_velocity * deltaTime;
  }

  for(auto i = listpessoas.begin(); i != listpessoas.end(); ++i){
    if(i->morte){
      listpessoas.erase(i);
      i--;
    }
    if(i->resgate){
        listpessoas.erase(i);
        i--;
    }
  }
}

void Pessoas::addPessoa(int px, int py, float escala, std::string mapa){
    auto &re{m_randomEngine};
    int j, i = 1;
    int cont = 0;
    mapa_scale = escala;
    cont = mapa.find('*', 0);
    glm::vec2 pontoZero = { cont%px, cont/px };
    listpessoas.clear();
    while(i>0){
        i = mapa.find_first_of("123456789", i);

        if(i<0) break;
        cont = std::stoi(&mapa.data()[i]);

        for(j=0;j<cont;j++){
            s_pessoa newPessoa;

            newPessoa.m_scale = escala/2;
            newPessoa.m_color = glm::vec4{1.0f, 1.0f, 0.0f, 1.0f};

            std::uniform_real_distribution<float> randomPos(-4.0f, 4.0f);
            newPessoa.iniPosicao = glm::vec2{ (i%px)+randomPos(re), (i/px)};
            newPessoa.iniPosicao-=pontoZero;
            newPessoa.iniPosicao*=escala*2.0f;
            newPessoa.posicao = newPessoa.iniPosicao * glm::vec2{1.0f, -1.0f};
            std::uniform_real_distribution<float> randomMov(0.15f, 0.01f);
            newPessoa.movimento = randomMov(re);

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
            glGenBuffers(1, &newPessoa.m_vbo);
            glBindBuffer(GL_ARRAY_BUFFER, newPessoa.m_vbo);
            glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions.data(),
                        GL_STATIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            // Generate EBO
            glGenBuffers(1, &newPessoa.m_ebo);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, newPessoa.m_ebo);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices.data(),
                        GL_STATIC_DRAW);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

            // Get location of attributes in the program
            GLint positionAttribute{glGetAttribLocation(m_program, "inPosition")};

            // Create VAO
            glGenVertexArrays(1, &newPessoa.m_vao);

            // Bind vertex attributes to current VAO
            glBindVertexArray(newPessoa.m_vao);

            glEnableVertexAttribArray(positionAttribute);
            glBindBuffer(GL_ARRAY_BUFFER, newPessoa.m_vbo);
            glVertexAttribPointer(positionAttribute, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, newPessoa.m_ebo);

            // End of binding to current VAO
            glBindVertexArray(0);

            listpessoas.push_back(newPessoa);
        }
        
        i++;
    }
    fmt::print(stdout, "Nº de pessoas: {}\n", listpessoas.size());
}