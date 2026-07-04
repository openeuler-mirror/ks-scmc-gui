#cmake_minimum_required(VERSION 3.10)
cmake_minimum_required(VERSION 2.8)

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

find_package(Qt5 COMPONENTS Widgets DBus Gui Core)

set(include_dir_list 
	${INCLUDE_DIRS} 
	${CMAKE_BINARY_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	${KLOG_QT5_INCLUDE_DIRS}
	${KIRAN_WIDGETS_QT5_INCLUDE_DIRS}
	${GENERAL_PROTO_HDRS}
	${KYLIN_LICENSE_INCLUDE_DIRS}
	${LIBNOTIFY_INCLUDE_DIRS})

set(link_lib_list 
	proto_lib
	grpc++
	grpc++_reflection
	gpr
	protobuf
	qrencode
	cryptopp
	Qt5::Widgets
	Qt5::Charts
	Qt5::DBus
	${KIRAN_WIDGETS_QT5_LIBRARIES}
	${KLOG_QT5_LIBRARIES}
	${GRPCXX_LIBRARIES}
	${PROTOBUF_LIBRARIES}
	${LIBNOTIFY_LIBRARIES})

if (KIRAN_STYLE_FOUND)
	message(STATUS "append kiran style dependens...")
	list(APPEND include_dir_list ${KIRAN_STYLE_INCLUDE_DIRS})
	list(APPEND link_lib_list ${KIRAN_STYLE_LIBRARIES})
endif()

add_executable(${PROJECT_NAME}
	${cur_src}
	${RESOURCES}
	${HEADERS}
	${FORMS}
	${QM_FILES}
	${SCMC_GENERATED_CPP_FILES}
	${LICENSE_MANAGER_GENERATED_CPP_FILES}
	${LICENSE_OBJECT_GENERATED_CPP_FILES})

target_include_directories(${PROJECT_NAME} PRIVATE ${include_dir_list})

target_link_libraries(${PROJECT_NAME} ${link_lib_list})

install(FILES ${QM_FILES}
    DESTINATION ${TRANSLATIONS_DIR})

if (GENERATE_TO_RUN)
	install(TARGETS ${PROJECT_NAME}
		DESTINATION ${INSTALL_DIR})
else ()
	install(TARGETS ${PROJECT_NAME}
                DESTINATION ${CMAKE_INSTALL_FULL_BINDIR})
endif ()

