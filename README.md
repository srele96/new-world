# New World

...

## Documentation

### Requirements

```txt
clangd
clang-format
cmake
g++
node
```

### Commands

First time:

```bash
cmake -S . -B build -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
```

When adding files:

```bash
cmake --build build
```

When build config changes:

```bash
cmake -S . -B build
```
