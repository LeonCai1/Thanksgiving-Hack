# Thanksgiving-Hack
Server side branch
# A gRPC based concurrent ray-tracing rendering farm
## Description
Improved the ray-tracing algorithm using concurrent programming based on [RayTracingInOneWeekend](https://raytracing.github.io/books/RayTracingInOneWeekend.html).

## Setup

### Envirenment path

Choose a directory to hold locally installed packages. This page assumes that the environment variable `MY_INSTALL_DIR` holds this directory path. For example:

```sh
$ export MY_INSTALL_DIR=$HOME/.local
```

Ensure that the directory exists:

```sh
$ mkdir -p $MY_INSTALL_DIR
```

Add the local `bin` folder to your path variable, for example:

```sh
$ export PATH="$MY_INSTALL_DIR/bin:$PATH"
```

### Install cmake

You need version 3.13 or later of `cmake`. Install it by following these instructions:

- Linux

  ```sh
  $ sudo apt install -y cmake
  ```

- macOS:

  ```sh
  $ brew install cmake
  ```

- For general `cmake` installation instructions, see [Installing CMake](https://cmake.org/install).

Check the version of `cmake`:

```sh
$ cmake --version
cmake version 3.19.6
```

**(Not necessary to camke version after 3.19.6)**Under Linux, the version of the system-wide `cmake` can often be too old. You can install a more recent version into your local installation directory as follows:

```sh
$ wget -q -O cmake-linux.sh https://github.com/Kitware/CMake/releases/download/v3.19.6/cmake-3.19.6-Linux-x86_64.sh
$ sh cmake-linux.sh -- --skip-license --prefix=$MY_INSTALL_DIR
$ rm cmake-linux.sh
```

### Install other required tools

Install the basic tools required to build gRPC:

- Linux

  ```sh
  $ sudo apt install -y build-essential autoconf libtool pkg-config
  ```

- macOS:

  ```sh
  $ brew install autoconf automake libtool pkg-config
  ```

### Clone the repo

```sh
$ git clone -b main https://github.com/LeonCai1/Thanksgiving-Hack.git
```

### Build and install gRPC and Protocol Buffers

While not mandatory, gRPC applications usually leverage [Protocol Buffers](https://developers.google.com/protocol-buffers) for service definitions and data serialization, and the example code uses [proto3](https://developers.google.com/protocol-buffers/docs/proto3).

The following commands build and locally install gRPC and Protocol Buffers:

```sh
$ cd grpc
$ mkdir -p cmake/build
$ pushd cmake/build
$ cmake -DgRPC_INSTALL=ON \
      -DgRPC_BUILD_TESTS=OFF \
      -DCMAKE_INSTALL_PREFIX=$MY_INSTALL_DIR \
      ../..
$ make -j 4
$ make install
$ popd
```

Note: We **strongly** encourage you to install gRPC *locally* — using an appropriately set `CMAKE_INSTALL_PREFIX` — because there is no easy way to uninstall gRPC after you’ve installed it globally.

## Now Build Your Project

1. Change to src's directory

   ```sh
   $ cd src
   ```

2. Build the project using `cmake`

   ```sh
   $ mkdir -p cmake/build
   $ pushd cmake/build
   $ cmake -DCMAKE_PREFIX_PATH=$MY_INSTALL_DIR ../..
   $ make -j 4
   ```

## Try it! (have to edit )

Run the example from the example **build** directory `examples/cpp/helloworld/cmake/build`:

1. Run the server:

   ```sh
   $ ./greeter_server
   ```

2. From a different terminal, run the client and see the client output:

   ```sh
   $ ./greeter_client
   Greeter received: Hello world
   ```

Congratulations! You’ve just run a client-server application with gRPC.




## Workflow

