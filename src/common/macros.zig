const std = @import("std");
const builtin = @import("builtin");

var activation_flag: u32 = 0;
pub inline fn activateModuleMacros() void {
    const ptr: *volatile u32 = &activation_flag;
    ptr.* = 1;
}
// =========================== The actual macros

fn Pointee(comptime P: type) type {
    return @typeInfo(P).pointer.child;
}

inline fn checkAlignedRuntime(addr: anytype, comptime bytes: usize) void {
    // Only Debug (swap the condition to include ReleaseSafe if you want it there)
    if (comptime builtin.mode == .Debug) {
        if (@intFromPtr(addr) % bytes != 0) {
            @panic("misaligned peripheral access");
        }
    }
}

const runtimeAlignementCheckDuringDebug: bool = false;

pub inline fn checkRegComptime(comptime addr: anytype, comptime bytes: usize) void {
    const T = Pointee(@TypeOf(addr));
    comptime std.debug.assert(@sizeOf(T) == bytes);

    comptime std.debug.assert(@alignOf(T) == bytes);
    comptime std.debug.assert(std.mem.isAligned(@intFromPtr(addr), bytes)); // same as addr % bytes == 0
}

inline fn checkRegPartial(addr: anytype, comptime bytes: usize) void {
    const T = Pointee(@TypeOf(addr));
    comptime std.debug.assert(@sizeOf(T) == bytes);

    comptime std.debug.assert(@alignOf(T) == bytes);
}

inline fn checkReg(addr: anytype, comptime bytes: usize) void {
    checkRegPartial(addr, bytes);

    if (comptime runtimeAlignementCheckDuringDebug) {
        checkAlignedRuntime(addr, bytes);
    }
}

/// READ32: one 32-bit volatile load, reinterpreted as the register's type.
pub inline fn read32(addr: anytype) Pointee(@TypeOf(addr)) {
    checkReg(addr, @sizeOf(u32));

    const raw: u32 = @as(*const volatile u32, @ptrCast(addr)).*;
    return @bitCast(raw);
}

pub inline fn read16(addr: anytype) Pointee(@TypeOf(addr)) {
    checkReg(addr, @sizeOf(u16));

    const raw: u16 = @as(*const volatile u16, @ptrCast(addr)).*;
    return @bitCast(raw);
}

pub inline fn read8(addr: anytype) Pointee(@TypeOf(addr)) {
    checkReg(addr, @sizeOf(u8));

    const raw: u8 = @as(*const volatile u8, @ptrCast(addr)).*;
    return @bitCast(raw);
}

/// WRITE32: the value's type must match the register's type (enforced by the signature).
pub inline fn write32(addr: anytype, value: Pointee(@TypeOf(addr))) void {
    checkReg(addr, @sizeOf(u32));
    comptime std.debug.assert(@sizeOf(@TypeOf(value)) == 4);

    @as(*volatile u32, @ptrCast(addr)).* = @bitCast(value);
}

pub inline fn write16(addr: anytype, value: Pointee(@TypeOf(addr))) void {
    checkReg(addr, @sizeOf(u16));
    comptime std.debug.assert(@sizeOf(@TypeOf(value)) == 2);

    @as(*volatile u16, @ptrCast(addr)).* = @bitCast(value);
}

pub inline fn write8(addr: anytype, value: Pointee(@TypeOf(addr))) void {
    checkReg(addr, @sizeOf(u8));
    comptime std.debug.assert(@sizeOf(@TypeOf(value)) == 1);

    @as(*volatile u8, @ptrCast(addr)).* = @bitCast(value);
}

/// FORCE_WRITE32: no type matching, any 32-bit value.
pub inline fn forceWrite32(addr: anytype, value: u32) void {
    checkReg(addr, @sizeOf(u32));
    comptime std.debug.assert(@sizeOf(@TypeOf(value)) == 4);

    @as(*volatile u32, @ptrCast(addr)).* = @bitCast(value);
}

pub inline fn forceWrite16(addr: anytype, value: u16) void {
    checkReg(addr, @sizeOf(u16));
    comptime std.debug.assert(@sizeOf(@TypeOf(value)) == 2);

    @as(*volatile u16, @ptrCast(addr)).* = @bitCast(value);
}

pub inline fn forceWrite8(addr: anytype, value: u8) void {
    checkReg(addr, @sizeOf(u8));
    comptime std.debug.assert(@sizeOf(@TypeOf(value)) == 1);

    @as(*volatile u8, @ptrCast(addr)).* = @bitCast(value);
}

// ================================ Comptime checks

pub fn checkRegisterType32(comptime T: type) void {
    if (@bitSizeOf(T) != 32)
        @compileError(@typeName(T) ++ " must be 32 bits");

    if (@alignOf(T) != @alignOf(u32))
        @compileError(@typeName(T) ++ " must be 32-bit aligned");
}
