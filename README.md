# clang-diff

## Prerequisites

First, you need to download LLVM and Clang sources.

```bash
$ mkdir ~/clang-llvm && cd ~/clang-llvm
$ git clone http://llvm.org/git/llvm.git
$ cd llvm/tools
$ git clone http://llvm.org/git/clang.git
$ cd clang/tools
$ git clone http://llvm.org/git/clang-tools-extra.git extra
```

Next you need to obtain the CMake build system and Ninja build tool.

```bash
$ git clone https://github.com/martine/ninja.git
$ cd ninja
$ git checkout release
$ ./bootstrap.py
$ sudo cp ninja /usr/local/bin/
$ sudo chmod a+rx /usr/local/bin/ninja
```

```bash
$ cd ~/clang-llvm
$ git clone git://cmake.org/cmake.git
$ cd cmake
$ git checkout release
$ ./bootstrap
$ make
$ sudo make install
```

Now we'll build Clang.

```bash
$ cd ~/clang-llvm
$ mkdir build && cd build
$ cmake -G Ninja -DCMAKE_EXPORT_COMPILE_COMMANDS=ON ../llvm
$ ln -s $PWD/compile_commands.json ../llvm
$ ninja check-all
```

## Usage

First you need to checkout the source code into Clang's tool directory.

```bash
$ cd llvm/tools/clang/tools
$ git clone https://github.com/petrhosek/clang-diff.git
$ echo 'add_subdirectory(clang-diff)' >> CMakeLists.txt
```

Now, we can build the tool.

```bash
$ cd ~/clang-llvm/build
$ ninja
```

You should now be able to run the tool which is located in `~/clang-llvm/build/bin`, on any source file.

```bash
$ ~/clang-llvm/bin/clang-diff foo.cpp bar.cpp
```
