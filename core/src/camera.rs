use super::ultrars::math::*;

#[repr(C)]
#[derive(Copy, Clone)]
pub struct Camera {
    buffer: [u8; 52],
    position: Vector3<f32>,
    padding: [u8; 416]
}
