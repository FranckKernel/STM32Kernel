const std = @import("std");
const usart = @import("usart");

const common_variables = @import("common_variables");

var activation_flag: u32 = 0;
pub inline fn activateModuleStdio() void {
    const ptr: *volatile u32 = &activation_flag;
    ptr.* = 1;
}
// ===============

// extern fn usart2_write(ptr: [*]const u8, len: usize) i32;

// C import is depracated

// Change write so it use an error, not a negative int.
// This is Zig, not C
fn write(
    bytes: []const u8,
    comptime hardware_writer: *const fn ([]const u8) usize,
) i32 {
    const bytes_written: usize = hardware_writer(bytes);
    // const bytes_written: i32 = usart2_write(bytes.ptr, bytes.len);

    if (bytes_written != bytes.len) {
        return -@as(i32, @intCast(bytes_written));
    }

    return @intCast(bytes_written);
}

const print_message_capacity = common_variables.print_message_capacity;

fn print(
    comptime fmt: []const u8,
    args: anytype,
    comptime writer: *const fn ([]const u8) i32,
) i32 {
    var buf: [print_message_capacity]u8 = undefined;
    const s = std.fmt.bufPrint(&buf, fmt, args) catch {
        const nbr_of_written_char: i32 = writer(&buf); // formatting ran out of space: send what fit
        _ = writer("...\r\n");
        return -@abs(nbr_of_written_char);
    };
    return writer(s);
}

fn vprint(
    comptime fmt: []const u8,
    args: anytype,
    comptime writer: *const fn ([]const u8) i32,
) void {
    var buf: [print_message_capacity]u8 = undefined;
    const s = std.fmt.bufPrint(&buf, fmt, args) catch {
        _ = writer(&buf); // formatting ran out of space: send what fit
        _ = writer("...\r\n");
        return;
    };

    _ = writer(s);
    return;
}

pub fn writep(bytes: []const u8) i32 {
    return write(bytes, usart.usart2_write);
}

pub fn printp(
    comptime fmt: []const u8,
    args: anytype,
) i32 {
    return print(fmt, args, writep);
}

pub fn vprintp(
    comptime fmt: []const u8,
    args: anytype,
) void {
    return vprint(fmt, args, writep);
}

// ==================== Background with STDIO ================

pub fn writeb(bytes: []const u8) i32 {
    return write(bytes, usart.usart2_write_background);
}

pub fn printb(comptime fmt: []const u8, args: anytype) i32 {
    return print(fmt, args, writeb);
}

pub noinline fn vprintb(comptime fmt: []const u8, args: anytype) void {
    vprint(fmt, args, writeb);
}

// ================================ DMA version ===============

pub fn write_dma(bytes: []const u8) i32 {
    return write(bytes, usart.usart2_write_dma);
}

pub fn printd(comptime fmt: []const u8, args: anytype) i32 {
    return print(fmt, args, write_dma);
}

pub noinline fn vprintd(comptime fmt: []const u8, args: anytype) void {
    vprint(fmt, args, write_dma);
}

// ====================== Stdio writter.

fn uartDrain(
    w: *std.Io.Writer,
    data: []const []const u8, // An array of strings.
    splat: usize, // Splat is an optimisation thing.
) std.Io.Writer.Error!usize {

    // First transmit the internal buffer.
    writeb(w.buffered()) catch {
        return error.WriteFailed;
    };

    w.end = 0;

    for (data[0 .. data.len - 1]) |slice| {
        // A good idea would be to check the slice length.
        // And merge slices
        if (slice.len > print_message_capacity) {
            return error.WriteFailed;
        }
    }

    // Then transmit additional slices.

    var current_left_index: usize = 0;
    var current_right_index: usize = 1;
    while (true) {
        //

        const left_slice = data[current_left_index];
        var current_merge_size: usize = left_slice.len;
        while (true) {
            if (current_right_index >= data.len) {
                break;
            }
            const right_slice = data[current_right_index];

            if (current_merge_size + right_slice.len > print_message_capacity) {
                break;
            } else {
                current_merge_size += right_slice.len;
                current_right_index += 1;
            }
        }
        // out of innter loop

        // In both case, right_index is last + 1 of merge
        // [100, 75, 33, 50]
        //  L^           R^
        // We have a merge of L,R=0,3
        // Length = R - L = 3.
        // Last index in merge is 2.

        var print_msg: [print_message_capacity]u8 = @splat(0);
        var offset: usize = 0;
        for (current_left_index..current_right_index) |i| {
            const slice = data[i];
            @memcpy(print_msg[offset..][0..slice.len], slice);
            offset += slice.len;
        }

        if (current_right_index == data.len) {
            // right was one past last.
            break;
        }

        writeb(print_msg) catch {
            return error.WriteFailed;
        };

        // For the new itteration
        current_left_index = current_right_index;
        current_right_index = current_right_index + 1;
    }

    var written: usize = 0;

    // The last slice is repeated 'splat' times.
    const last = data[data.len - 1];
    if (last.len == 0) {
        return;
    }

    const max_splat = print_message_capacity / last.len;

    // a*q + r
    const full_batches = splat / max_splat;
    const remainder = splat % max_splat;

    var print_msg: [print_message_capacity]u8 = undefined;

    // Full batches
    for (0..full_batches) |_| {
        for (0..max_splat) |i| {
            const offset = i * last.len;
            @memcpy(print_msg[offset..][0..last.len], last);
        }

        try writeb(print_msg[0 .. max_splat * last.len]);
        written += max_splat * last.len;
    }

    // Remaining repetitions
    if (remainder != 0) {
        const remaining_len = remainder * last.len;

        if (full_batches == 0) {
            // An optimisation, if full_batches was >0, then print_msg was already populated
            for (0..remainder) |i| {
                const offset = i * last.len;
                @memcpy(print_msg[offset..][0..last.len], last);
            }
        }

        try writeb(print_msg[0..remaining_len]);
        written += remaining_len;
    }

    return written;
}

fn test_io_writter() void {

    // STDIO Writter:
    var writer: std.Io.Writer = .{
        .buffer = &.{},
        .vtable = &.{
            .drain = uartDrain,
        },
    };

    try writer.print("123456  12hex={x}", .{12});
}

// =============================== Usage example
pub const Bin32 = struct {
    v: u32,

    pub fn format(self: Bin32, w: *std.Io.Writer) std.Io.Writer.Error!void {
        var i: u32 = 0;
        while (i < 32) : (i += 1) {
            const bit: u8 = @intCast((self.v >> @intCast(31 - i)) & 1);
            try w.writeByte('0' + bit);
            if (i % 4 == 3 and i != 31) try w.writeByte(' ');
        }
    }
};

pub fn to_bin32(v: u32) Bin32 {
    return .{ .v = v };
}

extern fn wait_seconds(seconds: f32) void;

pub noinline fn print_example() void {
    const sr: u32 = (0x4000_4400);
    vprintb("How are you, 27 = {}\r\n", .{27});
    vprintb("dec={d} hex={x} padded={x:0>8} char={c} str={s}\r\n\n", .{ 27, 27, 27, 'A', "hi" });
    vprintb("temp = {d:.2}\r\n", .{@as(f32, 3.14159)});
    vprintb("byte(sr= 0x4000_4400) = 0b{b:0>32}\r\n", .{sr}); // padded to 32 digits with zeros
    vprintb("byte(0xA5) = {b:0>8}\r\n", .{@as(u8, 0xA5)}); // 10100101
    vprintb("Octal of 8: {o}\r\n", .{@as(u32, 8)}); // octal

    // usage
    // wait_seconds(10);
    // vprintb("sr = {f}\r\n", .{Bin32{ .v = 0x4000_4400 }});
    vprintb("sr2 = {f}\r\n", .{to_bin32(0x4000_4400)});

    wait_seconds(0.5); // Need a wait. Otherwise fucked
    vprintb("End of the tests/example\n", .{});
}
