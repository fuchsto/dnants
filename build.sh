

rm -Rf ./build
mkdir -p build

(cd build && cmake -DCMAKE_BUILD_TYPE=Debug .. && make VERBOSE=1)

