cmake_minimum_required(VERSION 3.16)
project(Demo)

include(FetchContent)

FetchContent_Declare(
        Assimp
        GIT_REPOSITORY "https://github.com/assimp/assimp.git"
        GIT_TAG "v5.4.1")
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

set(DAWN_FETCH_DEPENDENCIES ON)
add_subdirectory(third_party/dawn EXCLUDE_FROM_ALL)

add_subdirectory(src)
add_subdirectory(app)