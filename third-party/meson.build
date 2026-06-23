# use find src_dir -name '*.c' | awk '{print "  \x27" $0 "\x27,"}' to get the files list
vendor_opts = ['warning_level=0']



# glfw
glfw_inc = include_directories('glfw/include')
if host_machine.system() != 'emscripten'
  glfw_src = files(
    'glfw/include/GLFW/glfw3.h',
    'glfw/include/GLFW/glfw3native.h',
    'glfw/src/internal.h',
    'glfw/src/platform.h',
    'glfw/src/mappings.h',
    'glfw/src/context.c',
    'glfw/src/init.c',
    'glfw/src/input.c',
    'glfw/src/monitor.c',
    'glfw/src/platform.c',
    'glfw/src/vulkan.c',
    'glfw/src/window.c',
    'glfw/src/egl_context.c',
    'glfw/src/osmesa_context.c',
    'glfw/src/null_platform.h',
    'glfw/src/null_joystick.h',
    'glfw/src/null_init.c',
    'glfw/src/null_monitor.c',
    'glfw/src/null_window.c',
    'glfw/src/null_joystick.c',
  )

  glfw_defines = []
  glfw_deps = []

  if host_machine.system() == 'linux' or host_machine.system() == 'android'
    glfw_src += files(
      'glfw/src/posix_time.h',
      'glfw/src/posix_thread.h',
      'glfw/src/posix_module.c',
      'glfw/src/posix_time.c',
      'glfw/src/posix_thread.c',
      'glfw/src/posix_poll.h',
      'glfw/src/posix_poll.c',
    )
  endif

  if host_machine.system() == 'linux'
    if get_option('linux_windowing') == 'wayland'
      glfw_src += files(
        'glfw/src/wl_platform.h',
        'glfw/src/xkb_unicode.h',
        'glfw/src/wl_init.c',
        'glfw/src/wl_monitor.c',
        'glfw/src/wl_window.c',
        'glfw/src/xkb_unicode.c',
        'glfw/src/linux_joystick.h',
        'glfw/src/linux_joystick.c',
      )
      glfw_defines += '-D_GLFW_WAYLAND'
    else
      glfw_src += files(
        'glfw/src/x11_platform.h',
        'glfw/src/xkb_unicode.h',
        'glfw/src/x11_init.c',
        'glfw/src/x11_monitor.c',
        'glfw/src/x11_window.c',
        'glfw/src/xkb_unicode.c',
        'glfw/src/glx_context.c',
        'glfw/src/linux_joystick.h',
        'glfw/src/linux_joystick.c',
      )
      glfw_defines += '-D_GLFW_X11'
    endif
  endif

  glfw_lib = static_library('glfw', glfw_src, include_directories: glfw_inc, c_args: glfw_defines, dependencies: glfw_deps, override_options: ['c_std=gnu99'] + vendor_opts)
  glfw_dep = declare_dependency(link_with: glfw_lib, include_directories: glfw_inc)
else
  glfw_dep = declare_dependency()
endif

# cglm
cglm_inc = include_directories('cglm/include')
cglm_src = files(
  'cglm/src/aabb2d.c',
  'cglm/src/affine.c',
  'cglm/src/affine2d.c',
  'cglm/src/bezier.c',
  'cglm/src/box.c',
  'cglm/src/cam.c',
  'cglm/src/clipspace/ortho_lh_no.c',
  'cglm/src/clipspace/ortho_lh_zo.c',
  'cglm/src/clipspace/ortho_rh_no.c',
  'cglm/src/clipspace/ortho_rh_zo.c',
  'cglm/src/clipspace/persp_lh_no.c',
  'cglm/src/clipspace/persp_lh_zo.c',
  'cglm/src/clipspace/persp_rh_no.c',
  'cglm/src/clipspace/persp_rh_zo.c',
  'cglm/src/clipspace/project_no.c',
  'cglm/src/clipspace/project_zo.c',
  'cglm/src/clipspace/view_lh_no.c',
  'cglm/src/clipspace/view_lh_zo.c',
  'cglm/src/clipspace/view_rh_no.c',
  'cglm/src/clipspace/view_rh_zo.c',
  'cglm/src/curve.c',
  'cglm/src/ease.c',
  'cglm/src/euler.c',
  'cglm/src/frustum.c',
  'cglm/src/io.c',
  'cglm/src/ivec2.c',
  'cglm/src/ivec3.c',
  'cglm/src/ivec4.c',
  'cglm/src/mat2.c',
  'cglm/src/mat2x3.c',
  'cglm/src/mat2x4.c',
  'cglm/src/mat3.c',
  'cglm/src/mat3x2.c',
  'cglm/src/mat3x4.c',
  'cglm/src/mat4.c',
  'cglm/src/mat4x2.c',
  'cglm/src/mat4x3.c',
  'cglm/src/noise.c',
  'cglm/src/plane.c',
  'cglm/src/project.c',
  'cglm/src/quat.c',
  'cglm/src/ray.c',
  'cglm/src/sphere.c',
  'cglm/src/swift/empty.c',
  'cglm/src/vec2.c',
  'cglm/src/vec3.c',
  'cglm/src/vec4.c',
)

cglm_lib = static_library('cglm', cglm_src, include_directories: cglm_inc, override_options: vendor_opts)
cglm_dep = declare_dependency(link_with: cglm_lib, include_directories: cglm_inc)

# glad
glad_inc = include_directories('glad/include')
glad_src = files(
  'glad/src/glad.c'
)
glad_lib = static_library('glad', glad_src, include_directories: glad_inc)
glad_dep = declare_dependency(link_with: glad_lib, include_directories: glad_inc)

# enet
enet_inc = include_directories('enet/include')
enet_src = files(
  'enet_impl.c'
)
enet_c_args = ['-DHAS_SOCKLEN_T']

enet_lib = static_library('enet', enet_src, include_directories: enet_inc, c_args: enet_c_args, override_options: vendor_opts + ['c_std=none'])
enet_dep = declare_dependency(link_with: enet_lib, include_directories: enet_inc)

# miniaudio
# TODO: Add android and web support
miniaudio_inc = include_directories('miniaudio')
miniaudio_src = files(
  'miniaudio/miniaudio.c'
)
miniaudio_deps = [cc.find_library('m'), cc.find_library('pthread')]

miniaudio_lib = static_library('miniaudio', miniaudio_src, include_directories: miniaudio_inc, override_options: vendor_opts)
miniaudio_dep = declare_dependency(link_with: miniaudio_lib, include_directories: miniaudio_inc)


# clay
clay_inc = include_directories('clay')
clay_src = files(
  'clay_impl.c',
)
clay_lib = static_library('clay', clay_src, include_directories: clay_inc, override_options: vendor_opts)
clay_dep = declare_dependency(link_with: clay_lib, include_directories: clay_inc)


# cgltf
cgltf_inc = include_directories('cgltf')
cgltf_src = files(
  'cgltf_impl.c',
)
cgltf_lib = static_library('cgltf', cgltf_src, include_directories: cgltf_inc, override_options: vendor_opts)
cgltf_dep = declare_dependency(link_with: cgltf_lib, include_directories: cgltf_inc)

# cJSON
cjson_inc = include_directories('cJSON')
cjson_src = files(
  'cJSON/cJSON.c',
)

cjson_lib = static_library('cJSON', cjson_src, include_directories: cjson_inc, override_options: vendor_opts)
cjson_dep = declare_dependency(link_with: cjson_lib, include_directories: cjson_inc)


# stb
stb_inc = include_directories('stb/include')
stb_src = files(
  'stb/src/stb_ds.c',
  'stb/src/stb_image.c',
  'stb/src/stb_perlin.c',
  'stb/src/stb_rect_pack.c',
  'stb/src/stb_truetype.c', 
)
stb_lib = static_library('stb', stb_src, include_directories: stb_inc, override_options: vendor_opts)
stb_dep = declare_dependency(link_with: stb_lib, include_directories: stb_inc)
