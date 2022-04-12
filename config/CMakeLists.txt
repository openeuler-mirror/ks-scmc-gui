#添加ks-scmc-gui.desktop.in文件
configure_file (
        "${PROJECT_SOURCE_DIR}/config/${PROJECT_NAME}.desktop.in"
        "${PROJECT_BINARY_DIR}/config/${PROJECT_NAME}.desktop"
)

install(FILES "${PROJECT_BINARY_DIR}/config/${PROJECT_NAME}.desktop"
        DESTINATION ${DATA_DIR}/applications/ )
