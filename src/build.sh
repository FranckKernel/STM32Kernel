#!/bin/bash
set -eou pipefail
shopt -s nullglob

TARGET=kernel

CC=arm-none-eabi-gcc
DUMP=arm-none-eabi-objdump
OBJCOPY=arm-none-eabi-objcopy

# ============= DIRECTORIES
BUILD_DIR="./build"

GPIO="./gpio"

# ============= FLAGS
CFLAGS=(
	"-mcpu=cortex-m4"
	"-mthumb"
	"-ffreestanding"
	"-fno-builtin"
	"-g"
	"-std=c23"
	"-O3"
)

LDFLAGS=(
	"-nostdlib"
	"-Wl,--gc-sections"
	"-lgcc"
)

mkdir -p "$BUILD_DIR"

echo "[CC] compiling..."

$CC "${CFLAGS[@]}" -c kernel/startup.s -o "$BUILD_DIR"/startup.o
$CC "${CFLAGS[@]}" -c kernel/main.c -o "$BUILD_DIR"/main.o "-I$GPIO"

$CC "${CFLAGS[@]}" -c "$GPIO/gpio.c" -o "$BUILD_DIR"/gpio.o

echo "[LD] linking..."

objects=("$BUILD_DIR"/*.o)

$CC "${CFLAGS[@]}" \
	"${objects[@]}" \
	"${LDFLAGS[@]}" \
	-T "./linker.ld" \
	-o "$BUILD_DIR/$TARGET.elf"

echo "[BIN] generating binary..."

$OBJCOPY \
	-O binary \
	"$BUILD_DIR/$TARGET.elf" \
	"$BUILD_DIR/$TARGET.bin"

$DUMP "$BUILD_DIR/$TARGET.elf" -D >"$BUILD_DIR/$TARGET.dump"

st-flash write "$BUILD_DIR/$TARGET.bin" 0x8000000

# Only if debug mode
# st-util
