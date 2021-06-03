cmake_minimum_required(VERSION 3.10)

project(main VERSION 1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/archive)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(VCPKG_TARGET_TRIPLET x86-windows-static-md)

include(vcpkg/scripts/buildsystems/vcpkg.cmake)

set(VCPKG_DIR $ENV{VCPKG_DIR}/installed/${VCPKG_TARGET_TRIPLET})

set(VCPKG_INCLUDE_DIR ${VCPKG_DIR}/include/)
set(VCPKG_LIB_DIR_DEBUG ${VCPKG_DIR}/debug/lib/)
set(VCPKG_LIB_DIR_RELEASE ${VCPKG_DIR}/lib/)

add_subdirectory("src")