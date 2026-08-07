#!/bin/bash
set -eou pipefail
shopt -s nullglob

TARGET=kernel

BUILD_DIR="./build"

CC=arm-none-eabi-gcc
DUMP=arm-none-eabi-objdump
OBJCOPY=arm-none-eabi-objcopy

CFLAGS=(
	"-mcpu=cortex-m4"
	"-mthumb"
	"-ffreestanding"
	"-fno-builtin"
	"-nostdlib"
	"-g"
)

LDFLAGS=(
	"-nostdlib"
	"-Wl,--gc-sections"
)

mkdir -p "$BUILD_DIR"

echo "[CC] compiling..."

$CC "${CFLAGS[@]}" -c kernel/startup.s -o "$BUILD_DIR"/startup.o

$CC "${CFLAGS[@]}" -c kernel/main.c -o "$BUILD_DIR"/main.o

echo "[LD] linking..."

objects=("$BUILD_DIR"/*.o)

$CC "${CFLAGS[@]}" \
	"${LDFLAGS[@]}" \
	-T "./linker.ld" \
	"${objects[@]}" \
	-o "$BUILD_DIR/$TARGET.elf"

echo "[BIN] generating binary..."

$OBJCOPY \
	-O binary \
	"$BUILD_DIR/$TARGET.elf" \
	"$BUILD_DIR/$TARGET.bin"

$DUMP "$BUILD_DIR/$TARGET.elf" -D >"$BUILD_DIR/$TARGET.dump"

st-flash write "$BUILD_DIR/$TARGET.bin" 0x8000000

st-util
