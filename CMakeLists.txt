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
  # v20251218.170355
  GIT_TAG e78ae5f0450857eb95217ef469fbb173a3cf103d
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

FetchContent_Declare(
  json
  URL https://github.com/nlohmann/json/releases/download/v3.12.0/json.tar.xz)
FetchContent_MakeAvailable(json)

add_subdirectory(src)
add_subdirectory(app)
