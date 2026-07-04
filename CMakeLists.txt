project(ks-scmc-gui)

cmake_minimum_required(VERSION 2.8)

include(GNUInstallDirs)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


set(_REFLECTION grpc++_reflection)
set(_PROTOBUF_LIBPROTOBUF protobuf)
set(_GRPC_GRPCPP grpc++)

find_program(_PROTOBUF_PROTOC protoc)

find_package(Protobuf REQUIRED)
find_package(PkgConfig REQUIRED)
find_package(Qt5 COMPONENTS Widgets REQUIRED LinguistTools)
find_package(Qt5Charts)
#pkg_search_module(GRPC REQUIRED grpc)
#find_package(gRPC CONFIG REQUIRED)
pkg_search_module(KIRAN_WIDGETS_QT5 REQUIRED kiranwidgets-qt5)
pkg_search_module(KLOG_QT5 REQUIRED klog-qt5)
pkg_search_module(CRYPTOPP REQUIRED cryptopp)
pkg_search_module(KYLIN_LICENSE REQUIRED kylin-license)
pkg_search_module(KYLIN_LICENSE REQUIRED license-wrapper)


set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
set(DATA_DIR "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_DATADIR}")
set(BIN_DIR "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_BINDIR}")
set(INSTALL_DIR "/usr/libexec")
set(ICON_DIR "${DATA_DIR}/${PROJECT_NAME}/icons")
set(TRANSLATIONS_DIR "${DATA_DIR}/${PROJECT_NAME}/translations/")

#添加config.h.in文件
configure_file (
    "${PROJECT_SOURCE_DIR}/config/config.h.in"
    "${PROJECT_BINARY_DIR}/config/config.h"
)

add_subdirectory(proto)
add_subdirectory(src)
add_subdirectory(config)
add_subdirectory(data/images)
