/**
 * Describes all buttons on the n64 controller
 */
#[derive(Clone, Copy)]
pub enum Button {
    RInput = 0x14,
    LInput = 0x15,
    ZInput = 0x1D,
    CLInput = 0x11,
    CRInput = 0x10,
    CDInput = 0x12,
    CUInput = 0x13,
    AInput = 0xFF,
    BInput = 0x1E,
    StartInput = 0x1C,
    DpadUp = 0x1B,
    DpadDown = 0x1A,
    DpadRight = 0x18,
    DpadLeft = 0x19,
}

/**
 * Describes the x and y axis
 */
pub enum Axis {
    XAxis = 0x01,
    YAxis = 0x00,
}

/// Registers for controller reading
pub const CONTROLLER1: *mut u32 = 0xBFC007C4 as *mut u32;
pub const CONTROLLER2: *mut u32 = 0xBFC007CC as *mut u32;

/**
 * Handles an input
 */
pub struct InputHandler {
    controller: *mut u32,
    current: u32,
    last: u32,
}

impl InputHandler {
    pub fn new(controller: *mut u32) -> Self {
        Self {
            controller,
            last: 0,
            current: 0,
        }
    }

    /**
     * Call once a frame to poll last controller
     * status
     */
    pub unsafe fn update(&mut self) {
        self.last = self.current;
        self.current = *self.controller;
    }

    /**
     * Call before update to poll status of single button
     * if just is true return only if last is false
     */
    pub fn read_button(&self, button: Button, just: bool) -> bool {
        // if controller is not present it will return this value
        if self.current == 0xFFFFFFFF {
            return false;
        }
        return self.is_pressed(button, self.current)
            && (!just || !self.is_pressed(button, self.last));
    }

    pub fn is_pressed(&self, button: Button, readout: u32) -> bool {
        return ((readout >> (button as u32)) & 0x01) == 1;
    }

    pub fn read_stick(&self, axis: Axis) -> u8 {
        return ((self.current) >> (axis as u8 * 8)) as u8;
    }
}
