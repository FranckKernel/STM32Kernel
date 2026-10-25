// build.zig
const std = @import("std");

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
    //

    // ======================= The The full thing

    const entire_kernel_module = b.createModule(.{
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
        .root_module = entire_kernel_module,
        .linkage = .static,
    });

    // Add the modules
    entire_kernel_module.addImport("gpio", gpio_module);
    // entire_kernel_module.addImport("debug", debug_module);
    // debug_module.addImport("stdio", stdio_module);
    // stdio_module.addImport("intrinsics", intrinsics_modules);
    // stdio_module.addImport("string", string_module);

    // kernel_lib.root_module.red_zone = false;
    kernel_lib.root_module.link_libc = false;
    kernel_lib.root_module.link_libcpp = false;
    // kernel_lib.root_module.sanitize_c = .off; //

    kernel_lib.root_module.unwind_tables = .none;
    // kernel_lib.omit_frame_pointer = false; // on the module (the commented line in your build.zig)
    // kernel_lib.link_eh_frame_hdr = false;
    // kernel_lib.root_module.

    kernel_lib.bundle_compiler_rt = false;

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

    // Set output directory to match your bash script
    const build_dir_path_str = "../../build64";
    const build_dir = b.path(build_dir_path_str);
    _ = build_dir;
    // kernel_lib.setOutputDir(b.pathFromRoot(build_dir_path_str));

    // Install the library to the build directory
    b.installArtifact(kernel_lib);
}
