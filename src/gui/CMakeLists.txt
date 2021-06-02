project(NWNx4Gui VERSION 1.0)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /DWIN32 /D_DEBUG /D__WXMSWUD__ /D__WXDEBUG__ /D_WINDOWS /DNOPCH /DwxUSE_GUI=1 /DUNICODE /DwxUSE_UNICODE")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /DWIN32 /DNDEBUG /D__WXMSWU__ /D_WINDOWS /DNOPCH /DwxUSE_GUI=1 /DUNICODE /DwxUSE_UNICODE")
set(SOURCE_FILES WIN32 mainframe.cpp ../controller/controller.cpp ../controller/udp.cpp dlgabout.cpp nwnx.cpp worker.cpp main.cpp main.h ../misc/log.cpp log.cpp log.h Log.cpp Log.h)

add_executable(NWNx4Gui  ${SOURCE_FILES})
install(TARGETS NWNx4Hook
        RUNTIME DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
        LIBRARY DESTINATION ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
target_sources(NWNx4Gui PRIVATE "NWNX GUI.rc" nwnx.rc)
target_precompile_headers(NWNx4Gui PUBLIC stdwx.h)

if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    target_include_directories(NWNx4Gui PRIVATE ../controller ../misc ../../lib/NWN2/include ${VCPKG_INCLUDE_DIR} ${VCPKG_LIB_DIR_DEBUG}/mswud)
    target_link_directories(NWNx4Gui PRIVATE ../lib/NWN2 ${VCPKG_LIB_DIR_DEBUG})
    target_link_libraries(NWNx4Gui PRIVATE comctl32.lib rpcrt4.lib ws2_32.lib wsock32.lib detours.lib wxmsw31ud_core.lib wxbase31ud_net.lib wxbase31ud.lib wxregexud.lib jpegd.lib libpng16d.lib zlibd.lib)
elseif(${CMAKE_BUILD_TYPE} STREQUAL "Release")
    target_include_directories(NWNx4Gui PRIVATE ../controller ../misc ../../lib/NWN2/include ${VCPKG_INCLUDE_DIR} ${VCPKG_LIB_DIR_RELEASE}/mswu)
    target_link_directories(NWNx4Gui PRIVATE ../lib/NWN2 ${VCPKG_LIB_DIR_RELEASE})
    target_link_libraries(NWNx4Gui PRIVATE comctl32.lib rpcrt4.lib ws2_32.lib wsock32.lib detours.lib wxmsw31u_core.lib wxbase31u_net.lib wxbase31u.lib wxregexu.lib jpegd.lib libpng16.lib zlib.lib)
endif()
