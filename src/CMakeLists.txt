set(RPC_INCLUDE_DIRS "include/")
set(PAGE_HEAD_DIR "pages/container/")
set(DATA_DIR "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_DATADIR}")
set(TRANSLATIONS_DIR "${DATA_DIR}/ks-scmc/translations/")
set(TS_FILES "../translations/ks-scmc.zh_CN.ts" )

file(GLOB_RECURSE cur_src "*.cpp")
file(GLOB_RECURSE HEADERS "*.h")
file(GLOB_RECURSE FORMS "*.ui")
file(GLOB_RECURSE RESOURCES "../data/*.qrc")
file(GLOB_RECURSE RESOURCES "../data/*.qrc")

add_definitions(-std=gnu++11)

qt5_create_translation( QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES} )

add_executable(ks-scmc-client
	${cur_src} 
	${RESOURCES} 
	${HEADERS} 
        ${FORMS}
        ${QM_FILES})


target_include_directories(ks-scmc-client PRIVATE
        ${RPC_INCLUDE_DIRS}
        ${CMAKE_BINARY_DIR}
        ${CMAKE_CURRENT_BINARY_DIR}
        ${KLOG_QT5_INCLUDE_DIRS}
        ${KIRAN_WIDGETS_QT5_INCLUDE_DIRS}
        ${PAGE_HEAD_DIR})

target_link_libraries(ks-scmc-client
        proto_lib
        grpc++
        grpc++_reflection
        gpr
        protobuf
        cryptopp
        Qt5::Widgets
        ${KIRAN_WIDGETS_QT5_LIBRARIES}
        ${KLOG_QT5_LIBRARIES})

install(FILES ${QM_FILES}
    DESTINATION ${TRANSLATIONS_DIR})

install(TARGETS ks-scmc-client
    DESTINATION ${BIN_DIR})
