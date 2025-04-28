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
  filedialog.cpp
  main.cpp
  modal.cpp
  tileset/addnewtile.cpp
  tileset/tiledefinition.cpp
  tileset/tileset.cpp
  ${IMGUI_SRC})
set_property(TARGET demo PROPERTY CXX_STANDARD 23)
set_property(TARGET demo PROPERTY C_STANDARD 23)
target_compile_definitions(demo PUBLIC "IMGUI_IMPL_WEBGPU_BACKEND_DAWN")
target_include_directories(demo PRIVATE
  ../include
  ${IMGUI_DIR}
  ${IMGUI_DIR}/backends)
target_link_libraries(demo assimp noworry)
