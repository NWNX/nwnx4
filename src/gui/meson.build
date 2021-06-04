source_files = ['stdwx.cpp', 'mainframe.cpp', '../controller/controller.cpp', '../controller/udp.cpp', 'dlgabout.cpp', 'nwnx.cpp', 'worker.cpp', 'main.cpp', '../misc/log.cpp', 'Log.cpp']

if get_option('buildtype') == 'debug'
    cpp_args = ['/DWIN32', '/D_DEBUG', '/D__WXMSWUD__', '/D__WXDEBUG__', '/D_WINDOWS', '/DNOPCH', '/DwxUSE_GUI=1', '/DUNICODE', '/DwxUSE_UNICODE']
    libs = [cpp.find_library('comctl32'), cpp.find_library('rpcrt4'), cpp.find_library('ws2_32'), cpp.find_library('wsock32'),
        cpp.find_library('detours', dirs: lib_dirs), cpp.find_library('wxmsw31ud_core', dirs: lib_dirs), cpp.find_library('wxbase31ud_net', dirs: lib_dirs),
        cpp.find_library('wxbase31ud', dirs: lib_dirs), cpp.find_library('wxregexud', dirs: lib_dirs), cpp.find_library('jpegd', dirs: lib_dirs),
        cpp.find_library('libpng16d', dirs: lib_dirs), cpp.find_library('zlibd', dirs: lib_dirs)]
else
    cpp_args = ['/DWIN32', '/DNDEBUG', '/D__WXMSWU__', '/D_WINDOWS', '/DNOPCH', '/DwxUSE_GUI=1', '/DUNICODE', '/DwxUSE_UNICODE']
    libs = [cpp.find_library('comctl32'), cpp.find_library('rpcrt4'), cpp.find_library('ws2_32'), cpp.find_library('wsock32'),
            cpp.find_library('detours', dirs: lib_dirs), cpp.find_library('wxmsw31u_core', dirs: lib_dirs), cpp.find_library('wxbase31u_net', dirs: lib_dirs),
            cpp.find_library('wxbase31u', dirs: lib_dirs), cpp.find_library('wxregexu', dirs: lib_dirs), cpp.find_library('jpeg', dirs: lib_dirs),
            cpp.find_library('libpng16', dirs: lib_dirs), cpp.find_library('zlib', dirs: lib_dirs)]
endif

executable('NWNx4Gui', source_files,
    cpp_args: cpp_args, include_directories: include_directories(['../controller', '../misc', include_dirs]), dependencies: libs)
