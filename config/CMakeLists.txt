#添加ks-scmc-gui.desktop.in文件
configure_file (
        "${PROJECT_SOURCE_DIR}/config/${PROJECT_NAME}.desktop.in"
        "${PROJECT_BINARY_DIR}/config/${PROJECT_NAME}.desktop"
)

install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/ks-scmc-gui.sh"
	RENAME "ks-scmc-gui"
	PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ GROUP_EXECUTE GROUP_READ WORLD_READ WORLD_EXECUTE
	DESTINATION ${BIN_DIR})


install(FILES "${PROJECT_BINARY_DIR}/config/${PROJECT_NAME}.desktop"
        DESTINATION ${DATA_DIR}/applications/ )
