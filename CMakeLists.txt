cmake_minimum_required(VERSION 3.16)
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
  immer
  GIT_REPOSITORY https://github.com/arximboldi/immer
  GIT_TAG df6ef46d97e1fe81f397015b9aeb32505cef653b)
set(immer_BUILD_TESTS OFF)
set(immer_BUILD_PERSIST_TESTS OFF)
set(immer_BUILD_EXAMPLES OFF)
set(immer_BUILD_DOCS OFF)

FetchContent_MakeAvailable(immer)

FetchContent_Declare(
  dawn
  GIT_REPOSITORY https://dawn.googlesource.com/dawn
  GIT_TAG 991810463a797d6e770127e604cdd3d8b8ff6e43
  GIT_SUBMODULES "")
set(DAWN_FETCH_DEPENDENCIES ON)

FetchContent_MakeAvailable(dawn)

add_subdirectory(src)
add_subdirectory(app)