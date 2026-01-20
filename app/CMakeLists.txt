set(IMGUI_DIR ../third_party/imgui)
set(IMGUI_SRC
  ${IMGUI_DIR}/imgui.cpp
  ${IMGUI_DIR}/imgui_draw.cpp
  ${IMGUI_DIR}/imgui_demo.cpp
  ${IMGUI_DIR}/imgui_tables.cpp
  ${IMGUI_DIR}/imgui_widgets.cpp
  ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
  ${IMGUI_DIR}/backends/imgui_impl_wgpu.cpp
  ${IMGUI_DIR}/misc/cpp/imgui_stdlib.cpp)

add_executable(demo
  AppState.cpp
  Deserialize.cpp
  main.cpp
  Project.cpp
  Serialize.cpp
  TilePicker.cpp
  ThumbnailUtil.cpp
  Commands/Command.cpp
  Commands/CreateLayerCommand.cpp
  Commands/CreateTileCommand.cpp
  Commands/DeleteLayerCommand.cpp
  Commands/PlaceTilesCommand.cpp
  Commands/UpdateTileCommand.cpp
  Editor/Editor.cpp
  Editor/ErrorModal.cpp
  Editor/FileDialog.cpp
  Editor/Tilemap/HeightMode.cpp
  Editor/Tilemap/LayerNode.cpp
  Editor/Tilemap/LevelNode.cpp
  Editor/Tilemap/TileMode.cpp
  Editor/Tilemap/View2DMode.cpp
  Editor/Tilemap/View3DMode.cpp
  Editor/Tilemap/ZPalette.cpp
  Editor/Tileset/NewTile.cpp
  Editor/Tileset/TileDefinitionEditor.cpp
  Editor/Tileset/TileList.cpp
  Editor/Tileset/TileThumbnail.cpp
  Editor/Tileset/TilesetThumbnails.cpp
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

target_link_libraries(demo noworry nlohmann_json::nlohmann_json)
