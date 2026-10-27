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

print_array() {
	printf -- "\n\n%s:\n" "$2"
	local -n arr_ref=$1 # Use nameref to pass array by name
	for elem in "${arr_ref[@]}"; do
		echo "$elem"
	done
}

TARGET=kernel

CC=arm-none-eabi-gcc
DUMP=arm-none-eabi-objdump
OBJCOPY=arm-none-eabi-objcopy

# CC=x86_64-elf-gcc
# DUMP=x86_64-elf-objdump

DEBUG_OR_RELEASE="${1:-release}"
RENODE_OR_REAL_MACHINE="${2:-real}"

ZIG_RELEASE_FLAG=()
############ Pick one of the 3:##################
ZIG_RELEASE_MODE="fast"
# ZIG_RELEASE_MODE="safe"
# ZIG_RELEASE_MODE="small"
#################################################

if [[ "$DEBUG_OR_RELEASE" == "debug" ]]; then
	ZIG_RELEASE_FLAG+=("--release=off") # debug is default
else
	ZIG_RELEASE_FLAG+=("--release=$ZIG_RELEASE_MODE")

fi

# ============= DIRECTORIES
BUILD_DIR="./build"
LIB_OUT_DIR="zig-out/lib"
ZIG_LIB_NAME="kernel"

GPIO="./gpio"
STDIO="./stdio"
STDLIB="./stdlib"
COMMON="./common"
TIMERS="./timers"
FLASH="./flash"
TASKS="./tasks"
RCC="./rcc"

CPU="./cpu"
FPU="./cpu/fpu"
NVIC="./cpu/nvic"

USART="./communication/usart"
DMA="./dm/"

# ============= FLAGS
USE_LIBC="false" # "true" or "false"

CFLAGS=(
	"-mcpu=cortex-m4"
	"-mthumb"
	"-mfloat-abi=hard"  # matches Zig's eabihf
	"-mfpu=fpv4-sp-d16" # F446 FPU: single precision only
	"-ffreestanding"
	"-fno-builtin"
	"-g"
	"-std=c23"
	"-O3"
	"-ffunction-sections"
	"-fdata-sections"
	"-flto"

	"-Wall"
	"-Wextra"
	"-Wdouble-promotion"          # warns when float silently becomes double (software-emulated)
	"-fsingle-precision-constant" # 1.5 is float, not double
)

LDFLAGS=(
	"-nostdlib"     # prevents automatic link of standard libs
	"-nostartfiles" # prevents automatic startup files
	# -nostdlib does no startfiles and -nodefaultlibs. this way, you can comment in and out which you want
	# -nodefaultlibs is kinda undone by -lgcc and the rest
	"-Wl,--gc-sections" # garbage‑collect unused sections
	# "-flto"
	"-lm"
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
$CC "${CFLAGS[@]}" -c kernel/vectors.s -o "$BUILD_DIR"/vectors.o
$CC "${CFLAGS[@]}" -c kernel/isr_default.s -o "$BUILD_DIR"/isr_default.o

$CC "${CFLAGS[@]}" -c "$STDLIB/syscall.c" -o "$BUILD_DIR"/syscall.o "-I$GPIO" "-I$USART" "-I$COMMON"
$CC "${CFLAGS[@]}" -c "$STDLIB/string.c" -o "$BUILD_DIR"/string.o
$CC "${CFLAGS[@]}" -c "$STDIO/stdio.c" -o "$BUILD_DIR"/stdio.o "-I$STDLIB"

$CC "${CFLAGS[@]}" -c "./cpu/nvic/nvic.c" -o "$BUILD_DIR"/nvic.o "-I$COMMON"
$CC "${CFLAGS[@]}" -c "$FPU/fpu.c" -o "$BUILD_DIR"/fpu.o "-I$COMMON"

$CC "${CFLAGS[@]}" -c "$GPIO/gpio.c" -o "$BUILD_DIR"/gpio.o
$CC "${CFLAGS[@]}" -c "$TIMERS/timer.c" -o "$BUILD_DIR"/timer.o
$CC "${CFLAGS[@]}" -c "$TASKS/task.c" -o "$BUILD_DIR"/task.o "-I$GPIO" "-I$TIMERS" "-I$STDIO"
$CC "${CFLAGS[@]}" -c "$TASKS/scheduler.c" -o "$BUILD_DIR"/scheduler.o "-I$TIMERS" "-I$GPIO"
$CC "${CFLAGS[@]}" -c "$FLASH/flash.c" -o "$BUILD_DIR"/flash.o
$CC "${CFLAGS[@]}" -c "$RCC/clock.c" -o "$BUILD_DIR"/clock.o "-I$GPIO" "-I$FLASH"

$CC "${CFLAGS[@]}" -c "$USART/usart.c" -o "$BUILD_DIR"/usart.o "-I$COMMON" "-I$TIMERS" "-I$STDLIB" "-I$STDIO"

$CC "${CFLAGS[@]}" -c kernel/main.c -o "$BUILD_DIR"/main.o "-I$GPIO" "-I$STDLIB" "-I$RCC" "-I$COMMON" "-I$NVIC" "-I$TIMERS" "-I$TASKS" "-I$FLASH" "-I$USART" "-I$STDIO" "-I$FPU"

# ================ Zig work =================
zig build "${ZIG_RELEASE_FLAG[@]}" --verbose

LIBRARY_PATHS=(
	# "$BUILD_DIR"
	"$LIB_OUT_DIR"
)

LIBRARY_FILES=(
	"$ZIG_LIB_NAME" # don't put the lib or the
	# "libother.a"
)

LIBRARY_ARGS=()
for path in "${LIBRARY_PATHS[@]}"; do
	LIBRARY_ARGS+=("-L$path")
done

for lib in "${LIBRARY_FILES[@]}"; do
	LIBRARY_ARGS+=("-l$lib")
done

print_array LIBRARY_ARGS "LIBRARY_ARGS"

# ================ Linking work =================
echo "[LD] linking..."

objects=("$BUILD_DIR"/*.o)

$CC "${CFLAGS[@]}" \
	"${objects[@]}" \
	"${LIBRARY_ARGS[@]}" \
	"${LDFLAGS[@]}" \
	-T "./linker.ld" \
	-o "$BUILD_DIR/$TARGET.elf"

LIBRARY_PATHS=(
	"$LIB_OUT_DIR"
)

LIBRARY_FILES=(
	"$ZIG_LIB_NAME" # don't put the lib or the
	# "libother.a"
)

echo "[BIN] generating binary..."

$OBJCOPY \
	-O binary \
	"$BUILD_DIR/$TARGET.elf" \
	"$BUILD_DIR/$TARGET.bin"

$DUMP "$BUILD_DIR/nvic.o" -D -h >"$BUILD_DIR/nvic.dump"
$DUMP "$BUILD_DIR/$TARGET.elf" -D -h >"$BUILD_DIR/$TARGET.dump"
$DUMP "$BUILD_DIR/usart.o" -D -h >"$BUILD_DIR/usart.dump"
$DUMP "$BUILD_DIR/main.o" -D -h >"$BUILD_DIR/main.dump"

if [[ "$RENODE_OR_REAL_MACHINE" == "real" ]]; then

	echo "[RUN]: Flashing the kernel"
	st-flash --connect-under-reset write "$BUILD_DIR/$TARGET.bin" 0x8000000
	# st-flash reset # It's already reset
	if [[ "$DEBUG_OR_RELEASE" == "debug" ]]; then
		st-util
	else
		# {
		# 	sleep 1
		# 	st-flash reset
		# } &
		# picocom --baud 38400 --databits 8 --parity n --stopbits 1 --imap lfcrlf /dev/ttyACM0
		echo ""
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
