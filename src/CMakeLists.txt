set(INCLUDE_DIRS "../include/" "pages/" "common/" "base/")
set(TS_FILES "../translations/${PROJECT_NAME}.zh_CN.ts" )
set(PROTO_LIB_DIR "${PROJECT_BINARY_DIR}/proto/")

file(GLOB_RECURSE cur_src "*.cpp")
file(GLOB_RECURSE HEADERS "*.h")
file(GLOB_RECURSE FORMS "*.ui")
file(GLOB_RECURSE RESOURCES "../data/*.qrc")
file(GLOB_RECURSE RESOURCES "../data/*.qrc")

add_definitions(-std=gnu++11)

qt5_create_translation( QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES} )

add_executable(${PROJECT_NAME}
	${cur_src} 
	${RESOURCES} 
	${HEADERS} 
        ${FORMS}
        ${QM_FILES})


target_include_directories(${PROJECT_NAME} PRIVATE
        ${INCLUDE_DIRS}
        ${CMAKE_BINARY_DIR}
        ${CMAKE_CURRENT_BINARY_DIR}
        ${KLOG_QT5_INCLUDE_DIRS}
        ${KIRAN_WIDGETS_QT5_INCLUDE_DIRS}
        ${GENERAL_PROTO_HDRS})

target_link_libraries(${PROJECT_NAME}
        proto_lib
        grpc++
        grpc++_reflection
        gpr
        protobuf
        cryptopp
        Qt5::Widgets
        Qt5::Charts
        ${KIRAN_WIDGETS_QT5_LIBRARIES}
        ${KLOG_QT5_LIBRARIES})

install(FILES ${QM_FILES}
    DESTINATION ${TRANSLATIONS_DIR})

install(TARGETS ${PROJECT_NAME}
    DESTINATION ${BIN_DIR})
