// Host-side unit tests for the mDNS hostname sanitizer.
// Run with: pio test -e native
#include <unity.h>
#include <string.h>
#include "hostname_util.h"

static char buf[HOSTNAME_MAX_LABEL + 1];

void setUp(void) { memset(buf, 0xAA, sizeof(buf)); buf[0] = '\0'; }
void tearDown(void) {}

static void test_passthrough_valid(void) {
    size_t n = sanitizeHostname("femtocam", buf, sizeof(buf), "fallback");
    TEST_ASSERT_EQUAL_STRING("femtocam", buf);
    TEST_ASSERT_EQUAL_size_t(8, n);
}

static void test_lowercases_uppercase(void) {
    sanitizeHostname("FemtoCAM", buf, sizeof(buf), "fallback");
    TEST_ASSERT_EQUAL_STRING("femtocam", buf);
}

static void test_replaces_space_dot_underscore(void) {
    sanitizeHostname("my cam_1.dev", buf, sizeof(buf), "fallback");
    TEST_ASSERT_EQUAL_STRING("my-cam-1-dev", buf);
}

static void test_drops_unicode_and_specials(void) {
    sanitizeHostname("ca\xc3\xa9m!@#$%^&*()", buf, sizeof(buf), "fallback");
    TEST_ASSERT_EQUAL_STRING("cam", buf);
}

static void test_trims_leading_and_trailing_hyphens(void) {
    sanitizeHostname("---hello---", buf, sizeof(buf), "fallback");
    TEST_ASSERT_EQUAL_STRING("hello", buf);
}

static void test_trims_chars_that_become_hyphens(void) {
    sanitizeHostname("___cam___", buf, sizeof(buf), "fallback");
    TEST_ASSERT_EQUAL_STRING("cam", buf);
}

static void test_empty_input_uses_fallback(void) {
    size_t n = sanitizeHostname("", buf, sizeof(buf), "fallback");
    TEST_ASSERT_EQUAL_STRING("fallback", buf);
    TEST_ASSERT_EQUAL_size_t(8, n);
}

static void test_null_input_uses_fallback(void) {
    sanitizeHostname(NULL, buf, sizeof(buf), "femtocam");
    TEST_ASSERT_EQUAL_STRING("femtocam", buf);
}

static void test_all_invalid_input_uses_fallback(void) {
    sanitizeHostname("\xff\xfe!!!", buf, sizeof(buf), "femtocam");
    TEST_ASSERT_EQUAL_STRING("femtocam", buf);
}

static void test_caps_at_max_label_length(void) {
    char in[200];
    memset(in, 'a', sizeof(in) - 1);
    in[sizeof(in) - 1] = '\0';
    size_t n = sanitizeHostname(in, buf, sizeof(buf), "fallback");
    TEST_ASSERT_EQUAL_size_t(HOSTNAME_MAX_LABEL, n);
    TEST_ASSERT_EQUAL_size_t(HOSTNAME_MAX_LABEL, strlen(buf));
}

static void test_respects_small_output_buffer(void) {
    char small[6];
    size_t n = sanitizeHostname("femtocam", small, sizeof(small), "fb");
    TEST_ASSERT_EQUAL_size_t(5, n);
    TEST_ASSERT_EQUAL_STRING("femto", small);
}

static void test_zero_buffer_size_is_safe(void) {
    char dummy[1] = {'x'};
    size_t n = sanitizeHostname("anything", dummy, 0, "fb");
    TEST_ASSERT_EQUAL_size_t(0, n);
    TEST_ASSERT_EQUAL_CHAR('x', dummy[0]);
}

static void test_hyphen_in_middle_kept(void) {
    sanitizeHostname("femto-cam-2", buf, sizeof(buf), "fb");
    TEST_ASSERT_EQUAL_STRING("femto-cam-2", buf);
}

static void test_korean_input_falls_back(void) {
    sanitizeHostname("\xea\xb8\xb0\xea\xb8\xb0", buf, sizeof(buf), "femtocam");
    TEST_ASSERT_EQUAL_STRING("femtocam", buf);
}

int main(int, char**) {
    UNITY_BEGIN();
    RUN_TEST(test_passthrough_valid);
    RUN_TEST(test_lowercases_uppercase);
    RUN_TEST(test_replaces_space_dot_underscore);
    RUN_TEST(test_drops_unicode_and_specials);
    RUN_TEST(test_trims_leading_and_trailing_hyphens);
    RUN_TEST(test_trims_chars_that_become_hyphens);
    RUN_TEST(test_empty_input_uses_fallback);
    RUN_TEST(test_null_input_uses_fallback);
    RUN_TEST(test_all_invalid_input_uses_fallback);
    RUN_TEST(test_caps_at_max_label_length);
    RUN_TEST(test_respects_small_output_buffer);
    RUN_TEST(test_zero_buffer_size_is_safe);
    RUN_TEST(test_hyphen_in_middle_kept);
    RUN_TEST(test_korean_input_falls_back);
    return UNITY_END();
}
