const std = @import("std");
const builtin = @import("builtin");

const CommonError = error{
    Full,
    Empty,
};

pub fn RingQueue(comptime T: type, comptime capacity: usize) type {
    return struct {
        const Self = @This();

        buffer: [capacity]T = undefined,
        head_idx: usize = 0,
        tail_idx: usize = 0,
        count: usize = 0,

        // ! means error union
        ///
        pub fn enqueue(self: *Self, item: T) error{Full}!void {
            if (self.count == capacity) {
                return error.Full;
            }

            // Zig will perform a value copy
            // So, it should copy.
            self.buffer[self.tail_idx] = item;
            self.tail_idx = (self.tail_idx + 1) % capacity;
            self.count += 1;
        }

        /// Dangerous, this needs to start with a big element. Like, consecutive data
        /// And, then, it must have a len field
        /// It might be worth it to remove this one?
        pub fn enqueueSlice(self: *Self, item: []const u8, max_size: usize) error{ Full, TooLarge }!void {
            if (self.count == capacity) return error.Full;
            if (item.len > max_size) return error.TooLarge;

            const entry = &self.buffer[self.tail_idx];

            @memcpy(entry.msg[0..item.len], item);
            // Careful, hardwritten. Needs a len.
            // And it needs to start with a fixed slice
            entry.len = @intCast(item.len);

            self.tail_idx = (self.tail_idx + 1) % capacity;
            self.count += 1;
        }

        pub fn dequeue(self: *Self) ?T {
            if (self.count == 0) {
                return null;
            }

            const item = self.buffer[self.head_idx];
            self.head_idx = (self.head_idx + 1) % capacity;
            self.count -= 1;
            return item;
        }

        pub fn isEmpty(self: *const Self) bool {
            return self.count == 0;
        }

        pub fn isFull(self: *const Self) bool {
            return self.count == capacity;
        }
    };
}
