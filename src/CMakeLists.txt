add_library(noworry
  application.cpp
  camera.cpp
  init_wgpu.cpp
  material.cpp
  mesh.cpp
  model.cpp
  pipeline.cpp
  renderer.cpp
  stb_image.c
  texture.cpp)
target_include_directories(noworry PRIVATE ../include)
target_link_libraries(noworry dawn::webgpu_dawn glm::glm glfw webgpu_glfw)
set_property(TARGET noworry PROPERTY CXX_STANDARD 23)
set_property(TARGET noworry PROPERTY C_STANDARD 23)
