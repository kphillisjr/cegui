set (CEGUI_TARGET_NAME ${CEGUI_DIRECT3D9_RENDERER_LIBNAME})

include_directories(${DIRECTXSDK_INCLUDE_DIR})
link_directories(${DIRECTXSDK_LIBRARY_DIR})

cegui_gather_files()
cegui_add_library(${CEGUI_TARGET_NAME} CORE_SOURCE_FILES CORE_HEADER_FILES)

cegui_target_link_libraries(${CEGUI_TARGET_NAME} ${CEGUI_BASE_LIBNAME} d3d9)
cegui_target_link_libraries(${CEGUI_TARGET_NAME} optimized d3dx9 debug d3dx9d)

