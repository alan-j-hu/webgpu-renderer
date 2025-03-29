cmake_minimum_required(VERSION 3.16)
project(Demo)

include(FetchContent)

FetchContent_Declare(
  glm
  GIT_REPOSITORY https://github.com/g-truc/glm.git
  GIT_TAG bf71a834948186f4097caa076cd2663c69a10e1e)

FetchContent_MakeAvailable(glm)

set(DAWN_FETCH_DEPENDENCIES ON)
add_subdirectory(third_party/dawn EXCLUDE_FROM_ALL)

add_subdirectory(src)

add_executable(demo main.cpp)
set_property(TARGET demo PROPERTY CXX_STANDARD 23)
set_property(TARGET demo PROPERTY C_STANDARD 23)
target_include_directories(demo PRIVATE include)
target_link_libraries(demo noworry)
