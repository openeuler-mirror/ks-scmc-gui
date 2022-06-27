麒麟信安容器系统V1(安全版)(KylinSec Container system security version)，提供精简、高效、安全的容器及管理.

yum install cmake gettext gcc-c++ qt5-qtbase-devel qt5-linguist qt5-qtcharts-devel kiranwidgets-qt5-devel protobuf-devel zlog-devel kiran-log-qt5-devel grpc grpc-devel grpc-plugins xsd qrencode-devel
cryptopp-devel c-ares-devel

编译：
mkdir build && cd build
cmake -DCMAKE_INSTALL_PREFIX=/usr ..
make clean;make

sudo make install
