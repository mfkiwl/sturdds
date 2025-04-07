cmake_minimum_required(VERSION 3.18...3.27)
project(sturdds)

string(ASCII 27 Esc)
set(Reset       "${Esc}[m")
set(Bold        "${Esc}[1m")
set(Red         "${Esc}[31m")
set(Green       "${Esc}[32m")
set(Yellow      "${Esc}[33m")
set(Blue        "${Esc}[34m")
set(Magenta     "${Esc}[35m")
set(Cyan        "${Esc}[36m")
set(White       "${Esc}[37m")
set(BoldRed     "${Esc}[1;31m")
set(BoldGreen   "${Esc}[1;32m")
set(BoldYellow  "${Esc}[1;33m")
set(BoldBlue    "${Esc}[1;34m")
set(BoldMagenta "${Esc}[1;35m")
set(BoldCyan    "${Esc}[1;36m")
set(BoldWhite   "${Esc}[1;37m")

# --- Version Control ---
set(STURDDS_MAJOR_VERSION 1)
set(STURDDS_MINOR_VERSION 0)
set(STURDDS_PATCH_VERSION 0)
set(PROJECT_VERSION ${STURDDS_MAJOR_VERSION}.${STURDDS_MINOR_VERSION}.${STURDDS_PATCH_VERSION})
message(STATUS "${BoldCyan}${PROJECT_NAME} version ${PROJECT_VERSION}${Reset}")

# # --- Compiler Flags ---
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# set(CMAKE_CXX_FLAGS "-Wall -Wextra -Wunused -pedantic -W")
set(CMAKE_CXX_FLAGS_DEBUG "-g -W")
# set(CMAKE_CXX_FLAGS_RELEASE "-O3 -Ofast -ffast-math -fomit-frame-pointer -fstrict-aliasing -flto -DNDEBUG -march=native")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG -march=native")

# --- Packages ---
find_package(fastdds REQUIRED)
find_package(fastcdr REQUIRED)

# --- Add Library Soruce Files ---
file(GLOB RTPS_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/rtps/sturdds/*.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/rtps/sturdds/*.cxx
    ${CMAKE_CURRENT_SOURCE_DIR}/rtps/sturdds/*.ipp
)
set(STURDDS_SRCS
    include/sturdds/publisher.hpp
    include/sturdds/subscriber.hpp
    include/sturdds/interface.hpp
    src/interface.cpp
    ${RTPS_SRCS}
)

# --- Create the C++ Library ---
add_library(${PROJECT_NAME} ${STURDDS_SRCS})
target_include_directories(
    ${PROJECT_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/rtps>
    $<INSTALL_INTERFACE:include>
)
target_link_libraries(
    ${PROJECT_NAME} PUBLIC
    fastdds
    fastcdr
)
set_target_properties(${PROJECT_NAME} PROPERTIES POSITION_INDEPENDENT_CODE ON)

# --- Build Executables ---
if (NOT DEFINED INSTALL_STURDDS_TESTS OR NOT INSTALL_STURDDS_TESTS)
    # message(STATUS "${BoldCyan}INSTALL_STURDDS_TESTS is false${Reset}")
else()
    # message(STATUS "${BoldCyan}INSTALL_STURDDS_TESTS is true${Reset}")
    file(GLOB testfiles "${CMAKE_CURRENT_SOURCE_DIR}/tests/*.cpp")
    foreach(file ${testfiles})
        # message(${file})
        get_filename_component(testname ${file} NAME_WE)
        add_executable(${testname} ${file})
        target_link_libraries( ${testname} PUBLIC ${PROJECT_NAME})
    endforeach(file ${testfiles})
endif()

# --- Make Library 'Findable' for other CMake Packages ---
include(CMakePackageConfigHelpers)

install(
    DIRECTORY include/ 
    DESTINATION include
)

install(
    TARGETS ${PROJECT_NAME}
    EXPORT ${PROJECT_NAME}Targets
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    INCLUDES DESTINATION include
)

export(
    TARGETS ${PROJECT_NAME} 
    FILE ${PROJECT_NAME}Targets.cmake
)

install(
    EXPORT ${PROJECT_NAME}Targets 
    FILE ${PROJECT_NAME}Targets.cmake 
    DESTINATION lib/cmake/${PROJECT_NAME}
)

configure_package_config_file(
    "${PROJECT_NAME}-config.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config.cmake"
    INSTALL_DESTINATION lib/cmake/${PROJECT_NAME}
    PATH_VARS PROJECT_NAME
)

install(
    FILES "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config.cmake"
    DESTINATION lib/cmake/${PROJECT_NAME}
)

# --- Python ---
if (NOT DEFINED INSTALL_PYTHON OR INSTALL_PYTHON)
    # find_package(Python REQUIRED COMPONENTS Interpreter Development.Module)
    # find_package(pybind11 CONFIG REQUIRED)
    # python_add_library(_sturdds_core MODULE src/sturdds-python.cpp WITH_SOABI)
    # target_link_libraries(_sturdds_core PRIVATE pybind11::headers ${PROJECT_NAME})
    # target_compile_definitions(_sturdds_core PRIVATE VERSION_INFO=${PROJECT_VERSION})
    # install(TARGETS _sturdds_core DESTINATION sturdds)

    # find_package(Python 3.8 COMPONENTS Interpreter Development.Module REQUIRED)
    # if (NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    #     set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
    #     set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
    # endif()
    # execute_process(
    #     COMMAND "${Python_EXECUTABLE}" -m nanobind --cmake_dir
    #     OUTPUT_STRIP_TRAILING_WHITESPACE OUTPUT_VARIABLE nanobind_ROOT
    # )
    # find_package(nanobind CONFIG REQUIRED)
    # nanobind_add_module(_sturdds_core NOMINSIZE src/sturdds-python.cpp)
    # target_link_libraries(_sturdds_core PRIVATE ${PROJECT_NAME})
    # target_compile_definitions(_sturdds_core PRIVATE VERSION_INFO=${PROJECT_VERSION})
    # install(TARGETS _sturdds_core DESTINATION sturdds)
    
    add_subdirectory(rtps/sturdds)
endif()
