use crate::error::Error;

/// basic stream implementation
/// for command responses
pub trait Stream {
    /// returns buffer lenght, or 0 if
    /// the buffer has no length
    fn len(&self) -> usize {
        0
    }

    fn is_empty(&self) -> bool {
        self.len() == 0
    }

    /// current write position or 0
    fn pos(&self) -> usize {
        0
    }

    /// flush the buffer and reset the write
    fn flush(&mut self) {}

    /// reset the write
    fn reset(&mut self) {}

    /// attempts resize the buffer
    /// return true if buffer was flushed or false if not
    fn resize(&mut self) -> bool {
        false
    }

    /// should return a sum of all byte written since last reset
    fn chksm(&self) -> u32 {
        0
    }

    /// write to the next byte
    fn write(&mut self, byte: u8) -> Result<usize, Error>;
}

/// Sample stream with a fixed buffer
pub struct BufferedStream {
    pub buffer: [u8; 512],
    pub pos: usize,
    chksm: u32,
}

impl Default for BufferedStream {
    fn default() -> Self {
        Self::new()
    }
}

impl BufferedStream {
    pub fn new() -> Self {
        Self {
            buffer: [0; 512],
            pos: 0,
            chksm: 0,
        }
    }
}

impl Stream for BufferedStream {
    fn len(&self) -> usize {
        self.buffer.len()
    }

    fn pos(&self) -> usize {
        self.pos
    }

    fn reset(&mut self) {
        self.pos = 0;
        self.chksm = 0;
    }

    fn chksm(&self) -> u32 {
        self.chksm
    }

    fn write(&mut self, byte: u8) -> Result<usize, Error> {
        let start = self.pos;
        if self.len() < self.pos() + 1 {
            // attempt to handle memory fill
            if !self.resize() {
                return Err(Error::MemoryFilledInterupt);
            } else {
                self.pos = 0;
            }
        }

        // add all bytes but start and end
        if byte != b'$' && byte != b'#' {
            self.chksm += byte as u32;
        }

        self.buffer[self.pos] = byte;
        self.pos += 1;
        Ok(self.pos - start)
    }
}
