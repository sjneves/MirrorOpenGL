project(Mirror)
add_executable(${PROJECT_NAME} main.cpp model.cpp openglwindow.cpp
                               trackball.cpp camera.cpp)
enable_abcg(${PROJECT_NAME})
