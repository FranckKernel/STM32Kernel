// ============================================================================
// GPIO MMIO
// ============================================================================

const GPIO_MMIO_ADDRESS_BASE: u32 = 0x4002_0000;
const GPIO_MMIO_LETTER_DIFF: u32 = 0x400;

const GPIO_MMIO_A_BASE: u32 = GPIO_MMIO_ADDRESS_BASE;
const GPIO_MMIO_B_BASE: u32 = GPIO_MMIO_A_BASE + 1 * GPIO_MMIO_LETTER_DIFF;
const GPIO_MMIO_C_BASE: u32 = GPIO_MMIO_A_BASE + 2 * GPIO_MMIO_LETTER_DIFF;
const GPIO_MMIO_D_BASE: u32 = GPIO_MMIO_A_BASE + 3 * GPIO_MMIO_LETTER_DIFF;
const GPIO_MMIO_E_BASE: u32 = GPIO_MMIO_A_BASE + 4 * GPIO_MMIO_LETTER_DIFF;
const GPIO_MMIO_F_BASE: u32 = GPIO_MMIO_A_BASE + 5 * GPIO_MMIO_LETTER_DIFF;
const GPIO_MMIO_G_BASE: u32 = GPIO_MMIO_A_BASE + 6 * GPIO_MMIO_LETTER_DIFF;

// ============================================================================
// GPIO PORT MODE
// ============================================================================

pub const GpioPortMode = enum(u2) {
    input = 0b00,
    output = 0b01,
    alternate_function = 0b10,
    analog = 0b11,
};

const GpioPortModeRegister = packed struct {
    pin: [16]GpioPortMode,
};

comptime {
    if (@sizeOf(GpioPortModeRegister) != 4)
        @compileError("GpioPortModeRegister must be 4 bytes");
}

// ============================================================================
// GPIO PORT OUTPUT TYPE
// ============================================================================

const GpioPortOutputType = enum(u1) {
    push_pull = 0b0,
    open_drain = 0b1,
};

const GpioPortOutputTypeRegister = packed struct {
    pin: [16]GpioPortOutputType,
    reserved: u16,
};

comptime {
    if (@sizeOf(GpioPortOutputTypeRegister) != 4)
        @compileError("GpioPortOutputTypeRegister must be 4 bytes");
}

// ============================================================================
// GPIO PORT OUTPUT SPEED
// ============================================================================

pub const GpioPortOutputSpeed = enum(u2) {
    low = 0b00,
    medium = 0b01,
    fast = 0b10,
    high = 0b11,
};

const GpioPortOutputSpeedRegister = packed struct {
    pin: [16]GpioPortOutputSpeed,
};

comptime {
    if (@sizeOf(GpioPortOutputSpeedRegister) != 4)
        @compileError("GpioPortOutputSpeedRegister must be 4 bytes");
}

// ============================================================================
// GPIO PORT PULL MODE
// ============================================================================

pub const GpioPortPullMode = enum(u2) {
    none = 0b00,
    up = 0b01,
    down = 0b10,
};

const GpioPortPullModeRegister = packed struct {
    pin: [16]GpioPortPullMode,
};

comptime {
    if (@sizeOf(GpioPortPullModeRegister) != 4)
        @compileError("GpioPortPullModeRegister must be 4 bytes");
}

// ============================================================================
// GPIO PORT INPUT DATA
// ============================================================================

pub const GpioInputData = enum(u1) {
    low = 0b0,
    high = 0b1,
};

const GpioPortInputDataRegister = packed struct {
    pin: [16]GpioInputData,
    reserved: u16,
};

comptime {
    if (@sizeOf(GpioPortInputDataRegister) != 4)
        @compileError("GpioPortInputDataRegister must be 4 bytes");
}

// ============================================================================
// GPIO PORT OUTPUT DATA
// ============================================================================

pub const GpioOutputData = enum(u1) {
    low = 0b0,
    high = 0b1,
};

const GpioPortOutputDataRegister = packed struct {
    pin: [16]GpioOutputData,
    reserved: u16,
};

comptime {
    if (@sizeOf(GpioPortOutputDataRegister) != 4)
        @compileError("GpioPortOutputDataRegister must be 4 bytes");
}

// ============================================================================
// GPIO PORT BIT SET / RESET
// ============================================================================

const GpioPortBitSet = enum(u1) {
    no_action = 0b0,
    set = 0b1,
};

const GpioPortBitReset = enum(u1) {
    no_action = 0b0,
    reset = 0b1,
};

const GpioPortBitSetResetRegister = packed struct {
    set: [16]GpioPortBitSet,
    reset: [16]GpioPortBitReset,
};

comptime {
    if (@sizeOf(GpioPortBitSetResetRegister) != 4)
        @compileError("GpioPortBitSetResetRegister must be 4 bytes");
}

// ============================================================================
// GPIO REGISTER BLOCK
// ============================================================================

const GpioXRegister = struct {
    mode: GpioPortModeRegister, // 0x00
    output_type: GpioPortOutputTypeRegister, // 0x04
    output_speed: GpioPortOutputSpeedRegister, // 0x08
    pull_mode: GpioPortPullModeRegister, // 0x0C
    input_data: GpioPortInputDataRegister, // 0x10
    output_data: GpioPortOutputDataRegister, // 0x14
    bit_set_reset: GpioPortBitSetResetRegister, // 0x18
};

comptime {
    if (@sizeOf(GpioXRegister) != 0x1C)
        @compileError("GpioXRegister must be 0x1C bytes");
}

// ============================================================================
// GPIO PORT LETTER
// ============================================================================

pub const GpioPortLetter = enum(u8) {
    a = 0,
    b = 1,
    c = 2,
    d = 3,
    e = 4,
    f = 5,
    g = 6,
};

// ============================================================================
// PRIVATE GPIO PORT POINTERS
// ============================================================================

const gpio_c = struct {
    const a: *volatile GpioXRegister =
        @ptrFromInt(GPIO_MMIO_A_BASE);

    const b: *volatile GpioXRegister =
        @ptrFromInt(GPIO_MMIO_B_BASE);

    const c: *volatile GpioXRegister =
        @ptrFromInt(GPIO_MMIO_C_BASE);

    const d: *volatile GpioXRegister =
        @ptrFromInt(GPIO_MMIO_D_BASE);

    const e: *volatile GpioXRegister =
        @ptrFromInt(GPIO_MMIO_E_BASE);

    const f: *volatile GpioXRegister =
        @ptrFromInt(GPIO_MMIO_F_BASE);

    const g: *volatile GpioXRegister =
        @ptrFromInt(GPIO_MMIO_G_BASE);
};

// ============================================================================
// PRIVATE GPIO PORT ARRAY
// ============================================================================

const gpio: [7]*volatile GpioXRegister = .{
    @ptrFromInt(GPIO_MMIO_A_BASE),
    @ptrFromInt(GPIO_MMIO_B_BASE),
    @ptrFromInt(GPIO_MMIO_C_BASE),
    @ptrFromInt(GPIO_MMIO_D_BASE),
    @ptrFromInt(GPIO_MMIO_E_BASE),
    @ptrFromInt(GPIO_MMIO_F_BASE),
    @ptrFromInt(GPIO_MMIO_G_BASE),
};

const gpio_struct = struct {
    pub const a: *volatile GpioXRegister = @ptrFromInt(GPIO_MMIO_A_BASE);
    pub const b: *volatile GpioXRegister = @ptrFromInt(GPIO_MMIO_B_BASE);
    pub const c: *volatile GpioXRegister = @ptrFromInt(GPIO_MMIO_C_BASE);
    pub const d: *volatile GpioXRegister = @ptrFromInt(GPIO_MMIO_D_BASE);
    pub const e: *volatile GpioXRegister = @ptrFromInt(GPIO_MMIO_E_BASE);
    pub const f: *volatile GpioXRegister = @ptrFromInt(GPIO_MMIO_F_BASE);
    pub const g: *volatile GpioXRegister = @ptrFromInt(GPIO_MMIO_G_BASE);
};

// ============================================================================
// PUBLIC GPIO API
// ============================================================================
pub fn gpioPortModeSetup(port: GpioPortLetter, pin: u8, mode: GpioPortMode) void {
    gpio[port].mode.pin[pin] = mode;
}

pub fn gpioOutputTypeSetup(port: GpioPortLetter, pin: u8, output_type: GpioPortOutputType) void {
    gpio[port].output_type.pin[pin] = output_type;
}

pub fn gpioPullModeSetup(port: GpioPortLetter, pin: u8, pull_mode: GpioPortPullMode) void {
    gpio[port].pull_mode.pin[pin] = pull_mode;
}

pub fn gpioOutputSpeedSetup(port: GpioPortLetter, pin: u8, output_speed: GpioPortOutputSpeed) void {
    gpio[port].output_speed.pin[pin] = output_speed;
}

pub fn gpioWrite(port: GpioPortLetter, pin: u8, level: GpioOutputData) void {
    switch (level) {
        .high => gpio[port].bit_set_reset.set[pin] = .set,
        .low => gpio[port].bit_set_reset.reset[pin] = .reset,
    }
}

pub fn gpioRead(port: GpioPortLetter, pin: u8) GpioInputData {
    return gpio[port].input_data.pin[pin];
}

// ============================================================================
// LD2
// ============================================================================

pub fn setLD2(level: GpioOutputData) void {
    // change the state of the second LED
    gpioWrite(.a, 5, level);
}
