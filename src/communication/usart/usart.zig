const std = @import("std");
const macros = @import("macros");
// const macros_direct = @import("../../common/macros.zig");

const read32 = macros.read32;
const write32 = macros.write32;

const USART2_BASE: u32 = 0x4000_4400; // The main one connected to my machine
// APB1

const USART1_BASE: u32 = 0x4001_1000;
const USART6_BASE: u32 = 0x4001_1400;
// AHB2

// The peripheral registers have to be accessed by half-words (16 bits) or words (32 bits).

// ================================ USART Data Register (DR)

const UsartData = packed struct(u32) {
    data: u9,
    _reserved: u23 = 0,
};

const usartDataCheck = {
    if (@bitSizeOf(UsartData) != 32)
        @compileError("UsartData must be 32 bits");
};

// ================================== USART Baud Rate Register (BRR)
// Usart Divider = DIV_Mantissa +  { DIV_Fraction / [8 × (2 – OVER8)] }
// With 96Mhz, we need Divider = 2500
const UsartBRR = packed struct(u32) {
    div_fraction: u4 = 0,
    div_mantissa: u12 = 0,
    // It's fixed signed representation. mantissa = whole number
    // fraction = bits of minus index. (2^-1, 2^-2, 2^-3, 2^-4)
    _reserved: u16 = 0,
};

comptime {
    if (@bitSizeOf(UsartBRR) != 32)
        @compileError("UsartBRR must be 32 bits");
}

// =================================== USART Status Register (SR)
const UsartStatus = packed struct(u32) {
    parity_error: bool = false, // Set by hardware. Cleared by software by r/w DR after RXNE set. (need to poll rxne, or interrupt rxne)
    framing_error: bool = false, // r only
    noise_detected_flag: bool = false, // r only
    overrun_error: bool = false, // r only
    idle_detected: bool = false, // r only

    rxne: bool = false, // read data register not empty
    transmission_complete: bool = false,
    // A frame is one complete unit of serial transmission. (start, 8 bits of data, 1 bit parity (optional), stop)
    transmit_data_register_empty: bool = false,

    lin_break_detected: bool = false,
    cts_flag: bool = false, // cts = clear to send. Used in Hardware Control Flow

    _reserved: u22 = 0,
};

comptime {
    if (@bitSizeOf(UsartStatus) != 32)
        @compileError("UsartStatus must be 32 bits");
}

// ================================== USART Control Register 1 (CR1)

pub const UsartWordLength = enum(u1) {
    bits8 = 0b0,
    bits9 = 0b1,
};

pub const UsartOversampling = enum(u1) {
    // Commonly called OVER8 (Which is ass backward!)
    @"16" = 0b0,
    @"8" = 0b1,
};

const UsartCR1 = packed struct(u32) {
    send_break: bool = false, // bit 0
    recieved_wakeup: bool = false, // bit 1
    reciever_enable: bool = false, // bit 2
    transmitter_enable: bool = false, // bit 3
    idle_interrupt_enable: bool = false, // bit 4
    rxne_interrupt_enable: bool = false, // bit 5
    // recieve data register not empty interrupt enable. (Get an interrupt every time you receieve data)
    tcie_interrupt_enable: bool = false, // bit 6  Transmission complete interrupt enable
    txe_interrupt_enable: bool = false, // bit 7  Transmitter data register empty interrupt enable
    parity_error_interrupt_enable: bool = false, // bit 8

    parity_selection_odd_not_even: bool = false, // bit 9

    parity_control_enable: bool = false, // To activate or not the parity
    wakeup_method: bool = false,

    word_length: UsartWordLength = UsartWordLength.bits8,

    usart_enable: bool = false,
    _reserved_bit14: u1 = 0,
    oversampling: UsartOversampling = UsartOversampling.@"16", // (RG note) : called over8 refers to. over8 is the oversampling

    _reserved: u16 = 0,
};

comptime {
    if (@bitSizeOf(UsartCR1) != 32)
        @compileError("UsartCR1 must be 32 bits");
}

// ================================== USART Control Register 2 (CR2)

pub const UsartStopBits = enum(u2) {
    @"1" = 0b00,
    @"0.5" = 0b01,
    @"2" = 0b10,
};

const UsartCR2 = packed struct(u32) {
    address: u4 = 0, // bit 0-3
    _reserved1: u1 = 0, // bit 4
    line_break_detection: bool = false, // bit 5 (10 bit break = 0, 11 bit break = 1)
    lbd_interrupt_enable: bool = false, // bit 6
    _reserved2: u1 = 0, // bit 7
    last_bit_clock_pulse: bool = false, // bit 8 (Pretty much irrelevant)

    clock_phase: bool = false, // bit 9 can leave these two default
    clock_polarity: bool = false, // bit 10

    clock_enable: bool = false, // bit 11, Makes it usart, otherwise uart
    stop_bits: UsartStopBits = UsartStopBits.@"1", // bit 12-13
    lin_mode_enable: bool = false, // bit 14

    _reserved: u17,
};

comptime {
    if (@bitSizeOf(UsartCR2) != 32) {
        @compileError("UsartCR2 must be 32 bits");
    }
}

// ================================== USART Control Register 3 (CR3)

const UsartCR3 = packed struct(u32) {
    error_interrupt_enable: bool = false,
    irda_mode_enable: bool = false,
    irda_low_power: bool = false,
    half_duplex_selected: bool = false,
    smart_card_nack_enable: bool = false,
    smart_card_mode_enable: bool = false,
    dma_enable_reciever: bool = false,
    dma_enable_transmitter: bool = false,
    rts_hardware_control_flow_enabled: bool = false,
    cts_hardware_control_flow_enabled: bool = false,
    cts_interrupt_enable: bool = false,
    onebit: bool = false,

    _reserved: u20 = 0,
};

comptime {
    if (@bitSizeOf(UsartCR3) != 32) {
        @compileError("UsartCR3 must be 32 bits");
    }
}

// ===================================== The META Struct
// The meta struct CANNOT be packed struct, else some weird shit happen
// Normal
pub const Usart = extern struct {
    status: UsartStatus,
    data: UsartData,
    brr: UsartBRR,
    cr1: UsartCR1,
    cr2: UsartCR2,
    cr3: UsartCR3,
    gtpr: u32,
};

comptime {
    std.debug.assert(@offsetOf(Usart, "status") == 0x00);
    std.debug.assert(@offsetOf(Usart, "data") == 0x04);
    std.debug.assert(@offsetOf(Usart, "brr") == 0x08);
    std.debug.assert(@offsetOf(Usart, "cr1") == 0x0C);
    std.debug.assert(@offsetOf(Usart, "cr2") == 0x10);
    std.debug.assert(@offsetOf(Usart, "cr3") == 0x14);
    std.debug.assert(@offsetOf(Usart, "gtpr") == 0x18);
}

// ================================= The assignments
pub const usart2: *volatile Usart = @ptrFromInt(USART2_BASE);
pub const usart1: *volatile Usart = @ptrFromInt(USART1_BASE);
pub const usart6: *volatile Usart = @ptrFromInt(USART6_BASE);

// ================================ The public config struct :

pub const UsartBaudrate = enum(u32) {
    usart_baudrate_2400 = 2400,
    usart_baudrate_9600 = 9600,
    usart_baudrate_38400 = 38400,
    usart_baudrate_57600 = 57600,
    usart_baudrate_115200 = 115200,
    usart_baudrate_230400 = 230400,
};

pub const UsartParity = enum(u8) {
    usart_parity_none = 0,
    usart_parity_odd = 1,
    usart_parity_even = 2,
};

pub const UsartMode = enum(u8) {
    usart_mode_both = 0,
    usart_mode_transmitter,
    usart_mode_reciever,
};

pub const UsartHardwareControl = enum(u8) {
    usart_hwc_none,
    usart_hwc_rts,
    usart_hwc_cts,
    usart_hwc_rts_cts,
};

const UsartConfig = struct {
    baudrate: UsartBaudrate,
    word_length: UsartWordLength,
    stop_bit: UsartStopBits,
    parity: UsartParity,
    mode: UsartMode,
    hardware_control: UsartHardwareControl,
    oversampling_not_over8: UsartOversampling,

    clock_info: struct {
        clock_enable: bool,
        clock_polarity: bool,
        clock_phase: bool,
    },
};

// ================================ The functions

pub fn config_usart(usart: *volatile Usart, config: *const UsartConfig) void {
    //

    _ = config;
    _ = usart;
}

extern fn get_system_tick() i32;

const sr: *volatile u32 = @ptrFromInt(0x4000_4400);
const dr: *volatile u32 = @ptrFromInt(0x4000_4404);

pub fn write_u8_raw(value: u8) void {
    while (sr.* & (1 << 7) == 0) {} // wait for TXE
    dr.* = value;
}

const max_system_tick_diff: u32 = 100000;

pub fn write_u8(usart: *volatile Usart, value: u8) bool {
    //
    const system_tick = get_system_tick();
    while (!read32(&usart.status).transmit_data_register_empty) {
        // -
        const new_st: i32 = get_system_tick();
        if (new_st - system_tick >= max_system_tick_diff) {
            // With this check, if the scheduler is called twice while this runs
            // We leave early, something bad happened
            return true;
        }
    }

    const data_write: UsartData = .{ .data = value };

    macros.write32(&usart.data, data_write);
    return false;
}

pub noinline fn write_u8_nocheck(usart: *volatile Usart, value: u8) bool {
    //
    // while (!read32(&usart.status).transmit_data_register_empty) {
    //     // -
    //     asm volatile ("nop");
    // }

    // const data_write: UsartData = .{ .data = value };

    // macros.write32(&usart.data, data_write);
    macros.forceWrite32(&usart.data, value);
    return false;
}

pub fn write(usart: *volatile Usart, str: []const u8) usize {
    const system_tick = get_system_tick();
    for (0..str.len) |i| {
        const timed_out: bool = write_u8(usart, str[i]);
        if (timed_out or get_system_tick() - system_tick >= max_system_tick_diff)
        // The scheduler is very slow.
        // It is completely possible to stall due to writting.
        // Baud rate = 38kHz.
        // Scheduler = 10 Khz
        // 8 bit + start + stop = 10 bit ber byte.
        // 512 char = 512 * 10 message.
        // Tick count = 5120 * 10000/38400 = 1333.3-
        // With 1000 byte consecutive is even worse.
        // Hence I'm starting to see why one would want to use a DMA setup for printing
        // TODO: DMA usart printing
        {
            return @intCast(i);
        }
    }
    return str.len;
}

pub fn usart2_write(str: []const u8) usize {
    return write(usart2, str);
}
