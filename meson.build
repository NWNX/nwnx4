project('NWNx4', 'cpp', default_options : ['default_library=static', 'cpp_std=c++17'])
add_project_arguments('/permissive', language: 'cpp')
cpp = meson.get_compiler('cpp')

# TODO: Fix the approach to vcpkg (???).
vcpkg_target_triplet = 'x86-windows-static-md'
vcpkg_dir = 'vcpkg_installed/' + vcpkg_target_triplet
vcpkg_dir_rel = '../../' + vcpkg_dir
vcpkg_dir_abs = meson.current_source_dir() + '/' + vcpkg_dir
vcpkg_include_dir_rel = vcpkg_dir_rel + '/include/'
vcpkg_lib_dir_debug_rel = vcpkg_dir_rel + '/debug/lib/'
vcpkg_lib_dir_release_rel = vcpkg_dir_rel + '/lib/'
# vcpkg_include_dir_abs = vcpkg_dir_abs + '/include/'
vcpkg_lib_dir_debug_abs = vcpkg_dir_abs + '/debug/lib/'
vcpkg_lib_dir_release_abs = vcpkg_dir_abs + '/lib/'

include_dirs = ['.', '..', '../../lib/NWN2/include', vcpkg_include_dir_rel]
lib_dirs = [meson.current_source_dir() + '/lib/NWN2']

if get_option('buildtype') == 'debug'
    include_dirs += vcpkg_lib_dir_debug_rel + '/mswud'
    lib_dirs += vcpkg_lib_dir_debug_abs
else
    include_dirs += vcpkg_lib_dir_release_rel + '/mswu'
    lib_dirs += vcpkg_lib_dir_release_abs
endif

subdir('src')
