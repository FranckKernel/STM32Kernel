const std = @import("std");
const builtin = @import("builtin");

const macros = @import("macros");
// const stdio = @import("stdio");

// Rule for default value:
// Important, must be set (don't default initialize)
// Important, but has make sense default, then default to "make sense default"
// Don't care, default to hardware default value
// Reserved: Default to hardware default value (most likely 0)

// DMA registers must be address by 32 bit

const read32 = macros.read32;
const write32 = macros.write32;

var activation_flag: u32 = 0;
pub inline fn activateModuleDMA() void {
    // This serves only to force lazyness out, and make the lsp work
    // It doesn't actually activate it.
    // But by forcing a volatile write, I make sure that it does an operation that can't be optimized out
    const ptr: *volatile u32 = &activation_flag;
    ptr.* = 1;

    macros.activateModuleMacros();

    const x = testGetFlags();
    if (x) {
        return;
    }
    testClearFlags();

    // for (0..7) |i| {
    //     const q = get_stream_flag(dma1, @truncate(i));
    //
    //     const f: StreamFlagsClear = ;
    //     clear_stream_flags(dma1, @truncate(i), f);
    // }
    // Same thing, I want the lsp to go cross file and access stuff from macros
}

const DMA_BASE: u32 = 0x4002_6000; // The main one connected to my machine

const DMA1_BASE: u32 = DMA_BASE;
const DMA2_BASE: u32 = DMA_BASE + 0x400;

// ===================================== The DMA common registers

pub const StreamFlags = packed struct(u6) {
    fifo_error_interrupt_flag: bool = false, // bit 0
    _reserved: u1 = 0, // bit 1
    direct_mode_error_interrupt_flag: bool = false, // bit 2
    transfer_error_interrupt_flag: bool = false, // bit 3
    half_transfer_interrupt_flag: bool = false, // bit 4
    transfer_complete_interrupt_flag: bool = false, // bit 5
};
// Sadly, we can't have arrays of packed struct!!!!
// Imagine, a u16 made of two consecutive StreamFlags, followed by a u4 _reserved
// and then an array of 2 u16

comptime {
    std.debug.assert(@bitOffsetOf(StreamFlags, "fifo_error_interrupt_flag") == 0);
    std.debug.assert(@bitOffsetOf(StreamFlags, "direct_mode_error_interrupt_flag") == 2);
    std.debug.assert(@bitOffsetOf(StreamFlags, "transfer_error_interrupt_flag") == 3);
    std.debug.assert(@bitOffsetOf(StreamFlags, "half_transfer_interrupt_flag") == 4);
    std.debug.assert(@bitOffsetOf(StreamFlags, "transfer_complete_interrupt_flag") == 5);
}

const Lisr = packed struct(u32) {
    s0: StreamFlags = .{},
    s1: StreamFlags = .{},
    _r0: u4 = 0,
    s2: StreamFlags = .{},
    s3: StreamFlags = .{},
    _r1: u4 = 0,

    /// Flags of stream n (0..3 within this register).
    pub fn getFlags(self: Lisr, comptime streamIdx: u2) StreamFlags {
        return switch (streamIdx) {
            0 => self.s0,
            1 => self.s1,
            2 => self.s2,
            3 => self.s3,
        };
    }
};

comptime {
    macros.checkRegisterType32(Lisr);
}

// =========
const Hisr = packed struct(u32) {
    s4: StreamFlags = .{},
    s5: StreamFlags = .{},
    _r0: u4 = 0,
    s6: StreamFlags = .{},
    s7: StreamFlags = .{},
    _r1: u4 = 0,

    /// Flags of stream n (0..3 within this register).
    pub fn getFlags(self: Hisr, comptime streamIdx: u2) StreamFlags {
        return switch (streamIdx) {
            0 => self.s4,
            1 => self.s5,
            2 => self.s6,
            3 => self.s7,
        };
    }
};

comptime {
    macros.checkRegisterType32(Hisr);
}

// =========
pub const StreamFlagsClear = packed struct(u6) {
    clear_fifo_error_interrupt_flag: bool = false, // bit 0
    _reserved: u1 = 0, // bit 1
    clear_direct_mode_error_interrupt_flag: bool = false, // bit 2
    clear_transfer_error_interrupt_flag: bool = false, // bit 3
    clear_half_transfer_interrupt_flag: bool = false, // bit 4
    clear_transfer_complete_interrupt_flag: bool = false, // bit 5
};

const Lifcr = packed struct(u32) {
    s0: StreamFlagsClear = .{},
    s1: StreamFlagsClear = .{},
    _r0: u4 = 0,
    s2: StreamFlagsClear = .{},
    s3: StreamFlagsClear = .{},
    _r1: u4 = 0,

    pub fn clearFlags(comptime streamIdx: u2, f: StreamFlagsClear) Lifcr {
        var r: Lifcr = .{};
        switch (streamIdx) {
            0 => r.s0 = f,
            1 => r.s1 = f,
            2 => r.s2 = f,
            3 => r.s3 = f,
        }
        return r;
    }
};

comptime {
    macros.checkRegisterType32(Lifcr);
}

// =========
const Hifcr = packed struct(u32) {
    s4: StreamFlagsClear = .{},
    s5: StreamFlagsClear = .{},
    _r0: u4 = 0,
    s6: StreamFlagsClear = .{},
    s7: StreamFlagsClear = .{},
    _r1: u4 = 0,

    pub fn clearFlags(comptime streamIdx: u2, f: StreamFlagsClear) Hifcr {
        var r: Hifcr = .{};
        switch (streamIdx) {
            0 => r.s4 = f,
            1 => r.s5 = f,
            2 => r.s6 = f,
            3 => r.s7 = f,
        }
        return r;
    }
};
comptime {
    macros.checkRegisterType32(Hifcr);
}

// ===================================== The DMA Stream Registers

const FlowController = enum(u1) {
    dma = 0b0,
    peripheral = 0b1,
};

pub const Direction = enum(u2) {
    peripheral_to_memory = 0b00,
    memory_to_peripheral = 0b01,
    memory_to_memory = 0b10,
    // reserved = 0b11,
};

pub const DataSize = enum(u2) {
    bit8 = 0b00,
    bit16 = 0b01,
    bit32 = 0b10,
    // reserved = 0b11,
};

const OffsetSize = enum(u1) {
    use_psize = 0b0,
    use_4 = 0b1,
};

pub const PriorityLevel = enum(u2) {
    low = 0b00,
    medium = 0b01,
    high = 0b10,
    very_high = 0b11,
};

const Target = enum(u1) {
    m0ar = 0b0,
    m1ar = 0b1,
};

const BurstConfig = enum(u2) {
    single_transfer = 0b00,
    increment_4 = 0b01,
    increment_8 = 0b10,
    increment16 = 0b11,
};

pub const Channel = enum(u3) {
    ch0 = 0b000,
    ch1 = 0b001,
    ch2 = 0b010,
    ch3 = 0b011,
    ch4 = 0b100,
    ch5 = 0b101,
    ch6 = 0b110,
    ch7 = 0b111,
};

const SxCR = packed struct(u32) {
    enable: bool, // bit 0
    direct_mode_error_interrupt_enable: bool = false, // bit 1
    transfer_error_interrupt_enable: bool = false, // bit 2
    half_transfer_interrupt_enable: bool = false, // bit 3
    transfer_complete_interrupt_enable: bool, // bit 4

    peripheral_flow_controller: FlowController = FlowController.dma, // bit 5
    data_transfer_direction: Direction, // bit 6-7
    circular_mode_enable: bool, // loop mode basically, automatic reload

    peripheral_increment: bool,
    memory_increment: bool,
    peripheral_data_size: DataSize, // p_size
    memory_data_size: DataSize,
    peripheral_increment_offset_size: OffsetSize = OffsetSize.use_psize,

    priority: PriorityLevel,
    double_buffer_mode_enable: bool = false,
    current_target: Target = Target.m0ar,

    _reserved1: u1 = 0,

    peripheral_burst: BurstConfig = BurstConfig.single_transfer,
    memory_burst: BurstConfig = BurstConfig.single_transfer,

    channel_selection: Channel,
    _reserved2: u4 = 0,
};
comptime {
    macros.checkRegisterType32(SxCR);
}

comptime {
    // Interrupt enables
    std.debug.assert(@bitOffsetOf(SxCR, "enable") == 0);
    std.debug.assert(@bitOffsetOf(SxCR, "direct_mode_error_interrupt_enable") == 1);
    std.debug.assert(@bitOffsetOf(SxCR, "transfer_error_interrupt_enable") == 2);
    std.debug.assert(@bitOffsetOf(SxCR, "half_transfer_interrupt_enable") == 3);
    std.debug.assert(@bitOffsetOf(SxCR, "transfer_complete_interrupt_enable") == 4);

    // Config
    std.debug.assert(@bitOffsetOf(SxCR, "peripheral_flow_controller") == 5); // PFCTRL
    std.debug.assert(@bitOffsetOf(SxCR, "data_transfer_direction") == 6); // DIR[1:0]
    std.debug.assert(@bitOffsetOf(SxCR, "circular_mode_enable") == 8); // CIRC
    std.debug.assert(@bitOffsetOf(SxCR, "peripheral_increment") == 9); // PINC
    std.debug.assert(@bitOffsetOf(SxCR, "memory_increment") == 10); // MINC
    std.debug.assert(@bitOffsetOf(SxCR, "peripheral_data_size") == 11); // PSIZE[1:0]
    std.debug.assert(@bitOffsetOf(SxCR, "memory_data_size") == 13); // MSIZE[1:0]
    std.debug.assert(@bitOffsetOf(SxCR, "peripheral_increment_offset_size") == 15); // PINCOS
    std.debug.assert(@bitOffsetOf(SxCR, "priority") == 16); // PL[1:0]
    std.debug.assert(@bitOffsetOf(SxCR, "double_buffer_mode_enable") == 18); // DBM
    std.debug.assert(@bitOffsetOf(SxCR, "current_target") == 19); // CT

    // Burst
    std.debug.assert(@bitOffsetOf(SxCR, "peripheral_burst") == 21); // PBURST[1:0]
    std.debug.assert(@bitOffsetOf(SxCR, "memory_burst") == 23); // MBURST[1:0]

    // Channel + reserved
    std.debug.assert(@bitOffsetOf(SxCR, "channel_selection") == 25);

    // Sanity: total bit width
    std.debug.assert(@bitSizeOf(SxCR) == 32);
}

comptime {
    // Useless, but all in one place.
    // Maybe I'll use something like this. keeping for copy paste modify inspiration
    std.debug.assert(@bitSizeOf(Direction) == 2);
    std.debug.assert(@bitSizeOf(DataSize) == 2);
    std.debug.assert(@bitSizeOf(PriorityLevel) == 2);
    std.debug.assert(@bitSizeOf(BurstConfig) == 2);
    std.debug.assert(@bitSizeOf(Channel) == 3);
    std.debug.assert(@bitSizeOf(Target) == 1);
    std.debug.assert(@bitSizeOf(FlowController) == 1);
}

// =========
const SxNDTR = packed struct(u32) {
    number_of_data_items_to_transfer: u16,
    _reserved: u16 = 0,
    // This register is read only when the stream is disable. So disable the stream before starting a write. Then enable
};

comptime {
    macros.checkRegisterType32(SxNDTR);
}

// =========
const SxNPAR = packed struct(u32) {
    peripheral_address: u32,
    // Same can only be configured when Enable = '0' in CR
};

comptime {
    macros.checkRegisterType32(SxNPAR);
}

// =========
const SxM0AR = packed struct(u32) {
    memory_address: u32,
};

comptime {
    macros.checkRegisterType32(SxM0AR);
}
// =========
const SxM1AR = packed struct(u32) {
    memory_address: u32,
};

comptime {
    macros.checkRegisterType32(SxM1AR);
}
// =========
const SxFCR = packed struct(u32) {
    raw: u32 = 0,
};

comptime {
    macros.checkRegisterType32(SxFCR);
}

// =========
const Stream = extern struct {
    cr: SxCR,
    ndtr: SxNDTR,
    npar: SxNPAR,
    m0ar: SxM0AR,
    m1ar: SxM1AR,
    fcr: SxFCR,
};

// ===================================== The META Struct
// The meta struct CANNOT be packed struct, else some weird shit happen
// Normal
const DMA = extern struct {
    lisr: Lisr,
    hisr: Hisr,
    lifcr: Lifcr,
    hifcr: Hifcr,
    streams: [8]Stream,
};

const dma1: *volatile DMA = @ptrFromInt(DMA1_BASE);
const dma2: *volatile DMA = @ptrFromInt(DMA2_BASE);

const dmas = [_]*volatile DMA{ dma1, dma2 };
// An array of pointers

comptime {
    @setEvalBranchQuota(100_000);
    for (dmas) |dma_ptr| {
        //
        macros.checkRegComptime(&dma_ptr.lisr, @sizeOf(u32));
        macros.checkRegComptime(&dma_ptr.hisr, @sizeOf(u32));
        macros.checkRegComptime(&dma_ptr.lifcr, @sizeOf(u32));
        macros.checkRegComptime(&dma_ptr.hifcr, @sizeOf(u32));

        for (&dma_ptr.streams) |*stream| {
            // Here, if i did (dma_ptr.stream) |stream|, it would copy it.
            // I want the address so: Must take it as a ptr.

            // With a pointer to the stream, then it can access it properly
            macros.checkRegComptime(&stream.cr, @sizeOf(u32));
            macros.checkRegComptime(&stream.ndtr, @sizeOf(u32));
            macros.checkRegComptime(&stream.npar, @sizeOf(u32));
            macros.checkRegComptime(&stream.m0ar, @sizeOf(u32));
            macros.checkRegComptime(&stream.m1ar, @sizeOf(u32));
            macros.checkRegComptime(&stream.fcr, @sizeOf(u32));
        }
    }
}

comptime {
    // DMA common registers
    if (@offsetOf(DMA, "lisr") != 0x00)
        @compileError("Incorrect LISR offset");
    if (@offsetOf(DMA, "hisr") != 0x04)
        @compileError("Incorrect HISR offset");
    if (@offsetOf(DMA, "lifcr") != 0x08)
        @compileError("Incorrect LIFCR offset");
    if (@offsetOf(DMA, "hifcr") != 0x0C)
        @compileError("Incorrect HIFCR offset");
    if (@offsetOf(DMA, "streams") != 0x10)
        @compileError("Incorrect DMA streams offset");

    // Stream layout
    if (@offsetOf(Stream, "cr") != 0x00)
        @compileError("Incorrect CR offset");
    if (@offsetOf(Stream, "ndtr") != 0x04)
        @compileError("Incorrect NDTR offset");
    if (@offsetOf(Stream, "npar") != 0x08)
        @compileError("Incorrect PAR offset");
    if (@offsetOf(Stream, "m0ar") != 0x0C)
        @compileError("Incorrect M0AR offset");
    if (@offsetOf(Stream, "m1ar") != 0x10)
        @compileError("Incorrect M1AR offset");
    if (@offsetOf(Stream, "fcr") != 0x14)
        @compileError("Incorrect FCR offset");

    if (@sizeOf(Stream) != 0x18)
        @compileError("DMA Stream must be 24 bytes");
    if (@sizeOf(DMA) != 0xD0) // 0xCC + 0x4 = 0xD0. Check the offset after C, its next|0
        @compileError("DMA register block must be 208 bytes");
}

// ================================ The public config struct:
pub const StreamConfig = struct {
    data_transfer_direction: Direction,
    circular_mode_enable: bool,

    peripheral_increment: bool,
    memory_increment: bool,
    peripheral_data_size: DataSize,
    memory_data_size: DataSize,

    priority: PriorityLevel,
    channel: Channel,

    // Now we get into stuff thats not just SCR
    peripheral_address: *volatile anyopaque,
    memory_address_0: *anyopaque,
    memory_address_1: ?*anyopaque = null,
    count: u16,
};

pub const Controller = enum(u2) {
    dma1 = 0,
    dma2 = 1,
};

pub const StreamNumber = enum(u3) {
    s0 = 0,
    s1 = 1,
    s2 = 2,
    s3 = 3,
    s4 = 4,
    s5 = 5,
    s6 = 6,
    s7 = 7,
};

// ================================ The functions:
/// Configure and also start it
pub fn configure_and_start(comptime controller: Controller, comptime stream: StreamNumber, config: StreamConfig) void {
    const dma_ptr: *volatile DMA = dmas[@backingInt(controller)];
    const streamIdx: u3 = @backingInt(stream);
    const streamPtr = &dma_ptr.streams[streamIdx];

    streamPtr.cr.enable = false;
    while (streamPtr.cr.enable) {}

    streamPtr.m0ar = .{ .memory_address = @intFromPtr(config.memory_address_0) };
    streamPtr.m1ar = .{
        .memory_address = if (config.memory_address_1) |ptr|
            @intFromPtr(ptr)
        else
            0,
    };
    streamPtr.npar = .{ .peripheral_address = @intFromPtr(config.peripheral_address) };
    streamPtr.ndtr = .{ .number_of_data_items_to_transfer = config.count };

    const localConfigRegister: SxCR = .{
        .enable = true,
        .transfer_complete_interrupt_enable = true,

        .data_transfer_direction = config.data_transfer_direction,
        .circular_mode_enable = config.circular_mode_enable,
        .peripheral_increment = config.peripheral_increment,
        .memory_increment = config.memory_increment,

        .peripheral_data_size = config.peripheral_data_size,
        .memory_data_size = config.memory_data_size,

        .priority = config.priority,
        .channel_selection = config.channel,
    };
    streamPtr.cr = localConfigRegister;

    // Now the other stuff.
}

/// Once configured, enable it
pub fn enable(comptime controller: Controller, comptime stream: StreamNumber) void {
    const dma_ptr: *volatile DMA = dmas[@backingInt(controller)];
    const streamIdx: u3 = @backingInt(stream);

    dma_ptr.streams[streamIdx].cr.enable = true;
}

/// It request to disable it.
/// Not instant. After disable, poll isEnabled();
pub fn disable(comptime controller: Controller, comptime stream: StreamNumber) void {
    const dma_ptr: *volatile DMA = dmas[@backingInt(controller)];
    const streamIdx: u3 = @backingInt(stream);

    dma_ptr.streams[streamIdx].cr.enable = false;
}

/// After disable, poll enable
pub fn isEnabled(comptime controller: Controller, comptime stream: StreamNumber) bool {
    const dma_ptr = dmas[@backingInt(controller)];
    const stream_idx: u3 = @backingInt(stream);

    return dma_ptr.streams[stream_idx].cr.enable;
}

pub fn getFlags(comptime controller: Controller, comptime stream: StreamNumber) StreamFlags {
    const dma_ptr: *volatile DMA = dmas[@backingInt(controller)];
    const streamNumber: u3 = @backingInt(stream);
    const idx: u2 = @truncate(streamNumber);

    if (streamNumber < 4) {
        return dma_ptr.lisr.getFlags(idx);
    } else {
        return dma_ptr.hisr.getFlags(idx);
    }
    return .{};
}

pub inline fn clearFlags(comptime controller: Controller, comptime stream: StreamNumber, flags: StreamFlagsClear) void {
    const dma_ptr: *volatile DMA = dmas[@backingInt(controller)];
    const streamNumber: u3 = @backingInt(stream);
    const idx: u2 = @truncate(streamNumber);

    if (streamNumber < 4) {
        dma_ptr.lifcr = Lifcr.clearFlags(idx, flags);
    } else {
        dma_ptr.hifcr = Hifcr.clearFlags(idx, flags);
    }
}
// ========================= Test to be called, to check the assembly

noinline fn testGetFlags() bool {
    const flags = getFlags(.dma1, .s4);
    return flags.direct_mode_error_interrupt_flag;
}

noinline fn testClearFlags() void {
    clearFlags(.dma1, .s4, .{ .clear_fifo_error_interrupt_flag = true });
}

// ====================================== Helper functions
pub inline fn toClearFlags(flags: StreamFlags) StreamFlagsClear {
    return @bitCast(flags);
}

// ====================================== Example of ISR :

export fn DMA1_Stream5_ISR_Example() void {
    //
    const flags: StreamFlags = getFlags(.dma1, .s5);

    const slow_way: bool = false;
    if (comptime slow_way) {
        var cFlags: StreamFlagsClear = .{};

        if (flags.fifo_error_interrupt_flag) {
            cFlags.clear_fifo_error_interrupt_flag = true;
        }
        if (flags.direct_mode_error_interrupt_flag) {
            cFlags.clear_direct_mode_error_interrupt_flag = true;
        }
        if (flags.transfer_error_interrupt_flag) {
            cFlags.clear_transfer_error_interrupt_flag = true;
        }
        if (flags.half_transfer_interrupt_flag) {
            cFlags.clear_half_transfer_interrupt_flag = true;
        }
        if (flags.transfer_complete_interrupt_flag) {
            cFlags.clear_transfer_complete_interrupt_flag = true;
            // Transfer complete, actually do something with this.
        }
        clearFlags(.dma1, .s5, cFlags);
    } else {
        if (flags.transfer_complete_interrupt_flag) {
            // Transfer complete, actually do something with this.

            // For a write command using dma.
            // do some buffer cleaning.
            // The head and tail, modify it...

            // Don't need to disable it. Once it's complete, the enable flag will be cleared
            // disable(.dma1, .s5);

            // For a message queue, it would put it here.
            // There's a queue.
            // if the queue is not empty, the writter enqueue his request.
            // If it is empty, it config and enables it.
            // Now here, the request has finished.
            // If the queue is not empty, dequeue it. Configure it and then enable it.

            // The dma transfer complete will tell you when usart recieved the final byte in DR.
            // But usart will take some time to write it.
            // It can send an interrupt when the last bit of that byte (including parity [optional] and stop bits are written)
        }

        // This works because it has the same structure
        clearFlags(.dma1, .s5, toClearFlags(flags));
    }
}
