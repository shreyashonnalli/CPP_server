cmake_minimum_required(VERSION 3.5)
project(TCP
  VERSION 1.0
  LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}\
#   -g \
#   -O2 \
#   -Wall \
#   -pedantic \
#   -Wextra \
#   -fno-omit-frame-pointer \
#   -fsanitize=address,undefined")

# set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/outputs)

find_package(Boost REQUIRED)
include_directories("${Boost_INCLUDE_DIRS}")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(matrix_library data_structure/matrix.cpp include/matrix.hpp)
add_library(clientinput_handler client_input_handler/clientinputhandler.cpp include/clientinputhandler.hpp)
add_library(clienthandler client_handler/clienthandler.cpp include/clienthandler.hpp include/server.hpp)

add_executable(game_server main.cpp game_server/server.cpp include/server.hpp)
target_link_libraries(game_server matrix_library clientinput_handler clienthandler)

add_executable(game_client game_client/client.cpp include/client.hpp)

target_include_directories(matrix_library PUBLIC include)
target_include_directories(game_server PUBLIC include)
target_include_directories(game_client PUBLIC include)
target_include_directories(clientinput_handler PUBLIC include)
target_include_directories(clienthandler PUBLIC include)