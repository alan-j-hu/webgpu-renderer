add_library(noworry
  application.cpp
  grid.cpp
  init_wgpu.cpp
  layout.cpp
  mesh.cpp
  renderer.cpp
  rendertarget.cpp
  resourcetable.cpp
  stb_image.c
  texture.cpp
  transform.cpp
  camera/abstractcamera.cpp
  camera/orthographiccamera.cpp
  camera/perspectivecamera.cpp
  material/flatmaterial.cpp
  material/flatmesheffect.cpp
  material/mesheffect.cpp
  material/texturematerial.cpp
  material/texturemesheffect.cpp
  scene/renderobject.cpp
  scene/scene.cpp)
target_include_directories(noworry PRIVATE ../include)
target_link_libraries(noworry dawn::webgpu_dawn glm::glm glfw webgpu_glfw)
set_property(TARGET noworry PROPERTY CXX_STANDARD 23)
set_property(TARGET noworry PROPERTY C_STANDARD 23)
