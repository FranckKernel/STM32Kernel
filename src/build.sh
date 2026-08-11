#!/bin/bash
set -eou pipefail
shopt -s nullglob

MOV_WORKSPACE=21 # In hyprland, move it there

function find_git_root() {
	local dir=${1:-$PWD} # start from given dir or current directory
	while [[ "$dir" != "/" ]]; do
		if [[ -d "$dir/.git" ]]; then
			echo "$dir"
			return 0
		fi
		dir=$(dirname "$dir")
	done
	echo "No git repository found" >&2
	return 1
}
project_root=$(find_git_root)
src="$project_root/src/"
cd "$src" || { # Tries to cd to "src" relative to current dir
	echo "Could not cd to $src."
	exit 1
}

TARGET=kernel

CC=arm-none-eabi-gcc
DUMP=arm-none-eabi-objdump
OBJCOPY=arm-none-eabi-objcopy

FLASH_NOT_EMULATE="false"
DEBUG="false"

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
	"-specs=nosys.specs"
	"-Wl,--start-group"
	"-lm"
	"-lc"
	"-lgcc"
	"-Wl,--end-group"
)

mkdir -p "$BUILD_DIR"

echo "[CC] compiling..."

$CC "${CFLAGS[@]}" -c kernel/startup.s -o "$BUILD_DIR"/startup.o
$CC "${CFLAGS[@]}" -c kernel/main.c -o "$BUILD_DIR"/main.o "-I$GPIO"

$CC "${CFLAGS[@]}" -c "./stdlib/syscall.c" -o "$BUILD_DIR"/syscall.o "-I$GPIO"

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

if [[ "$FLASH_NOT_EMULATE" == "true" ]]; then
	echo "[RUN]: Flashing the os in a simulated Renode"
	st-flash write "$BUILD_DIR/$TARGET.bin" 0x8000000
	if [[ "$DEBUG" == "true" ]]; then
		st-util
	fi
else
	echo "[RUN]: Running the os in a simulated Renode"
	if [[ "$DEBUG" == "true" ]]; then
		# renode --console debug.resc
		renode debug.resc &
		sleep 1
		RENODE_PID="$(pgrep -f "Renode.dll")"
		move_pid_to_workspace "$RENODE_PID" "$MOV_WORKSPACE"
	else
		# renode --console run.resc
		renode run.resc &
		sleep 1
		RENODE_PID="$(pgrep -f "Renode.dll")"
		move_pid_to_workspace "$RENODE_PID" "$MOV_WORKSPACE"

	fi
fi
