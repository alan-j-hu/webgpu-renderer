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
  # 1.0.2
  GIT_TAG c10f4f2352dec2bbf11678a6ee866797924cf2e7)

FetchContent_MakeAvailable(glm)

FetchContent_Declare(
  dawn
  GIT_REPOSITORY https://github.com/google/dawn.git
  # v20251015.200420
  GIT_TAG 6c7ec7c76611d8a4ed243624604b08e2ef0fda2d
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