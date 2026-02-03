# New World

A project that marks a new era, a new world, if you like to call it that.

## Documentation

### Requirements

```txt
clangd
clang-format
cmake
g++
node
```

### Developer dependencies

```bash
sudo dnf install cmake-format clang-tools-extra
```

### Commands

Clone the repo:

```bash
git clone --recursive your-repo
```

Otherwise, update submodules:

```bash
git submodule update --init --recursive
```

Install dependencies:

Fedora:

We have to install `SDL2-devel` dependency because it provides proper cmake configuration which allows cmake to find and link it.

```bash
sudo dnf install SDL2 SDL2-devel mesa-libGL mesa-libGL-devel libcurl libcurl-devel
```

First time:

```bash
cmake -S . -B build
```

When adding files:

```bash
cmake --build build
```

When build config changes:

```bash
cmake -S . -B build
```
