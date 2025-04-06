build_type='Release'
c_compiler='clang-18'
cpp_compiler='clang++-18'
build_tests='True'
build_python='True'

clear
rm -rf build 
mkdir build
cd build
cmake .. \
    -DCMAKE_C_COMPILER=$c_compiler \
    -DCMAKE_CXX_COMPILER=$cpp_compiler \
    -DINSTALL_STURDDS_TESTS=$build_tests \
    -DINSTALL_PYTHON=$build_python \
    -DCMAKE_INSTALL_PREFIX=../build \
    -DCMAKE_BUILD_TYPE=$build_type \
    -DCMAKE_EXPORT_COMPILE_COMMANDS=1
cmake --build . -- -j4
# make
# make install
cd ..