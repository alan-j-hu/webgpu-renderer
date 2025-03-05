cmake_minimum_required(VERSION 3.16)
project(Demo)

set(DAWN_FETCH_DEPENDENCIES ON)
add_subdirectory(third_party/dawn EXCLUDE_FROM_ALL)

set(sources main.cpp
  application.cpp camera.cpp init_wgpu.c model.cpp pipeline.cpp)

add_executable(demo ${sources})

set_property(TARGET demo PROPERTY CXX_STANDARD 23)
set_property(TARGET demo PROPERTY C_STANDARD 23)
target_link_libraries(demo dawn::webgpu_dawn glfw webgpu_glfw)
