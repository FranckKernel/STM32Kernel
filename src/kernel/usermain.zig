const std = @import("std");
const builtin = @import("builtin");
const std_options = @import("std_options"); // My own thing
const usart = @import("usart");

const macros = @import("macros");
const gpio = @import("gpio");
const stdio = @import("stdio");
const dma = @import("dma");

const print = stdio.print;
const vprint = stdio.vprint;
const vprintb = stdio.vprintb;

pub const panic = std.debug.FullPanic(std_options.kernel_panic_handler);

const input_pin: u32 = 12;
// const input_letter :

extern fn wait_seconds(seconds: f32) void;

export fn zig_main() noreturn {
    // setup

    vprint("\n\n===Welcome to zig ====\n\n", .{});

    _ = usart.usart2_write("Writting in the foreground!\n");
    _ = usart.usart2_write_background("Writting in the background!\n");
    dma.activateModuleDMA();

    stdio.print_example();

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
