starlight_inc = [include_directories('.'), public_inc]

starlight_src = files(
  'audio/audio.c',  
  'core/camera.c',
  'core/ecs.c',
  'core/engine.c',
  'core/scene/scene_manager.c',
  'core/window/window.c',
  'core/window/input.c',
  'core/renderer/renderer.c',
  'core/renderer/shader.c',
  'core/renderer/texture.c',
  'core/renderer/vertex_buffer.c',
  'core/resources/texture_atlas.c',
  'core/resources/gltf.c',
  'core/resources/model_loader.c',
  'core/ui/ui.c',
  'core/ui/ui_backend.c',
  'core/ui/ui_font.c',
  'network/network.c',
  'network/net_packer.c',
  'platform/platform.c',
  'utils/json_helper.c',
  'utils/logger.c',
  'utils/math_utils.c',
)

#is_debug = get_option('buildtype') == 'debug'
#wants_asan = is_debug and host_machine.system() != 'android' and host_machine.system() != 'emscripten'

starlight_c_args = ['-Wno-missing-braces', '-Wno-unused-parameter', '-Wno-unused-variable']
starlight_link_args = []

#if wants_asan
#  starlight_c_args += '-fsanitize=address'
#  starlight_link_args += '-fsanitize=address'
#endif

starlight_deps = [
  glfw_dep, cglm_dep, glad_dep, clay_dep, enet_dep, miniaudio_dep, cjson_dep, cgltf_dep, stb_dep, cc.find_library('m')
]


if host_machine.system() == 'android'
  starlight_src += 'platform/platform_android.c'

  android_c_args = []
  #android_link_args = ['-llog', '-landroid', '-u ANativeActivity_onCreate']
  android_link_args = [] 
  android_deps = [android_glue_dep]

  starlight_lib = static_library('starlight', starlight_src, include_directories: starlight_inc, c_args: starlight_c_args + android_c_args, link_args: starlight_link_args + android_link_args, dependencies: starlight_deps + android_deps)
  starlight_dep = declare_dependency(link_with: starlight_lib, include_directories: public_inc, compile_args: starlight_c_args + android_c_args, link_args: starlight_link_args + android_link_args, dependencies: starlight_deps)

elif host_machine.system() == 'emscripten'
  starlight_src += 'platform/platform_web.c'

  web_c_args = ['-pthread']
  web_link_args = [
    '-pthread',
    '-sPROXY_POSIX_SOCKETS',
    '-sUSE_GLFW=3',
    '-sWASM=1',
    '-sMAX_WEBGL_VERSION=2',
    '-sMIN_WEBGL_VERSION=2',
    '-sASSERTIONS=1',
  ]

  starlight_lib = static_library('starlight', starlight_src, include_directories: starlight_inc, c_args: starlight_c_args + web_c_args, link_args: starlight_link_args + web_link_args, dependencies: starlight_deps)
  starlight_dep = declare_dependency(link_with: starlight_lib, include_directories: public_inc, compile_args: starlight_c_args + web_c_args, link_args: starlight_link_args + web_link_args, dependencies: starlight_deps)

else
  starlight_src += 'platform/platform_desktop.c'

  starlight_lib = static_library('starlight', starlight_src, include_directories: starlight_inc, c_args: starlight_c_args, link_args: starlight_link_args, dependencies: starlight_deps)
  starlight_dep = declare_dependency(link_with: starlight_lib, include_directories: public_inc, compile_args: starlight_c_args, link_args: starlight_link_args, dependencies: starlight_deps)
endif

