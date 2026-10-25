// build.zig
const std = @import("std");
const builtin = @import("builtin");

inline fn checkAligned(addr: anytype, comptime bytes: usize) void {
    // Only Debug (swap the condition to include ReleaseSafe if you want it there)
    if (comptime builtin.mode == .Debug) {
        if (@intFromPtr(addr) % bytes != 0) {
            @panic("misaligned peripheral access");
        }
    }
}

pub fn build(b: *std.Build) void {
    const target = b.standardTargetOptions(.{
        .default_target = .{
            .cpu_arch = .thumb,
            .os_tag = .freestanding,
            .abi = .eabihf,
            .cpu_model = .{ .explicit = &std.Target.arm.cpu.cortex_m4 },
        },
    });

    const optimize = b.standardOptimizeOption(.{
        .preferred_optimize_mode = .ReleaseSafe,
    });

    // ======================= Specific modules

    const gpio_module = b.createModule(.{
        .root_source_file = b.path("gpio/gpio.zig"),
        .target = target,
        .optimize = optimize,
    });

    const usart_module = b.createModule(.{
        .root_source_file = b.path("communication/usart/usart.zig"),
        .target = target,
        .optimize = optimize,
    });

    const macros_module = b.createModule(.{
        .root_source_file = b.path("common/macros.zig"),
        .target = target,
        .optimize = optimize,
    });

    const std_options_module = b.createModule(.{
        .root_source_file = b.path("kernel/std_options.zig"),
        .target = target,
        .optimize = optimize,
    });
    const stdio_module = b.createModule(.{
        .root_source_file = b.path("stdio/stdio.zig"),
        .target = target,
        .optimize = optimize,
    });
    //

    // ======================= The The full thing

    const main_module = b.createModule(.{
        .root_source_file = b.path("kernel/usermain.zig"), // Source file
        .target = target,
        .optimize = optimize,
        // .omit_frame_pointer = false,
    });

    // might be a cool feature (error tracing)
    // entire_kernel_module.error_tracing = true;

    // Create the kernel library (static archive)
    const kernel_lib = b.addLibrary(.{
        .name = "kernel",
        .root_module = main_module,
        .linkage = .static,
    });

    // Add the modules
    main_module.addImport("gpio", gpio_module);
    main_module.addImport("stdio", stdio_module);
    main_module.addImport("std_options", std_options_module);
    main_module.addImport("macros", macros_module);
    main_module.addImport("usart", usart_module);
    std_options_module.addImport("stdio", stdio_module);
    usart_module.addImport("macros", macros_module);
    stdio_module.addImport("usart", usart_module);
    gpio_module.addImport("macros", macros_module);

    // kernel_lib.root_module.red_zone = false;
    kernel_lib.root_module.link_libc = false;
    kernel_lib.root_module.link_libcpp = false;
    // kernel_lib.root_module.sanitize_c = .off; //

    kernel_lib.root_module.unwind_tables = .none;
    // kernel_lib.omit_frame_pointer = false; // on the module (the commented line in your build.zig)
    // kernel_lib.link_eh_frame_hdr = false;
    // kernel_lib.root_module.

    kernel_lib.bundle_compiler_rt = true;

    // Add include directories (For C/Cpp headers only)
    // kernel_lib.root_module.addIncludePath(b.path("../ProtectedMode/LongModePrep/"));

    // Add single C file (Not needed)
    const add_c_file: bool = false;
    if (add_c_file) {
        kernel_lib.root_module.addCSourceFile(.{
            .file = b.path("kernel64/com1.c"),
            .flags = &.{
                "-std=gnu23",
                "-ffreestanding",
                "-mcmodel=kernel",
                "-mno-red-zone",
                "-fno-stack-protector",
            },
        });
    }

    // Optional: Add assembly files

    // ======================= Create a test runner step

    // b.addSystemCommand();

    // ======================= Test steps for the user
    const check = b.step("check", "Compile for the language server");
    check.dependOn(&kernel_lib.step);

    // Set output directory to match your bash script
    const build_dir_path_str = "../../build64";
    const build_dir = b.path(build_dir_path_str);
    _ = build_dir;
    // kernel_lib.setOutputDir(b.pathFromRoot(build_dir_path_str));

    // Install the library to the build directory
    b.installArtifact(kernel_lib);
}
