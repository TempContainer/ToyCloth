cmake_minimum_required(VERSION 3.18)
project(Toy VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
find_package(GLUT REQUIRED)
find_package(OpenGL REQUIRED)
find_package(glm REQUIRED)

add_executable(cloth main.cpp physics.h)
target_link_libraries(cloth PRIVATE ${GLUT_LIBRARIES} ${OPENGL_LIBRARIES} glm::glm)