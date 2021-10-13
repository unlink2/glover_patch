use super::ultrars::math::*;

#[repr(C)]
#[derive(Copy, Clone)]
pub struct Camera {
    buffer: [u8; 52],
    position: Point3<f32>,
    padding: [u8; 416],
}
