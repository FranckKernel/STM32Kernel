const std = @import("std");
const builtin = @import("builtin");
const std_options = @import("std_options"); // My own thing
const usart = @import("usart");

const macros = @import("macros");
const gpio = @import("gpio");
const stdio = @import("stdio");

const print = stdio.print;
const vprint = stdio.vprint;

pub const panic = std.debug.FullPanic(std_options.kernel_panic_handler);

const input_pin: u32 = 12;
// const input_letter :

export fn zig_main() noreturn {
    // setup

    vprint("\n\n===Welcome to zig ====\n\n", .{});

    if (true) {
        // @panic("Panic works\n");
    }
    // _ = macros.forceWrite32(usart.usart2.data, 0);
    // _ = usart.write_u8(usart.usart2, 0);
    while (true) {
        // main loop
        // _ = stdio.write("How are you?");
    }
}
