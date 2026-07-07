# starlight

A graphics framework written in C. Supports Desktop, Web and Android.


## Building the Project
> **Windows:** MSYS2 should work, just make sure all dependencies are installed.

Clone this repository recursively to pull in the submodules

**To build the project, you will need:**

* A C Compiler: Clang, GCC, Emscripten, Android NDK
* Meson + Ninja

You can download them from your package manager

After cloning the repo recursively, `cd` into it.

### Desktop (Linux)
In `meson_options.txt`, set `linux_windowing` to `x11` or `wayland` as needed.
```
CC=clang CXX=clang++ meson setup bin/desktop-debug --buildtype="debug"
meson compile -C bin/desktop-debug
```

Run:
```
./bin/desktop-debug/src/starlight
```


### Web (Emscripten)
Source emsdk first, then:
```
meson setup bin/web-debug --cross-file cross/emscripten.ini --buildtype="debug"
meson compile -C bin/web-debug
```

Serve locally:
```
cd scripts && python3 localhost-server.py
```

The local server requires HTTPS and specific cross-origin isolation headers for
Emscripten's `SharedArrayBuffer` support. Generate a local certificate once using
[mkcert](https://github.com/FiloSottile/mkcert). Install it for your distro.
```
cd scripts
mkcert -install && mkcert 0.0.0.0
```



### Android
Requires Android SDK, NDK, and Gradle. Set `ndk_home` in `meson_options.txt` and in `cross/android-arm64.ini`.

Build the shared library.

```
meson setup bin/android-arm64-debug --cross-file cross/android-arm64.ini --buildtype=debug
meson compile -C bin/android-arm64-debug
```

Build the apk.
```
cd android-project && gradle build
```


The generated apk will be in `android-project/app/build/outputs/apk/debug/app-debug.apk`.

## Dependencies
`OpenGL ES + glad` for graphics.

`GLFW` for window.

`cglm` for math.

`clay` for UI.

`miniaudio` for audio.

`enet` for networking.

`cgltf` for model loading.

`stb` libraries.

`cJSON` for json parsing.



## Credits and Attribution
Font used: [Monocraft](https://github.com/IdreesInc/Monocraft)

