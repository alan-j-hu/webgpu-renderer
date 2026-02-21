cmake_minimum_required(VERSION 3.24)
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
  dawn
  GIT_REPOSITORY https://github.com/google/dawn.git
  GIT_SHALLOW ON
  # v20260219.200501
  GIT_TAG 536c572abab2602db0b652eda401ebd01e046c11
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
  glm
  GIT_REPOSITORY https://github.com/g-truc/glm.git
  # 1.0.2
  GIT_TAG c10f4f2352dec2bbf11678a6ee866797924cf2e7)
FetchContent_MakeAvailable(glm)

FetchContent_Declare(
  imgui
  GIT_REPOSITORY https://github.com/ocornut/imgui.git
  # v1.92.6
  GIT_TAG 6ded5230d043aa32c755e65c910c2af5002fb9f9)
FetchContent_MakeAvailable(imgui)

FetchContent_Declare(
  json
  URL https://github.com/nlohmann/json/releases/download/v3.12.0/json.tar.xz)
FetchContent_MakeAvailable(json)

FetchContent_Declare(
  stb
  GIT_REPOSITORY https://github.com/nothings/stb.git
  GIT_TAG f1c79c02822848a9bed4315b12c8c8f3761e1296)
FetchContent_MakeAvailable(stb)

add_subdirectory(src)
add_subdirectory(app)
