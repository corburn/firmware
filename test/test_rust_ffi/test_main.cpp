/**
 * @file test_rust_ffi.cpp
 * @brief Integration tests for Rust FFI bindings
 * 
 * This file tests the Rust library integration with C++ code.
 */

#include <unity.h>
#include <string>

// Include both the C bindings and C++ wrapper
extern "C" {
#include "rust_ffi.h"
}
#include "RustFFI.h"

/**
 * Test basic addition function from Rust
 */
void test_rust_add(void) {
    TEST_ASSERT_EQUAL_INT32(5, rust_add(2, 3));
    TEST_ASSERT_EQUAL_INT32(0, rust_add(0, 0));
    TEST_ASSERT_EQUAL_INT32(-1, rust_add(-3, 2));
    TEST_ASSERT_EQUAL_INT32(100, rust_add(50, 50));
}

/**
 * Test string handling from Rust (direct C API)
 */
void test_rust_hello_c_api(void) {
    const char* msg = rust_hello();
    TEST_ASSERT_NOT_NULL(msg);
    TEST_ASSERT_EQUAL_STRING("Hello from Rust!", msg);
    
    // Clean up
    rust_free_string(const_cast<char*>(msg));
}

/**
 * Test string handling from Rust (C++ wrapper)
 */
void test_rust_hello_cpp_wrapper(void) {
    std::string greeting = meshtastic::rust::getHello();
    TEST_ASSERT_EQUAL_STRING("Hello from Rust!", greeting.c_str());
}

/**
 * Test addition using C++ wrapper
 */
void test_rust_add_cpp_wrapper(void) {
    TEST_ASSERT_EQUAL_INT32(42, meshtastic::rust::add(20, 22));
    TEST_ASSERT_EQUAL_INT32(0, meshtastic::rust::add(-5, 5));
}

/**
 * Test RAII string wrapper
 */
void test_rust_string_raii(void) {
    {
        meshtastic::rust::RustString msg(rust_hello());
        TEST_ASSERT_NOT_NULL(msg.c_str());
        TEST_ASSERT_EQUAL_STRING("Hello from Rust!", msg.c_str());
        
        std::string cpp_str = msg.to_string();
        TEST_ASSERT_EQUAL_STRING("Hello from Rust!", cpp_str.c_str());
        // msg is automatically cleaned up when going out of scope
    }
    // If we reach here without crashing, RAII cleanup worked
    TEST_PASS();
}

void setup() {
    // NOTE!!! Wait for >2 secs
    // if board doesn't support software reset via Serial.DTR/RTS
    delay(2000);

    UNITY_BEGIN();
    
    RUN_TEST(test_rust_add);
    RUN_TEST(test_rust_hello_c_api);
    RUN_TEST(test_rust_hello_cpp_wrapper);
    RUN_TEST(test_rust_add_cpp_wrapper);
    RUN_TEST(test_rust_string_raii);
    
    UNITY_END();
}

void loop() {
    // Nothing to do here
}
