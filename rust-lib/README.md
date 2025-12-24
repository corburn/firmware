# Meshtastic Rust Library

This directory contains Rust components for the Meshtastic firmware, providing performance-critical or memory-safe implementations that can be called from the C++ codebase via FFI (Foreign Function Interface).

## Structure

- `src/lib.rs` - Main Rust library with FFI exports
- `cbindgen.toml` - Configuration for generating C headers
- `Cargo.toml` - Rust package configuration

## Building

The Rust library is built as a static library that is linked into the main firmware.

### Prerequisites

- Rust toolchain (rustc 1.70+)
- cargo
- cbindgen (for generating C headers)

### Build Commands

```bash
# Build the Rust library
cd rust-lib
cargo build --release

# Generate C header files (from rust-lib directory)
cbindgen --config cbindgen.toml --crate meshtastic-rust --output ../src/rust_ffi.h

# Run tests
cargo test
```

## Integration with C++

The Rust library exports C-compatible functions that can be called from C++. 

### Generated Files

- `../src/rust_ffi.h` - Auto-generated C header with function declarations
- `../src/RustFFI.h` - C++ wrapper providing RAII and idiomatic C++ interfaces

### Using Rust Functions in C++

```cpp
#include "RustFFI.h"

// Example usage
std::string greeting = meshtastic::rust::getHello();
int32_t sum = meshtastic::rust::add(10, 20);
```

## Adding New Rust Functions

1. Add your Rust function to `src/lib.rs` with `#[no_mangle]` and `extern "C"`
2. Regenerate the C header: `cbindgen --config cbindgen.toml --crate meshtastic-rust --output ../src/rust_ffi.h`
3. Optionally add C++ wrappers to `../src/RustFFI.h`
4. Update the PlatformIO build configuration to link the Rust library

## Migration Strategy

This infrastructure enables incremental migration to Rust:

1. **New Features** - Write new functionality in Rust from the start
2. **Critical Paths** - Rewrite performance-critical code in Rust
3. **Safety-Critical** - Move memory-unsafe operations to Rust for better safety guarantees
4. **Gradual** - The C++ and Rust code coexist, with C++ calling into Rust via FFI

## Testing

```bash
cargo test
cargo test --release
```

## Documentation

Generate and view Rust documentation:

```bash
cargo doc --open
```
