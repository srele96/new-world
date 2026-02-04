# New World

A project that marks a new era, a new world, if you like to call it that.

A new era, a new world, where i free of my past limitations.

## Documentation

A section containing the project documentation.

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

#### You will need Boost

I installed boost doing it the recommended way. Clone, then build. I don't use Windows.

Get boost as specified here: <https://www.boost.org/doc/user-guide/getting-started.html>

I used default location: `/usr/local`.

**You need to run `sudo ./b2 install` as root, because `/usr/local` is system directory. _Otherwise, `b2` won't show errors, but boost won't be installed either._**

You can find more about boost, and its documentation, here: <https://www.boost.org/libraries/latest/libs/>

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
