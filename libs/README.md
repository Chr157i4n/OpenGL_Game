# Third-party libraries

This folder holds third-party dependencies.  The CMake build downloads most of
them automatically into `libs/_deps/` (ignored by git):

| Library | Purpose | Source |
| --- | --- | --- |
| SDL2 | Window, input and OpenGL context | https://github.com/libsdl-org/SDL |
| GLEW | OpenGL function loading | https://github.com/Perlmint/glew-cmake |
| GLM | Vector and matrix math | https://github.com/g-truc/glm |
| stb | Image and font loading | https://github.com/nothings/stb |
| TinyXML2 | Level file parsing | https://github.com/leethomason/tinyxml2 |
| ENet | Networking | https://github.com/lsalzman/enet |
| Lua | Scripting | https://github.com/lua/lua |
| SoLoud | 2D/3D audio (SDL2 backend) | https://github.com/jarikomppa/soloud |

## System packages

Set `-DOG_USE_SYSTEM_DEPS=ON` to prefer packages installed by vcpkg, the
distribution package manager, or a toolchain file.  Missing packages are still
downloaded unless `-DOG_FETCH_DEPS=OFF` is set.

## Audio

Audio uses SoLoud with the SDL2 backend.  SoLoud is downloaded automatically
into `libs/_deps/` and needs no manual installation.  Set
`-DOG_ENABLE_AUDIO=OFF` for a silent client build.

## Clean rebuild

The downloaded sources live in `libs/_deps/`.  Deleting this folder (or the
build folder) forces CMake to download them again on the next configure.
