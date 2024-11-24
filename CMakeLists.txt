cmake_minimum_required(VERSION 3.25)
project(v4_tunnel_v6)

set(CMAKE_CXX_STANDARD 20)

find_package(FastEngine REQUIRED)

add_executable(v4_tunnel_v6 main.cpp)

target_link_libraries(v4_tunnel_v6 FastEngine::FastEngineServer SDL2::SDL2 SDL2_image::SDL2_image)
