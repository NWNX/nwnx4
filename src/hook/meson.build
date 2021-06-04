source_files = ['hook.cpp', 'crashdump.cpp', '../plugins/legacy_plugin.cpp', '../plugins/plugin.cpp', '../misc/log.cpp', '../plugins/plugin.cpp']

if get_option('buildtype') == 'debug'
    cpp_args = ['/DWIN32', '/D_DEBUG', '/D_WINDOWS', '/D_USRDLL', '/DTEST_EXPORTS', '/D_UNICODE']
    libs = [cpp.find_library('comctl32'), cpp.find_library('rpcrt4'), cpp.find_library('ws2_32'), cpp.find_library('wsock32'), cpp.find_library('detours', dirs: lib_dirs)]
else
    cpp_args = ['/DWIN32', '/DNDEBUG', '/D_CONSOLE', '/D_WINDOWS', '/D_USRDLL', '/DTEST_EXPORTS', '/D_UNICODE']
    libs = [cpp.find_library('comctl32'), cpp.find_library('rpcrt4'), cpp.find_library('ws2_32'), cpp.find_library('wsock32'), cpp.find_library('detours', dirs: lib_dirs)]
endif

NWNx4Hook_lib = shared_library('NWNx4Hook', source_files,
    cpp_args: cpp_args, include_directories: include_directories(['../controller', '../misc', include_dirs]), dependencies: libs)
