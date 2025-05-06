
#include "defs.h"
#include "test_arch.h"

#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>


void test_i8  (void** state) { (void)state; assert_int_equal(sizeof(dast_i8),   sizeof(int8_t)); }
void test_u8  (void** state) { (void)state; assert_int_equal(sizeof(dast_u8),   sizeof(uint8_t)); }
void test_i16 (void** state) { (void)state; assert_int_equal(sizeof(dast_i16),  sizeof(int16_t)); }
void test_u16 (void** state) { (void)state; assert_int_equal(sizeof(dast_u16),  sizeof(uint16_t)); }
void test_i32 (void** state) { (void)state; assert_int_equal(sizeof(dast_i32),  sizeof(int32_t)); }
void test_u32 (void** state) { (void)state; assert_int_equal(sizeof(dast_u32),  sizeof(uint32_t)); }
void test_i64 (void** state) { (void)state; assert_int_equal(sizeof(dast_i64),  sizeof(int64_t)); }
void test_u64 (void** state) { (void)state; assert_int_equal(sizeof(dast_u64),  sizeof(uint64_t)); }
void test_bool(void** state) { (void)state; assert_int_equal(sizeof(dast_bool), sizeof(int)); }
void test_sz  (void** state) { (void)state; assert_int_equal(sizeof(dast_sz),   sizeof(size_t)); }

void test_arch(void** state) {
    (void)state;
#if !defined(DAST_32BIT) && !defined(DAST_64BIT)
    assert_true(0);
#endif
}
