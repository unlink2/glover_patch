extern crate ultrars;
use self::ultrars::math::*;

// size == F0
// LAND_ACTOR
#[repr(C)]
#[derive(Copy, Clone)]
pub struct Actor {
    pub next: *mut Actor,
    pub prev: *mut Actor,
    unknown_1: [u32; 7],
    visible: u32, // actor will render if nonzero. usually set to
    flags: u32, // some flags for actors, e.g. for glover the 0th byte == 05 is on ball
    ptrs: ActorPtrs,
    unknown_2: u32,
    position: Vector3<f32>,
    previous_position: Vector3<f32>,

    unknown_3: [u32; 3],
    velocity: Vector3<f32>,

    unknown_4: [u32; 6],
    unknown_f32: [f32; 3],
    angle: f32,
    unknown_5: [u32; 5],
    rotation: Vector3<f32>,
    unknown_8: [u32; 4],
    scale: Vector3<u32>,

    model_data: u32, // TODO make this a pointer to struct
    anim: u32, // TODO make this ptr PAct_Anim.t
    objbank: u32, // TODO make ptr, ObjBank.mp
    act_shad: u32, // TODO make ptr
    collision: u32, // TODO make ptr
    col: u32, // TODO ptr

    unknown_7: [u32; 2],
    unknown_ptr_1: *mut u32, // 40 size?
    unknown_ptr_2: *mut u32
}

#[repr(C)]
#[derive(Copy, Clone)]
union ActorPtrs {
    pub ball_attached: *mut Actor,
    pub unused: u32
}
