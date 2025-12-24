/**
 * @file RustFFI.h
 * @brief C++ wrapper for Meshtastic Rust library FFI
 * 
 * This header provides C++ wrappers for calling Rust functions.
 */

#ifndef RUST_FFI_H
#define RUST_FFI_H

#include <string>
#include <memory>

// Include the generated C bindings
extern "C" {
#include "rust_ffi.h"
}

namespace meshtastic {
namespace rust {

/**
 * RAII wrapper for Rust-allocated strings
 */
class RustString {
private:
    const char* ptr;
    
public:
    explicit RustString(const char* p) : ptr(p) {}
    
    ~RustString() {
        if (ptr) {
            rust_free_string(const_cast<char*>(ptr));
        }
    }
    
    // Delete copy operations
    RustString(const RustString&) = delete;
    RustString& operator=(const RustString&) = delete;
    
    // Allow move operations
    RustString(RustString&& other) noexcept : ptr(other.ptr) {
        other.ptr = nullptr;
    }
    
    RustString& operator=(RustString&& other) noexcept {
        if (this != &other) {
            if (ptr) {
                rust_free_string(const_cast<char*>(ptr));
            }
            ptr = other.ptr;
            other.ptr = nullptr;
        }
        return *this;
    }
    
    const char* c_str() const { return ptr; }
    std::string to_string() const { return ptr ? std::string(ptr) : ""; }
};

/**
 * Get a greeting message from Rust
 */
inline std::string getHello() {
    RustString msg(rust_hello());
    return msg.to_string();
}

/**
 * Add two integers using Rust
 */
inline int32_t add(int32_t a, int32_t b) {
    return rust_add(a, b);
}

} // namespace rust
} // namespace meshtastic

#endif // RUST_FFI_H
