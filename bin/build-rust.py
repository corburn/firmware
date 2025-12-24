#!/usr/bin/env python3
"""
Build script for Rust library integration with PlatformIO
This script is called during the build process to compile Rust code
and link it with the C++ firmware.
"""

import os
import subprocess
import sys
from pathlib import Path

Import("env")

# Get the project directory
project_dir = env.get("PROJECT_DIR")
rust_lib_dir = os.path.join(project_dir, "rust-lib")
rust_target_dir = os.path.join(project_dir, "target")

# Determine build mode (debug or release)
BUILD_TYPE = env.get("BUILD_TYPE", "release")
rust_profile = "release" if BUILD_TYPE == "release" else "debug"

def build_rust_library():
    """Build the Rust library"""
    print("=" * 60)
    print("Building Rust library...")
    print("=" * 60)
    
    if not os.path.exists(rust_lib_dir):
        print(f"Warning: Rust library directory not found at {rust_lib_dir}")
        print("Skipping Rust build.")
        return None
    
    # Build command
    cargo_args = ["cargo", "build"]
    if rust_profile == "release":
        cargo_args.append("--release")
    
    try:
        # Run cargo build
        result = subprocess.run(
            cargo_args,
            cwd=project_dir,
            check=True,
            capture_output=True,
            text=True
        )
        print(result.stdout)
        if result.stderr:
            print(result.stderr)
        
        # Determine the library path
        lib_path = os.path.join(
            rust_target_dir,
            rust_profile,
            "libmeshtastic_rust.a"
        )
        
        if os.path.exists(lib_path):
            print(f"Rust library built successfully: {lib_path}")
            return lib_path
        else:
            print(f"Warning: Rust library not found at {lib_path}")
            return None
            
    except subprocess.CalledProcessError as e:
        print(f"Error building Rust library: {e}")
        print(f"stdout: {e.stdout}")
        print(f"stderr: {e.stderr}")
        return None
    except FileNotFoundError:
        print("Error: cargo not found. Please install Rust toolchain.")
        print("Visit https://rustup.rs/ to install Rust.")
        return None

def generate_rust_headers():
    """Generate C headers from Rust code using cbindgen"""
    print("Generating Rust FFI headers...")
    
    cbindgen_config = os.path.join(rust_lib_dir, "cbindgen.toml")
    output_header = os.path.join(project_dir, "src", "rust_ffi.h")
    
    if not os.path.exists(cbindgen_config):
        print(f"Warning: cbindgen config not found at {cbindgen_config}")
        return
    
    try:
        result = subprocess.run(
            [
                "cbindgen",
                "--config", cbindgen_config,
                "--crate", "meshtastic-rust",
                "--output", output_header
            ],
            cwd=rust_lib_dir,
            check=True,
            capture_output=True,
            text=True
        )
        print(f"Generated header: {output_header}")
        if result.stderr:
            print(result.stderr)
    except subprocess.CalledProcessError as e:
        print(f"Warning: Failed to generate headers with cbindgen: {e}")
        print("Continuing without regenerating headers...")
    except FileNotFoundError:
        print("Warning: cbindgen not found. Install with: cargo install cbindgen")
        print("Continuing without regenerating headers...")

# Only build Rust if this is not a clean operation
if "clean" not in COMMAND_LINE_TARGETS:
    # Generate headers first
    generate_rust_headers()
    
    # Build the Rust library
    rust_lib_path = build_rust_library()
    
    # Add the Rust library to the build if it was successfully built
    if rust_lib_path and os.path.exists(rust_lib_path):
        # Add the Rust library to the linker
        env.Append(LIBS=[File(rust_lib_path)])
        print(f"Added Rust library to build: {rust_lib_path}")
        
        # Add any additional linker flags if needed
        # env.Append(LINKFLAGS=[...])
else:
    print("Skipping Rust build (clean operation)")
