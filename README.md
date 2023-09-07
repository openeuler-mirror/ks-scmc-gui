麒麟信安安全容器魔方(KylinSec security Container magic Cube)，提供精简、高效、安全的容器及管理.

yum install cmake gettext gcc-c++ qt5-qtbase-devel qt5-linguist qt5-qtcharts-devel kiranwidgets-qt5-devel protobuf-devel zlog-devel kiran-log-gtk3-devel grpc grpc-devel grpc-plugins xsd 
cryptopp-devel

编译：
mkdir build && cd build
cmake -DCMAKE_INSTALL_PREFIX=/usr ..
make clean;make

sudo make install
