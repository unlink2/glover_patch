use core::mem::{self, size_of};

use crate::memory::umemset;

/**
 * This module
 * is a helper for cloning ram contents
 * from one location to another in a fixed data strucute
 * This is very much an unsafe module using a raw pointer
 * as a destination!
 * TODO use alocated once it is implemented!
 */
use super::memory::umemcpy;

pub struct CloneHeader {
    original: *mut u8,
    size: usize,
    clone: *mut u8,
}

pub struct CloneContext {
    cloned: *mut u8,
    cloned_offset: usize,
    headers: *mut CloneHeader,
    headers_offset: usize,
}

impl CloneContext {
    pub fn new(cloned: *mut u8, headers: *mut CloneHeader) -> Self {
        Self {
            cloned,
            headers,
            cloned_offset: 0,
            headers_offset: 0,
        }
    }

    pub fn destination(&self) -> *mut u8 {
        unsafe { self.cloned.add(self.cloned_offset) }
    }

    pub fn header(&self) -> *mut CloneHeader {
        unsafe { self.headers.add(self.headers_offset) }
    }

    pub fn reset(&mut self) {
        self.headers_offset = 0;
        self.cloned_offset = 0;
    }

    pub fn clone<T>(&mut self, src: *mut T) {
        unsafe {
            let size = mem::size_of::<T>();
            // store a header
            let header = self.header();
            (*header).original = src as *mut u8;
            (*header).size = size;
            (*header).clone = self.destination();
            self.headers_offset += 1;

            // then copy the raw bytes
            umemcpy(self.destination(), src as *mut u8, size);
            self.cloned_offset += size;
        }
    }

    pub fn restore(&mut self, i: usize) {
        unsafe {
            let header = self.headers.add(i);

            umemcpy((*header).original, (*header).clone, (*header).size);
        }
    }

    pub fn restore_all(&mut self) {
        for i in 0..self.headers_offset {
            self.restore(i);
        }
    }
}
