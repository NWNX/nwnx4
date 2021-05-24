project(NWNx4)

message($ENV{VCPKG_DIR})

set(VCPKG_INCLUDE_DIR $ENV{VCPKG_DIR}/installed/x86-windows-static-md/include/)
set(VCPKG_LIB_DIR_DEBUG $ENV{VCPKG_DIR}/installed/x86-windows-static-md/debug/lib/)
set(VCPKG_LIB_DIR_RELEASE $ENV{VCPKG_DIR}/installed/x86-windows-static-md/lib/)

add_subdirectory("hook")
add_subdirectory("controller")
add_subdirectory("gui")
