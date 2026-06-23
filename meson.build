project('starlight', 'c', version: '0.1.0', default_options: ['c_std=gnu11', 'warning_level=2'])

window_width  = get_option('window_width')
window_height = get_option('window_height')
window_title  = get_option('window_title')
assets_dir  = meson.project_source_root() / get_option('assets_dir')

global_defines = [
  '-DWINDOW_WIDTH=' + window_width.to_string(),
  '-DWINDOW_HEIGHT=' + window_height.to_string(),
  '-DWINDOW_TITLE="' + window_title + '"',
  '-DASSETS_DIR="' +  assets_dir + '"',
]

if host_machine.system() == 'android'
  global_defines += '-D_PLATFORM_ANDROID'
elif host_machine.system() == 'emscripten'
  global_defines += '-D_PLATFORM_WEB'
else
  global_defines += '-D_PLATFORM_DESKTOP'
endif

add_project_arguments(global_defines, language : ['c'])

cc = meson.get_compiler('c')

subdir('third-party')
subdir('src')
