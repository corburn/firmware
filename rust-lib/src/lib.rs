//! Meshtastic Rust Library
//! 
//! This library provides Rust implementations of various Meshtastic functionality
//! that can be called from C++ via FFI.

use std::ffi::CString;
use std::os::raw::c_char;

/// Example function that can be called from C++
/// Returns a greeting message, or null on allocation failure
#[no_mangle]
pub extern "C" fn rust_hello() -> *const c_char {
    match CString::new("Hello from Rust!") {
        Ok(message) => message.into_raw(),
        Err(_) => std::ptr::null(),
    }
}

/// Free a string allocated by Rust
#[no_mangle]
pub extern "C" fn rust_free_string(s: *mut c_char) {
    if s.is_null() {
        return;
    }
    unsafe {
        let _ = CString::from_raw(s);
    }
}

/// Example computation function
#[no_mangle]
pub extern "C" fn rust_add(a: i32, b: i32) -> i32 {
    a + b
}

#[cfg(test)]
mod tests {
    use super::*;
    use std::ffi::CStr;

    #[test]
    fn test_add() {
        assert_eq!(rust_add(2, 3), 5);
    }

    #[test]
    fn test_hello() {
        let msg = rust_hello();
        assert!(!msg.is_null());
        unsafe {
            let c_str = CStr::from_ptr(msg);
            assert_eq!(c_str.to_str().unwrap(), "Hello from Rust!");
            rust_free_string(msg as *mut c_char);
        }
    }
}
