project(Marooned)
add_executable(${PROJECT_NAME} main.cpp openglwindow.cpp mapa.cpp
                               ship.cpp pessoas.cpp gameData.cpp
                               starlayers.cpp)
enable_abcg(${PROJECT_NAME})