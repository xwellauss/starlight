starlight_includes = include_directories('.')
starlight_src = files(
  'main.c',
  'utils/utils.c',
  'utils/json_helper.c',
  'network/network.c',
  'network/net_packer.c',
  'audio/audio.c',
  'core/model_loader/gltf.c',
  'core/model_loader/model_loader.c',
  'core/renderer/shader.c',
  'core/renderer/texture.c',
  'core/renderer/vertex_buffer.c',
  'core/ui/ui.c',
  'core/ui/ui_backend.c',
  'core/ui/ui_font.c',
  'core/camera.c',
  'core/ecs.c',
  'core/game_engine.c',
  'core/scene.c',
  'core/texture_atlas.c',
  'core/window.c',
  'scenes/physics_sim/scene_gravity.c',
  'scenes/scene_block.c',
  'scenes/scene_default.c',
  'scenes/scene_menu.c',
  'scenes/scene_model.c',
  #  'scenes/scene_play.c',
  #  'scenes/scene_play_client.c',
  #  'scenes/scene_play_server.c',
)

is_debug = get_option('buildtype') == 'debug'
wants_asan = is_debug and host_machine.system() != 'android' and host_machine.system() != 'emscripten'

starlight_c_args = ['-Wno-missing-braces', '-Wno-unused-parameter', '-Wno-unused-variable']
starlight_link_args = []

if wants_asan
  starlight_c_args += '-fsanitize=address'
  starlight_link_args += '-fsanitize=address'
endif

starlight_deps = [
  glfw_dep, cglm_dep, glad_dep, clay_dep, enet_dep, miniaudio_dep, cjson_dep, cgltf_dep, stb_dep, cc.find_library('m')
]


if host_machine.system() == 'android'
elif host_machine.system() == 'emscripten'
else
  starlight_exe = executable('starlight', starlight_src, include_directories: [starlight_includes], c_args: starlight_c_args, link_args: starlight_link_args, dependencies: starlight_deps)
endif
