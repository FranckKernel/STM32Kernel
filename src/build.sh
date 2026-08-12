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

DEBUG_OR_RELEASE="${1:-release}"
QEMU_OR_REAL_MACHINE="${2:-real}"

# ============= DIRECTORIES
BUILD_DIR="./build"

GPIO="./gpio"
STDLIB="./stdlib"
CPU="./cpu"

# ============= FLAGS
USE_LIBC="false" # "true" or "false"

CFLAGS=(
	"-mcpu=cortex-m4"
	"-mthumb"
	"-ffreestanding"
	"-fno-builtin"
	"-g"
	"-std=c23"
	"-O0"
	"-ffunction-sections"
	"-fdata-sections"
)

LDFLAGS=(
	"-nostdlib"     # prevents automatic link of standard libs
	"-nostartfiles" # prevents automatic startup files
	# -nostdlib does no startfiles and -nodefaultlibs. this way, you can comment in and out which you want
	# -nodefaultlibs is kinda undone by -lgcc and the rest
	"-Wl,--gc-sections" # garbage‑collect unused sections
)

LDFLAGS_LIBC=(
	"-specs=nosys.specs" # newlib syscall stubs
	"-specs=nano.specs"  # newlib‑nano optimisations
	"-Wl,--start-group"
	"-lm"
	"-lc"
	# "-lnosys"

	# if i didn't have an empty syscall.c, then libnosys would be needed
	# but its good to have it here so i remember it exist
	"-Wl,--end-group"
)

if [ "$USE_LIBC" = "true" ]; then
	LDFLAGS+=("${LDFLAGS_LIBC[@]}")
	CFLAGS+=("-DUSE_LIBC")
else
	echo 123
fi
# ✅ -lgcc goes dead last, unconditionally
LDFLAGS+=("-lgcc")

mkdir -p "$BUILD_DIR"

echo "[CC] compiling..."

$CC "${CFLAGS[@]}" -c kernel/startup.s -o "$BUILD_DIR"/startup.o
$CC "${CFLAGS[@]}" -c kernel/main.c -o "$BUILD_DIR"/main.o "-I$GPIO" "-I$STDLIB" "-I$CPU"

$CC "${CFLAGS[@]}" -c "./stdlib/syscall.c" -o "$BUILD_DIR"/syscall.o "-I$GPIO"

$CC "${CFLAGS[@]}" -c "$GPIO/gpio.c" -o "$BUILD_DIR"/gpio.o
$CC "${CFLAGS[@]}" -c "$CPU/clock.c" -o "$BUILD_DIR"/clock.o "-I$GPIO"

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

if [[ "$QEMU_OR_REAL_MACHINE" == "real" ]]; then
	echo "[RUN]: Flashing the kernel"
	st-flash write "$BUILD_DIR/$TARGET.bin" 0x8000000
	st-flash reset
	if [[ "$DEBUG_OR_RELEASE" == "debug" ]]; then
		st-util
	fi
else
	echo "[RUN]: Flashing the os in a simulated Renode"
	if [[ "$DEBUG_OR_RELEASE" == "debug" ]]; then
		# renode --console debug.resc
		echo "[RUN]: Running the os in a simulated Renode (DEBUG MODE)."
		renode debug.resc &
		sleep 1
		RENODE_PID="$(pgrep -f "Renode.dll")"
		move_pid_to_workspace "$RENODE_PID" "$MOV_WORKSPACE"
	else
		# renode --console run.resc
		echo "[RUN]: Running the os in a simulated Renode (RELEASE MODE)."
		renode run.resc &
		sleep 1
		RENODE_PID="$(pgrep -f "Renode.dll")"
		move_pid_to_workspace "$RENODE_PID" "$MOV_WORKSPACE"

	fi
fi
