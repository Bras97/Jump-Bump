cmake_minimum_required(VERSION 3.10)
project(server)

set(CMAKE_CXX_STANDARD 11)

add_executable(server serverMain.cpp serverConstants.h serverPlayer.h serverPlayer.cpp serverConstants.cpp serverGame.cpp serverGame.h)
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(server Threads::Threads)