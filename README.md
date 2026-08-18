# Digital Logic Simulator

A desktop app for building and simulating digital logic circuits — place gates, wire them together, flip switches, and watch signals propagate in real time. Built with C++17 and SFML.

## What you'll need

- Windows
- CMake (3.16 or newer)
- A C++17 compiler — MinGW-W64 or MSVC both work
- Git (CMake uses this to download SFML automatically)
- An internet connection, just for the first build

You don't need to install SFML yourself. The project downloads and builds it automatically the first time you configure.

**One thing to watch for if you're using MinGW:** make sure your compiler is an **MSVCRT** build, not a UCRT one. This matters because SFML ships some prebuilt libraries (FreeType, libFLAC) that were compiled against the older MSVCRT runtime. If you use a UCRT-based MinGW instead, you'll hit linker errors like `undefined reference to '_setjmp'`. If that happens, grab an MSVCRT build from [winlibs.com](https://winlibs.com) instead — just look for "msvcrt" in the filename. If you're building with MSVC, none of this applies to you.

## How to build it

Open a terminal in the project folder (where `CMakeLists.txt` lives) and run:

```
cmake -S . -B build -G "MinGW Makefiles" -DCMAKE_CXX_COMPILER="<full path to your msvcrt g++.exe>" -DCMAKE_C_COMPILER="<full path to your msvcrt gcc.exe>"
cmake --build build
```
(make sure do as mention above if you have multiple compilers)

If you just have MSVCRT one then you can just use:
```
cmake -S . -B build
cmake --build build
```
The first command sets things up and, the very first time, also downloads and compiles SFML from source — this can take a few minutes and might look like nothing's happening while it clones. That's normal, just let it finish. (personally it took me around 4 minutes of waiting)

If CMake picks a generator that doesn't match your compiler (say, it wants Ninja but you've only got MinGW's make), tell it explicitly:

```
cmake -S . -B build -G "MinGW Makefiles"
```

## How to run it

```
.\build\bin\DigitalLogicSimulator.exe
```

The font and other resources get copied next to the executable automatically as part of the build, so there's nothing extra to set up.

## If something goes wrong (which probably will )

**"Could not find a package configuration file for SFML"** — this shouldn't happen, since the project fetches SFML on its own rather than looking for an existing install. If you see it, something in `CMakeLists.txt` may have been changed.

**Linker errors about `_setjmp` or `__imp___iob_func`** — this is the MinGW runtime mismatch mentioned above. Switch to an MSVCRT build of MinGW.

**The exe builds fine but won't launch, no error at all** — usually missing runtime DLLs. This project links its runtime statically so it shouldn't come up, but if you're seeing it, double check you're using the compiler this was built with.

**"generator does not match the generator used previously"** — delete the `build` folder and start the configure step fresh. Don't mix generators in the same build folder.

**"Unable to load font." and the app exits** — the font file didn't make it into `resources/fonts/`. Check that `Inter-Regular.ttf` is there before building.

## A note on the code

Most of the simulation logic — gates, wires, circuit evaluation — lives in header files with the implementation written inline, rather than split across `.h`/`.cpp` pairs. CMake picks up every source file under `src/` and `include/` automatically, so adding new files to the project doesn't require touching the build config.

## Project layout

```
├── CMakeLists.txt
├── src/
│   ├── main.cpp
│   ├── core/       — simulation backend (gates, wires, circuit logic)
│   └── gui/        — the interactive interface
├── include/dls/    — shared type definitions
└── resources/fonts/Inter-Regular.ttf
```
