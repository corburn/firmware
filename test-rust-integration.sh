#!/bin/bash
# Test script to verify Rust integration

set -euo pipefail

echo "================================"
echo "Rust Integration Verification"
echo "================================"
echo ""

# Check if Rust is installed
echo "1. Checking Rust installation..."
if ! command -v cargo &> /dev/null; then
    echo "❌ Error: cargo not found. Please install Rust from https://rustup.rs/"
    exit 1
fi
echo "✓ Rust found: $(rustc --version)"
echo ""

# Check if cbindgen is installed
echo "2. Checking cbindgen installation..."
if ! command -v cbindgen &> /dev/null; then
    echo "⚠️  Warning: cbindgen not found. Install with: cargo install cbindgen"
    echo "   Headers can still be built but won't be automatically regenerated."
else
    echo "✓ cbindgen found: $(cbindgen --version)"
fi
echo ""

# Build Rust library
echo "3. Building Rust library..."
cd "$(dirname "$0")"
cargo build --release
echo "✓ Rust library built successfully"
echo ""

# Run Rust tests
echo "4. Running Rust tests..."
cargo test
echo "✓ All Rust tests passed"
echo ""

# Check if library was generated
echo "5. Verifying library artifacts..."
if [ -f "target/release/libmeshtastic_rust.a" ]; then
    SIZE=$(du -h target/release/libmeshtastic_rust.a | cut -f1)
    echo "✓ Static library: target/release/libmeshtastic_rust.a ($SIZE)"
else
    echo "❌ Error: Static library not found"
    exit 1
fi
echo ""

# Check if headers exist
echo "6. Verifying FFI headers..."
if [ -f "src/rust_ffi.h" ]; then
    echo "✓ C header: src/rust_ffi.h"
else
    echo "⚠️  Warning: C header not found at src/rust_ffi.h"
    echo "   Generate with: cd rust-lib && cbindgen --config cbindgen.toml --crate meshtastic-rust --output ../src/rust_ffi.h"
fi

if [ -f "src/RustFFI.h" ]; then
    echo "✓ C++ wrapper: src/RustFFI.h"
else
    echo "❌ Error: C++ wrapper not found at src/RustFFI.h"
    exit 1
fi
echo ""

echo "================================"
echo "✓ All checks passed!"
echo "================================"
echo ""
echo "Next steps:"
echo "  - Run PlatformIO build: pio run"
echo "  - The Rust library will be automatically built and linked"
echo "  - See RUST_MIGRATION.md for usage examples"
