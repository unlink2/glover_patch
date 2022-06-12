#[derive(Debug, Eq, PartialEq)]
pub enum Error {
    MemoryFilledInterupt,
    InvalidChecksum,
    AddressOutOfRange,
    LengthMismatch,
}
