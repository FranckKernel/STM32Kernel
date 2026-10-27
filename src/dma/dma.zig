const std = @import("std");
const builtin = @import("builtin");

const macros = @import("macros");
const stdio = @import("stdio");

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

const StreamFlags = packed struct(u6) {
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
const StreamFlagsClear = packed struct(u6) {
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
const SxCR = packed struct(u32) {
    enable: bool,
    raw_rest: u31,
};
comptime {
    macros.checkRegisterType32(SxCR);
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
const StreamConfig = struct {

    //
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
pub fn configure(comptime controller: Controller, comptime stream: StreamNumber, config: StreamConfig) void {
    _ = controller;
    _ = stream;
    _ = config;
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
inline fn toClearFlags(flags: StreamFlags) StreamFlagsClear {
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
