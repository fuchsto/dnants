

rm -Rf ./build
mkdir -p build

(cd build && cmake .. && make VERBOSE=1)

