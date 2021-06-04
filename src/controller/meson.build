source_files = ['../misc/log.cpp', '../misc/log.h', 'main.cpp', 'controller.cpp', 'service.cpp', 'udp.cpp', 'stdafx.h']

if get_option('buildtype') == 'debug'
    cpp_args = ['/DWIN32', '/D_DEBUG', '/D_CONSOLE', '/D_CRT_SECURE_NO_DEPRECATE', '/D_WINDOWS', '/DUNICODE']
    libs = [cpp.find_library('ws2_32'), cpp.find_library('wsock32'), cpp.find_library('detours', dirs: lib_dirs)]
else
    cpp_args = ['/DWIN32', '/D_NDEBUG', '/D_CONSOLE', '/D_CRT_SECURE_NO_DEPRECATE', '/D_WINDOWS', '/DUNICODE']
    libs = [cpp.find_library('ws2_32'), cpp.find_library('wsock32'), cpp.find_library('detours', dirs: lib_dirs)]
endif

executable('NWNx4Controller', source_files,
    cpp_args: cpp_args, include_directories: include_directories(include_dirs), dependencies: libs)
