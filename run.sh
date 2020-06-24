if [ "$#" -eq 0 ]; then
echo "Usage: ./run.sh clean     rm all build file"
echo "Usage: ./run.sh {linux64|arm-linux-gnueabihf|aarch64-gnu}"
echo "         not set target, use default config ./run.sh linux"
echo "openssl 1.0.2g after, 1.1 before"
fi

BUILD_DIR=build
if [ "$1" = "clean" ]; then
    rm -rf "$BUILD_DIR"
    exit 0
fi

BUILD_TARGET=linux64
if [  x"$1" != x ]; then
    echo "build target set: $1"
    BUILD_TARGET=$1
    CMAKE_OPTS="toolchain/$1-toolchain.cmake"
fi

BUILD_DIR="$BUILD_DIR/$BUILD_TARGET"

mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

echo "Gonna make with target=$BUILD_TARGET .."

cmake \
    -DCMAKE_TOOLCHAIN_FILE="$CMAKE_OPTS"    \
    -DCMAKE_VERBOSE_MAKEFILE=ON             \
    -DCMAKE_SKIP_RPATH=OFF                 ../../

make