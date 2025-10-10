cmake_minimum_required(VERSION 3.23)
project(Demo)

include(FetchContent)

FetchContent_Declare(
        Assimp
        GIT_REPOSITORY "https://github.com/assimp/assimp.git"
        GIT_TAG "v6.0.2")
set(BUILD_SHARED_LIBS OFF)
set(ASSIMP_BUILD_TESTS OFF)
set(ASSIMP_INJECT_DEBUG_POSTFIX OFF)
set(ASSIMP_INSTALL OFF)
set(ASSIMP_WARNINGS_AS_ERRORS OFF)

FetchContent_MakeAvailable(Assimp)

FetchContent_Declare(
  glm
  GIT_REPOSITORY https://github.com/g-truc/glm.git
  GIT_TAG bf71a834948186f4097caa076cd2663c69a10e1e)

FetchContent_MakeAvailable(glm)

FetchContent_Declare(
  dawn
  GIT_REPOSITORY https://dawn.googlesource.com/dawn
  GIT_TAG 78fff7bf05e787a29cfd37a037c2cb63a55e923a
  GIT_SHALLOW ON
  GIT_PROGRESS TRUE
  GIT_SUBMODULES ""
  GIT_REMOTE_UPDATE_STRATEGY CHECKOUT)
set(DAWN_FETCH_DEPENDENCIES ON)
set(DAWN_BUILD_MONOLITHIC_LIBRARY STATIC)
set(DAWN_ENABLE_INSTALL ON)
set(TINT_BUILD_CMD_TOOLS OFF)
set(TINT_BUILD_TESTS OFF)

FetchContent_MakeAvailable(dawn)

add_subdirectory(src)
add_subdirectory(app)