set(IMGUI_DIR ../third_party/imgui)
set(IMGUI_SRC
  ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
  ${IMGUI_DIR}/backends/imgui_impl_wgpu.cpp
  ${IMGUI_DIR}/imgui.cpp
  ${IMGUI_DIR}/imgui_draw.cpp
  ${IMGUI_DIR}/imgui_demo.cpp
  ${IMGUI_DIR}/imgui_tables.cpp
  ${IMGUI_DIR}/imgui_widgets.cpp)

add_executable(demo
  AppState.cpp
  FileDialog.cpp
  main.cpp
  Modal.cpp
  Pane.cpp
  Reducer.cpp
  TilePicker.cpp
  TileThumbnail.cpp
  Commands/Command.cpp
  Commands/CreateLayerCommand.cpp
  Commands/CreateTileCommand.cpp
  Commands/DeleteLayerCommand.cpp
  Commands/PlaceTileCommand.cpp
  Commands/UpdateTileCommand.cpp
  Tilemap/LayerNode.cpp
  Tilemap/TilemapEditor.cpp
  Tilemap/TileMode.cpp
  Tilemap/View3DMode.cpp
  Tileset/AddTile.cpp
  Tileset/RotationDropdown.cpp
  Tileset/TileDefinitionEditor.cpp
  Tileset/TileList.cpp
  Tileset/TileMesh.cpp
  Tileset/TileRotations.cpp
  Tileset/TilesetPane.cpp
  ${IMGUI_SRC})
set_property(TARGET demo PROPERTY CXX_STANDARD 23)
set_property(TARGET demo PROPERTY C_STANDARD 23)
target_compile_definitions(demo PUBLIC "IMGUI_IMPL_WEBGPU_BACKEND_DAWN")
target_include_directories(demo PRIVATE
  ../include
  ${IMGUI_DIR}
  ${IMGUI_DIR}/backends)
target_link_libraries(demo assimp noworry)
