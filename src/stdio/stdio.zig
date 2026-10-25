const std = @import("std");
const usart = @import("usart");

// extern fn usart2_write(ptr: [*]const u8, len: usize) i32;

// C import is depracated

pub fn write(bytes: []const u8) i32 {
    const bytes_written: usize = usart.usart2_write(bytes);
    // const bytes_written: i32 = usart2_write(bytes.ptr, bytes.len);

    if (bytes_written != bytes.len) {
        return -@as(i32, @intCast(bytes_written));
    }

    return @intCast(bytes_written);
}

pub fn print(comptime fmt: []const u8, args: anytype) i32 {
    var buf: [512]u8 = undefined;
    const s = std.fmt.bufPrint(&buf, fmt, args) catch {
        const nbr_of_written_char: i32 = write(&buf); // formatting ran out of space: send what fit
        _ = write("...\r\n");
        return -@abs(nbr_of_written_char);
    };
    return write(s);
}

pub fn vprint(comptime fmt: []const u8, args: anytype) void {
    var buf: [512]u8 = undefined;
    const s = std.fmt.bufPrint(&buf, fmt, args) catch {
        _ = write(&buf); // formatting ran out of space: send what fit
        _ = write("...\r\n");
        return;
    };

    _ = write(s);
    return;
}

pub fn write_background(bytes: []const u8) i32 {
    const bytes_written: usize = usart.usart2_write_background(bytes);
    // const bytes_written: i32 = usart2_write(bytes.ptr, bytes.len);

    if (bytes_written != bytes.len) {
        return -@as(i32, @intCast(bytes_written));
    }

    return @intCast(bytes_written);
}

pub fn printb(comptime fmt: []const u8, args: anytype) i32 {
    var buf: [512]u8 = undefined;
    const s = std.fmt.bufPrint(&buf, fmt, args) catch {
        const nbr_of_written_char: i32 = write_background(&buf); // formatting ran out of space: send what fit
        _ = write_background("...\r\n");
        return -@abs(nbr_of_written_char);
    };
    return write_background(s);
}

pub noinline fn vprintb(comptime fmt: []const u8, args: anytype) void {
    var buf: [512]u8 = undefined;
    const s = std.fmt.bufPrint(&buf, fmt, args) catch {
        _ = write_background(&buf); // formatting ran out of space: send what fit
        _ = write_background("...\r\n");
        return;
    };

    _ = write_background(s);
    return;
}

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
