build_type='Release'
c_compiler='clang-18'
cpp_compiler='clang++-18'

clear
rm -rf build 
mkdir build
cd build
cmake .. \
        -DCMAKE_C_COMPILER=$c_compiler \
        -DCMAKE_CXX_COMPILER=$cpp_compiler \
        -DCMAKE_INSTALL_PREFIX=../build \
        -DCMAKE_BUILD_TYPE=$build_type \
        -DCMAKE_EXPORT_COMPILE_COMMANDS=1
cmake --build . -- -j4
# make
# make install
cd ..