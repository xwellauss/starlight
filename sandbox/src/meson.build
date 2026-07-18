sandbox_inc = [include_directories('.'), public_inc]

sandbox_src = files(
  'main.c',
  'scenes/scene_default.c',
  'scenes/scene_block.c',
  'scenes/scene_model.c',
  'scenes/scene_play.c',
  'scenes/scene_server_test.c',
  'scenes/scene_client_test.c',
  'scenes/physics_sim/scene_gravity.c',
  'scenes/scene_menu.c',
)

sandbox_c_args = []
sandbox_link_args = []

sandbox_deps = [starlight_dep]

if host_machine.system() == 'android'
  android_c_args = ['-DASSETS_DIR=""']
  #android_link_args = ['-llog', '-landroid', '-u ANativeActivity_onCreate']
  android_link_args = []
  android_deps = [android_glue_dep]

  sandbox_lib = shared_library('starlight_sandbox', sandbox_src, include_directories: sandbox_inc, c_args: sandbox_c_args + android_c_args, link_args: sandbox_link_args + android_link_args, dependencies: sandbox_deps + android_deps)

elif host_machine.system() == 'emscripten'
  web_c_args = [
    '-pthread',
    '-sPROXY_POSIX_SOCKETS',
    '-DASSETS_DIR="assets/"',
  ]
  web_link_args = [
    '-sTOTAL_MEMORY=262144000',
    '-sALLOW_MEMORY_GROWTH=1',
    '-sPROXY_POSIX_SOCKETS',
    '-sPROXY_TO_PTHREAD',
    '-lwebsocket.js',
    '--embed-file=' + assets_dir +  '@/assets/',
    '--shell-file=' + assets_dir + '/index.html',
  ]

  sandbox_exe = executable('starlight_sandbox', sandbox_src, include_directories: sandbox_inc, c_args: sandbox_c_args + web_c_args, link_args: sandbox_link_args + web_link_args, dependencies: sandbox_deps, name_suffix: 'html')

else
  desktop_c_args = ['-DASSETS_DIR="assets/"']
  desktop_link_args = []

  sandbox_exe = executable('starlight_sandbox', sandbox_src, include_directories: sandbox_inc, c_args: sandbox_c_args + desktop_c_args, link_args: sandbox_link_args + desktop_link_args, dependencies: sandbox_deps)
endif
