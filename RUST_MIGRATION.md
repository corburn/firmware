# Rust Migration Guide

This document describes the Rust integration in the Meshtastic firmware and provides guidance for the ongoing migration from C++ to Rust.

## Overview

The Meshtastic firmware now includes infrastructure to support Rust components alongside the existing C++ codebase. This enables:

- **Incremental Migration**: New features can be written in Rust while maintaining C++ compatibility
- **Memory Safety**: Rust's ownership system prevents many classes of bugs
- **Performance**: Rust provides zero-cost abstractions and can match or exceed C++ performance
- **Better Concurrency**: Rust's type system prevents data races at compile time

## Current Status

**Phase 1: Infrastructure Setup** ✅ (Complete)

- [x] Cargo workspace configured
- [x] Rust library crate created (`rust-lib/`)
- [x] FFI bindings infrastructure (cbindgen)
- [x] Example Rust functions with C++ integration
- [x] Build system integration with PlatformIO
- [x] Documentation and examples

## Architecture

```
firmware/
├── Cargo.toml              # Workspace configuration
├── rust-lib/               # Rust library crate
│   ├── src/
│   │   └── lib.rs         # Rust implementation with FFI exports
│   ├── Cargo.toml         # Rust package config
│   ├── cbindgen.toml      # Header generation config
│   └── README.md          # Rust-specific docs
├── src/
│   ├── rust_ffi.h         # Auto-generated C bindings
│   ├── RustFFI.h          # C++ wrapper (RAII, idiomatic)
│   └── ...                # Existing C++ code
└── bin/
    └── build-rust.py      # PlatformIO build integration
```

## Using Rust Functions in C++

### Basic Usage

```cpp
#include "RustFFI.h"

void example() {
    // Call Rust functions through C++ wrappers
    std::string greeting = meshtastic::rust::getHello();
    int32_t sum = meshtastic::rust::add(10, 20);
}
```

### Direct FFI Usage

```cpp
extern "C" {
#include "rust_ffi.h"
}

void example() {
    const char* msg = rust_hello();
    // Use msg...
    rust_free_string(const_cast<char*>(msg));
}
```

## Adding New Rust Components

### 1. Implement in Rust

Add your function to `rust-lib/src/lib.rs`:

```rust
#[no_mangle]
pub extern "C" fn rust_my_function(input: i32) -> i32 {
    // Implementation
    input * 2
}
```

### 2. Generate C Headers

```bash
cd rust-lib
cbindgen --config cbindgen.toml --crate meshtastic-rust --output ../src/rust_ffi.h
```

### 3. Add C++ Wrapper (Optional)

Add a wrapper to `src/RustFFI.h`:

```cpp
inline int32_t myFunction(int32_t input) {
    return rust_my_function(input);
}
```

### 4. Use in C++ Code

```cpp
#include "RustFFI.h"

int32_t result = meshtastic::rust::myFunction(42);
```

## Migration Strategy

### Recommended Approach

1. **Start Small**: Begin with utility functions and data structures
2. **Critical Paths**: Move performance-critical code to Rust
3. **Safety-Critical**: Prioritize code with memory management complexity
4. **Maintain Compatibility**: Ensure C++ code continues to work during migration

### Good Candidates for Rust

- ✅ Data parsing and serialization
- ✅ Cryptographic operations
- ✅ Protocol implementations
- ✅ Buffer management
- ✅ State machines
- ✅ Utility functions (hashing, encoding, etc.)

### Keep in C++ (For Now)

- ⏸️ Hardware abstraction layer (HAL)
- ⏸️ Platform-specific code
- ⏸️ Code with heavy Arduino API dependencies
- ⏸️ UI/Graphics code with existing C++ library dependencies

## Building

The Rust components are automatically built as part of the normal PlatformIO build:

```bash
# Normal build - includes Rust
platformio run -e tbeam

# Build only Rust library
cd rust-lib
cargo build --release

# Run Rust tests
cargo test
```

## Testing

### Rust Unit Tests

```bash
cd rust-lib
cargo test
```

### Integration Tests

Integration testing happens through the normal firmware test suite. The Rust components are exercised through their FFI interfaces.

## Troubleshooting

### Rust Not Found

If you see "cargo not found" errors:

```bash
# Install Rust
curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | sh
source $HOME/.cargo/env
```

### Header Generation Fails

If cbindgen is not installed:

```bash
cargo install cbindgen
```

### Build Errors

1. Ensure Rust toolchain is up to date: `rustup update`
2. Clean build: `cargo clean && platformio run --target clean`
3. Check Rust library builds independently: `cd rust-lib && cargo build`

## Performance Considerations

- Rust FFI calls have minimal overhead (essentially zero for simple functions)
- Avoid frequent small FFI calls in hot loops
- Batch operations when crossing the FFI boundary
- Use `#[inline]` in Rust for functions called frequently

## Safety Guidelines

### Memory Management

- **Rust → C++**: Rust allocates, Rust frees (provide cleanup functions)
- **C++ → Rust**: Pass by value or use raw pointers carefully
- **Shared Data**: Use `Arc` or similar for shared ownership

### Example: String Handling

```rust
// Rust allocates
#[no_mangle]
pub extern "C" fn create_string() -> *mut c_char {
    CString::new("hello").unwrap().into_raw()
}

// Rust frees
#[no_mangle]
pub extern "C" fn free_string(s: *mut c_char) {
    if !s.is_null() {
        unsafe { CString::from_raw(s); }
    }
}
```

```cpp
// C++ usage with RAII
class RustString {
    char* ptr;
public:
    explicit RustString() : ptr(create_string()) {}
    ~RustString() { free_string(ptr); }
    const char* c_str() const { return ptr; }
};
```

## Resources

- [The Rust FFI Omnibus](http://jakegoulding.com/rust-ffi-omnibus/)
- [cbindgen User Guide](https://github.com/eqrion/cbindgen/blob/master/docs.md)
- [Rust Book - FFI](https://doc.rust-lang.org/nomicon/ffi.html)
- [PlatformIO Scripting API](https://docs.platformio.org/en/latest/scripting/index.html)

## Next Steps

- [ ] Migrate utility functions (encoding, hashing, etc.)
- [ ] Move protocol buffers handling to Rust
- [ ] Implement cryptographic operations in Rust
- [ ] Add Rust-based packet parsing
- [ ] Profile and optimize FFI boundaries
- [ ] Set up benchmarking infrastructure

## Contributing

When contributing Rust code:

1. Follow Rust naming conventions (snake_case for functions)
2. Add doc comments (`///`) for all public items
3. Include unit tests for new functionality
4. Run `cargo fmt` and `cargo clippy` before committing
5. Update FFI headers with cbindgen
6. Add C++ wrappers for ergonomic usage

## Questions?

For questions about the Rust migration, please:

1. Check existing documentation in `rust-lib/README.md`
2. Review the example code in `rust-lib/src/lib.rs`
3. Open a GitHub discussion or issue
