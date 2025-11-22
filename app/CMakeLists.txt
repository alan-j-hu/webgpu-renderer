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
  Project.cpp
  TilePicker.cpp
  TileThumbnail.cpp
  Commands/Command.cpp
  Commands/CreateLayerCommand.cpp
  Commands/CreateTileCommand.cpp
  Commands/DeleteLayerCommand.cpp
  Commands/PlaceTileCommand.cpp
  Commands/UpdateTileCommand.cpp
  Tilemap/HeightMode.cpp
  Tilemap/LayerNode.cpp
  Tilemap/TilemapEditor.cpp
  Tilemap/TileMode.cpp
  Tilemap/View2DMode.cpp
  Tilemap/View3DMode.cpp
  Tilemap/ZPalette.cpp
  Tileset/AddTile.cpp
  Tileset/TileDefinitionEditor.cpp
  Tileset/TileList.cpp
  Tileset/TilesetPane.cpp
  ${IMGUI_SRC})
set_property(TARGET demo PROPERTY CXX_STANDARD 23)
set_property(TARGET demo PROPERTY C_STANDARD 23)
target_compile_definitions(demo PUBLIC "IMGUI_IMPL_WEBGPU_BACKEND_DAWN")
target_include_directories(demo PRIVATE
  ../include
  ${IMGUI_DIR}
  ${IMGUI_DIR}/backends)

add_custom_target(link_resources ALL
  COMMAND ${CMAKE_COMMAND} -E create_symlink
  "${CMAKE_CURRENT_SOURCE_DIR}/Resources"
  "${CMAKE_CURRENT_BINARY_DIR}/Resources")
add_dependencies(demo link_resources)

target_link_libraries(demo noworry)
