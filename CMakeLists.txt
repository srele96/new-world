cmake_minimum_required(VERSION 3.20)

project(new-world LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(FetchContent)

FetchContent_Declare(
  bs_thread_pool
  GIT_REPOSITORY https://github.com/bshoshany/thread-pool.git
  GIT_TAG bd4533f1f70c2b975cbd5769a60d8eaaea1d2233 # v5.1.0
)

FetchContent_MakeAvailable(bs_thread_pool)

add_library(bs_thread_pool INTERFACE)
target_include_directories(bs_thread_pool
                           INTERFACE ${bs_thread_pool_SOURCE_DIR}/include)

find_package(SDL2 REQUIRED)

add_library(
  imgui
  vendor/imgui/imgui.cpp
  vendor/imgui/imgui_draw.cpp
  vendor/imgui/imgui_tables.cpp
  vendor/imgui/imgui_widgets.cpp
  vendor/imgui/backends/imgui_impl_sdl2.cpp
  vendor/imgui/backends/imgui_impl_opengl3.cpp)

target_include_directories(imgui
                           PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/vendor/imgui)

target_link_libraries(imgui PUBLIC SDL2::SDL2)

add_library(glad vendor/glad/src/glad.c)

target_include_directories(glad PUBLIC vendor/glad/include)

find_package(CURL REQUIRED)

# https://www.boost.org/doc/user-guide/getting-started.html
find_package(Boost CONFIG REQUIRED COMPONENTS json)

add_executable(new-world main.cpp)

target_link_libraries(new-world PRIVATE imgui glad CURL::libcurl Boost::headers
                                        Boost::json bs_thread_pool)
