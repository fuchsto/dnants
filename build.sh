
BUILD_DIR=./build

export PYBIND11_BASE=${HOME}/opt/pybind11


mkdir -p $BUILD_DIR
rm -Rf $BUILD_DIR/*

(cd $BUILD_DIR && \
  cmake -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_INSTALL_PREFIX=$BUILD_DIR/install/ \
        \
        -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
  .. && \
  make install)

