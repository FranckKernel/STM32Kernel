const std = @import("std");

extern fn usart2_write(ptr: [*]const u8, len: usize) i32;

// C import is depracated

pub fn write(bytes: []const u8) i32 {
    return usart2_write(bytes.ptr, bytes.len);
}

pub fn print(comptime fmt: []const u8, args: anytype) i32 {
    var buf: [512]u8 = undefined;
    const s = std.fmt.bufPrint(&buf, fmt, args) catch {
        const nbr_of_written_char: i32 = write(&buf); // formatting ran out of space: send what fit
        _ = write("...\r\n");
        return -nbr_of_written_char;
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
