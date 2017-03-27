
BUILD_DIR=./build

mkdir -p $BUILD_DIR
rm -Rf $BUILD_DIR/*

(cd $BUILD_DIR && \
  cmake -DCMAKE_BUILD_TYPE=Debug \
        -DINSTALL_PREFIX=$HOME/opt/gos/ \
        \
        -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
  .. && \
  make VERBOSE=1) \
  && (cp $BUILD_DIR/compile_commands.json .)

