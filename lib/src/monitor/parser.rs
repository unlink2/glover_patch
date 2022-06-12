use crate::error::Error;

use super::stream::Stream;

pub fn is_digit(b: u8) -> bool {
    (b'0'..=b'9').contains(&b)
}

pub fn is_hex(b: u8) -> bool {
    (b'a'..=b'f').contains(&b) || (b'A'..=b'F').contains(&b)
}

pub fn is_hex_digit(b: u8) -> bool {
    is_hex(b) || is_digit(b)
}

pub fn from_hex(b: u8) -> Option<u8> {
    if (b'0'..=b'9').contains(&b) {
        Some(b - b'0')
    } else if (b'A'..=b'F').contains(&b) {
        Some(b - b'A' + 10)
    } else if (b'a'..=b'f').contains(&b) {
        Some(b - b'a' + 10)
    } else {
        None
    }
}

/// non-size-bounded hex conversion
/// for when the size does not need to fit a particular
/// bound
pub fn from_hexu(b: &[u8]) -> Option<usize> {
    // filter for the first \0 if it exists
    let end_index = b.iter().position(|&v| v == 0).unwrap_or(b.len());
    let b = &b[0..end_index];

    let mut result = 0;
    let shift_len = (b.len() - 1) * 4;
    for (i, byte) in b.iter().enumerate() {
        let val = from_hex(*byte)? as usize;

        result |= val << (shift_len - 4 * i);
    }
    Some(result)
}

pub fn to_hex(b: u8) -> Option<u8> {
    if b >= 16 {
        None
    } else if b <= 9 {
        Some(b + b'0')
    } else {
        Some(b + b'a' - 10)
    }
}

pub fn to_hex_tuple(b: u8) -> (u8, u8) {
    let h = (b >> 4) & 0xF;
    let l = b & 0xF;
    // we can unwrap here because it will always be valid
    (to_hex(h).unwrap(), to_hex(l).unwrap())
}

// converts a stream of bytes to hex and write it to
// the output stream
pub fn to_hex8(b: u8, stream: &mut dyn Stream) -> Result<(), Error> {
    let t = to_hex_tuple(b);
    stream.write(t.0)?;
    stream.write(t.1)?;
    Ok(())
}

pub fn to_hexu(b: &[u8], stream: &mut dyn Stream) -> Result<(), Error> {
    for byte in b {
        to_hex8(*byte, stream)?;
    }
    Ok(())
}
