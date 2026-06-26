project('starlight', 'c', version: '0.1.0', default_options: ['c_std=gnu11', 'warning_level=2'])

global_defines = []

if host_machine.system() == 'android'
  global_defines += '-D_PLATFORM_ANDROID'
elif host_machine.system() == 'emscripten'
  global_defines += '-D_PLATFORM_WEB'
else
  global_defines += '-D_PLATFORM_DESKTOP'
endif

add_project_arguments(global_defines, language : ['c'])

cc = meson.get_compiler('c')

public_inc = include_directories('include')

subdir('third-party')
subdir('src')

build_sandbox = get_option('build_sandbox')

if build_sandbox
  subdir('sandbox')
endif
