cd ./rpc && make && cd -
mkdir -p build && cd build
cmake -DCMAKE_INSTALL_PREFIX=/usr ..
make
