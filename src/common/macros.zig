const std = @import("std");
const builtin = @import("builtin");

fn Pointee(comptime P: type) type {
    return @typeInfo(P).pointer.child;
}

inline fn checkAligned(addr: anytype, comptime bytes: usize) void {
    // Only Debug (swap the condition to include ReleaseSafe if you want it there)
    if (comptime builtin.mode == .Debug) {
        if (@intFromPtr(addr) % bytes != 0) {
            @panic("misaligned peripheral access");
        }
    }
}

inline fn checkReg(addr: anytype, comptime bytes: usize) void {
    comptime std.debug.assert(@sizeOf(Pointee(@TypeOf(addr))) == bytes);
    std.debug.assert(std.mem.isAligned(@intFromPtr(addr), bytes)); // same as addr % bytes == 0
}

/// READ32: one 32-bit volatile load, reinterpreted as the register's type.
pub inline fn read32(addr: anytype) Pointee(@TypeOf(addr)) {
    const T = Pointee(@TypeOf(addr));
    comptime std.debug.assert(@sizeOf(T) == 4);
    const raw: u32 = @as(*const volatile u32, @ptrCast(addr)).*;
    return @bitCast(raw);
}

pub inline fn read16(addr: anytype) Pointee(@TypeOf(addr)) {
    const T = Pointee(@TypeOf(addr));
    comptime std.debug.assert(@sizeOf(T) == 2);
    const raw: u16 = @as(*const volatile u16, @ptrCast(addr)).*;
    return @bitCast(raw);
}

pub inline fn read8(addr: anytype) Pointee(@TypeOf(addr)) {
    const T = Pointee(@TypeOf(addr));
    comptime std.debug.assert(@sizeOf(T) == 1);
    const raw: u8 = @as(*const volatile u8, @ptrCast(addr)).*;
    return @bitCast(raw);
}

/// WRITE32: the value's type must match the register's type (enforced by the signature).
pub inline fn write32(addr: anytype, value: Pointee(@TypeOf(addr))) void {
    comptime std.debug.assert(@sizeOf(@TypeOf(value)) == 4);
    @as(*volatile u32, @ptrCast(addr)).* = @bitCast(value);
}

pub inline fn write16(addr: anytype, value: Pointee(@TypeOf(addr))) void {
    comptime std.debug.assert(@sizeOf(@TypeOf(value)) == 2);
    @as(*volatile u16, @ptrCast(addr)).* = @bitCast(value);
}

pub inline fn write8(addr: anytype, value: Pointee(@TypeOf(addr))) void {
    comptime std.debug.assert(@sizeOf(@TypeOf(value)) == 1);
    @as(*volatile u8, @ptrCast(addr)).* = @bitCast(value);
}

/// FORCE_WRITE32: no type matching, any 32-bit value.
pub inline fn forceWrite32(addr: anytype, value: u32) void {
    comptime std.debug.assert(@sizeOf(@TypeOf(value)) == 4);
    @as(*volatile u32, @ptrCast(addr)).* = @bitCast(value);
}

pub inline fn forceWrite16(addr: anytype, value: u16) void {
    comptime std.debug.assert(@sizeOf(@TypeOf(value)) == 2);
    @as(*volatile u16, @ptrCast(addr)).* = @bitCast(value);
}

pub inline fn forceWrite8(addr: anytype, value: u8) void {
    comptime std.debug.assert(@sizeOf(@TypeOf(value)) == 1);
    @as(*volatile u8, @ptrCast(addr)).* = @bitCast(value);
}
