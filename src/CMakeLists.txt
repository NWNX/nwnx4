project(NWNx4)

message($ENV{VCPKG_DIR})

set(VCPKG_TARGET_TRIPLET x86-windows-static-md)

include("C:\\opt\\vcpkg\\scripts\\buildsystems\\vcpkg.cmake")

set(VCPKG_DIR $ENV{VCPKG_DIR}/installed/x86-windows-static-md)

set(VCPKG_INCLUDE_DIR ${VCPKG_DIR}/include/)
set(VCPKG_LIB_DIR_DEBUG ${VCPKG_DIR}/debug/lib/)
set(VCPKG_LIB_DIR_RELEASE ${VCPKG_DIR}/lib/)

add_subdirectory("hook")
add_subdirectory("controller")
add_subdirectory("gui")
