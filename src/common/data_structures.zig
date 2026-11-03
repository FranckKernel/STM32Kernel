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

            self.buffer[self.tail_idx] = item;
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
