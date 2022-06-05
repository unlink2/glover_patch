use core::arch::asm;

// $12 == $status
pub type EnableIntFn = unsafe extern "C" fn(flags: usize) -> ();
pub type DisableIntFn = unsafe extern "C" fn() -> usize;

#[naked]
pub unsafe extern "C" fn disable_int() -> usize {
    asm!(
        r#"
        .set noat
        mfc0 $t0, $12
        addiu $at, $zero, 0xFFFE
        and $t1, $t0, $at
        mtc0 $t1, $12
        andi $v0, $t0, 0x1
        nop
        jr $ra
        nop
    "#,
        options(noreturn)
    );
}

#[naked]
pub unsafe extern "C" fn enable_int(flags: usize) {
    asm!(
        r#"
        .set noat
        mfc0 $t0, $12
        or $t0, $t0, $a0
        mtc0 $t0, $12
        nop
        nop
        jr $ra
        nop
    "#,
        options(noreturn)
    );
}
