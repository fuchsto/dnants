
BUILD_DIR=./build.dev

export PYBIND11_BASE=${HOME}/opt/pybind11


mkdir -p $BUILD_DIR
rm -Rf $BUILD_DIR/*

(cd $BUILD_DIR && \
  cmake -DCMAKE_BUILD_TYPE=Debug \
        -DCMAKE_INSTALL_PREFIX=$BUILD_DIR/install/ \
        \
        -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
  .. && \
  make VERBOSE=1 install) \
  && (cp $BUILD_DIR/compile_commands.json .)

