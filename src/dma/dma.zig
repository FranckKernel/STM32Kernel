const std = @import("std");
const builtin = @import("builtin");

const macros = @import("macros");
const stdio = @import("stdio");

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
    // Same thing, I want the lsp to go cross file and access stuff from macros
}

const DMA_BASE: u32 = 0x4002_6000; // The main one connected to my machine

const DMA1_BASE: u32 = DMA_BASE;
const DMA2_BASE: u32 = DMA_BASE + 0x400;

// ===================================== The DMA common registers
const Lisr = packed struct(u32) {
    raw: u32,
};

comptime {
    macros.checkRegisterType32(Lisr);
}

// =========
const Hisr = packed struct(u32) {
    raw: u32,
};

comptime {
    macros.checkRegisterType32(Hisr);
}

// =========
const Lifcr = packed struct(u32) {
    raw: u32,
};

comptime {
    macros.checkRegisterType32(Lifcr);
}

// =========
const Hifcr = packed struct(u32) {
    raw: u32,
};
comptime {
    macros.checkRegisterType32(Hifcr);
}

// ===================================== The DMA Stream Registers
const SxCR = packed struct(u32) {
    raw: u32,
};
comptime {
    macros.checkRegisterType32(SxCR);
}

// =========
const SxNDTR = packed struct(u32) {
    raw: u32,
};

comptime {
    macros.checkRegisterType32(SxNDTR);
}

// =========
const SxNPAR = packed struct(u32) {
    raw: u32,
};

comptime {
    macros.checkRegisterType32(SxNPAR);
}

// =========
const SxM0AR = packed struct(u32) {
    raw: u32,
};

comptime {
    macros.checkRegisterType32(SxM0AR);
}
// =========
const SxM1AR = packed struct(u32) {
    raw: u32,
};

comptime {
    macros.checkRegisterType32(SxM1AR);
}
// =========
const SxFCR = packed struct(u32) {
    raw: u32,
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
pub const DMA = struct {
    lisr: Lisr,
    hisr: Hisr,
    lifcr: Lifcr,
    hifcr: Hifcr,
    streams: [8]Stream,
};

pub const dma1: *volatile DMA = @ptrFromInt(DMA1_BASE);
pub const dma2: *volatile DMA = @ptrFromInt(DMA2_BASE);

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

// ================================ The functions:
