const std = @import("std");
const builtin = @import("builtin");

pub fn RingQueue(comptime T: type, comptime capacity: usize) type {
    return struct {
        const Self = @This();

        buffer: [capacity]T = undefined,
        head: usize = 0,
        tail: usize = 0,
        count: usize = 0,

        pub fn enqueue(self: *Self, item: T) error{Full}!void {
            if (self.count == capacity) {
                return error.Full;
            }

            self.buffer[self.tail] = item;
            self.tail = (self.tail + 1) % capacity;
            self.count += 1;
        }

        pub fn dequeue(self: *Self) ?T {
            if (self.count == 0) {
                return null;
            }

            const item = self.buffer[self.head];
            self.head = (self.head + 1) % capacity;
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
