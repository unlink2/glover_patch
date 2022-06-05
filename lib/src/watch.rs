use crate::render::{Drawable, RenderContext};
use core::{ffi::c_void, marker::PhantomData};

/**
 * This is a memory watch type
 * Note that some watch must be aligned to
 * function correctly!
 */
pub enum WatchType {
    Float32(*const f32),
    Int8(*const i8),
    Int16(*const i16),
    Int32(*const i32),
    Int64(*const i64),
    UInt8(*const u8),
    UInt16(*const u16),
    UInt32(*const u32),
    UInt64(*const u64),
}

/**
 * An address to watch
 */
pub struct WatchAddr<T>
where
    T: Copy,
{
    pub x: isize,
    pub y: isize,
    pub watch_type: WatchType,
    pub active: bool,
    pahntom: PhantomData<T>,
}

impl<T> WatchAddr<T>
where
    T: Copy,
{
    pub fn new(x: isize, y: isize, addr: WatchType) -> Self {
        Self {
            x,
            y,
            watch_type: addr,
            active: true,
            pahntom: PhantomData::default(),
        }
    }
}

impl<T> Drawable<T> for WatchAddr<T>
where
    T: Copy,
{
    fn draw(&mut self, ctxt: &mut dyn RenderContext) {
        // decide which type to read
        match self.watch_type {
            WatchType::Int8(ptr) => {}
            _ => todo!("Not implemented"),
        }
    }

    fn update(&mut self, data: T) {}
}

/**
 * A drawable watch object
 */
pub struct Watch<T>
where
    T: Copy,
{
    pub x: isize,
    pub y: isize,
    pub watch_list: [WatchAddr<T>; 8],
}
