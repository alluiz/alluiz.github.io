project(asteroidsv2)
add_executable(${PROJECT_NAME} main.cpp openglwindow.cpp asteroids/asteroids.cpp bullets/bullets.cpp ship/ship.cpp starlayers/starlayers.cpp)
enable_abcg(${PROJECT_NAME})