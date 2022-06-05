pub fn min<T>(v1: T, v2: T) -> T
where
    T: Ord,
{
    return if v1 < v2 { v1 } else { v2 };
}

#[repr(C)]
#[derive(Copy, Clone)]
pub struct Vector3<T> {
    pub x: T,
    pub y: T,
    pub z: T,
}

impl<T> Vector3<T> {
    pub fn new(x: T, y: T, z: T) -> Self {
        return Self { x, y, z };
    }
}

#[repr(C)]
#[derive(Copy, Clone)]
pub struct Point3<T> {
    pub x: T,
    pub y: T,
    pub z: T,
}

impl<T> Point3<T> {
    pub fn new(x: T, y: T, z: T) -> Self {
        return Self { x, y, z };
    }
}
