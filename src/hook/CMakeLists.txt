project(NWNx4Hook VERSION 1.0)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /DWIN32 /D_DEBUG /D_WINDOWS /D_USRDLL /DTEST_EXPORTS /D_UNICODE")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /DWIN32 /DNDEBUG /D_CONSOLE /D_WINDOWS /D_USRDLL /DTEST_EXPORTS /D_UNICODE")
set(SOURCE_FILES hook.cpp crashdump.cpp ../plugins/legacy_plugin.cpp ../plugins/plugin.cpp ../misc/log.cpp ../plugins/plugin.cpp hook.map)

add_library(NWNx4Hook SHARED ${SOURCE_FILES})

set_source_files_properties(hook.map PROPERTIES HEADER_FILE_ONLY TRUE)

if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    target_include_directories(NWNx4Hook PRIVATE ../controller ../misc ../../lib/NWN2/include ${VCPKG_INCLUDE_DIR} ${VCPKG_LIB_DIR_DEBUG}/mswud)
    target_link_directories(NWNx4Hook PRIVATE ../lib/NWN2 ${VCPKG_LIB_DIR_DEBUG})
    target_link_libraries(NWNx4Hook PRIVATE comctl32.lib rpcrt4.lib ws2_32.lib wsock32.lib detours.lib)
elseif(${CMAKE_BUILD_TYPE} STREQUAL "Release")
    target_include_directories(NWNx4Hook PRIVATE ../controller ../misc ../../lib/NWN2/include ${VCPKG_INCLUDE_DIR} ${VCPKG_LIB_DIR_RELEASE}/mswu)
    target_link_directories(NWNx4Hook PRIVATE ../lib/NWN2 ${VCPKG_LIB_DIR_RELEASE})
    target_link_libraries(NWNx4Hook PRIVATE comctl32.lib rpcrt4.lib ws2_32.lib wsock32.lib detours.lib)
endif()
