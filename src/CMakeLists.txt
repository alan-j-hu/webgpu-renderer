add_library(noworry
  application.cpp
  camera.cpp
  flatmaterial.cpp
  flatmesheffect.cpp
  flatmeshpipeline.cpp
  init_wgpu.cpp
  mesh.cpp
  meshbuilder.cpp
  mesheffect.cpp
  model.cpp
  renderer.cpp
  resourcetable.cpp
  stb_image.c
  texture.cpp
  texturematerial.cpp
  texturemesheffect.cpp
  texturemeshpipeline.cpp)
target_include_directories(noworry PRIVATE ../include)
target_link_libraries(noworry dawn::webgpu_dawn glm::glm glfw webgpu_glfw)
set_property(TARGET noworry PROPERTY CXX_STANDARD 23)
set_property(TARGET noworry PROPERTY C_STANDARD 23)
