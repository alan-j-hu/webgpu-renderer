add_library(noworry
  application.cpp
  grid.cpp
  init_wgpu.cpp
  renderer.cpp
  rendertarget.cpp
  resourcetable.cpp
  stb_image.c
  texture.cpp
  transform.cpp
  camera/abstractcamera.cpp
  camera/orthographiccamera.cpp
  camera/perspectivecamera.cpp
  Draw2D/Frame2D.cpp
  Draw2D/SpritePipeline.cpp
  Draw2D/SpriteRenderer.cpp
  Draw2D/Spritesheet.cpp
  Gfx3D/BasicMesh.cpp
  Gfx3D/DynamicMesh.cpp
  Gfx3D/Model.cpp
  Gfx3D/ModelData.cpp
  Gfx3D/ModelInstance.cpp
  Gfx3D/ObjectBindGroup.cpp
  Gfx3D/RenderBatch.cpp
  Pipeline/MeshVertexShader.cpp
  Pipeline/PipelineFactory.cpp
  Material/FlatMaterial.cpp
  Material/FlatEffect.cpp
  Material/TextureMaterial.cpp
  Material/TextureEffect.cpp
  Resource/ResourceLoader.cpp
  scene/renderobject.cpp
  scene/scene.cpp)
target_include_directories(noworry PRIVATE ../include)
target_link_libraries(noworry
  assimp dawn::webgpu_dawn glm::glm glfw webgpu_glfw)
set_property(TARGET noworry PROPERTY CXX_STANDARD 23)
set_property(TARGET noworry PROPERTY C_STANDARD 23)
