// FEMTOCAM — hostname_util.h
// Portable mDNS/DNS hostname label sanitizer (no Arduino deps so it is unit-testable).
#pragma once
#include <stddef.h>
#include <string.h>

#ifndef HOSTNAME_MAX_LABEL
#define HOSTNAME_MAX_LABEL 63
#endif

// Normalize a user-supplied hostname into a valid single DNS label:
//   - lowercase ASCII letters, digits, and '-'
//   - space / '_' / '.' become '-'
//   - any other byte (incl. multi-byte UTF-8) is dropped
//   - leading and trailing '-' are trimmed
//   - length capped at HOSTNAME_MAX_LABEL
//   - if the result is empty, `fallback` is copied into `out` (when provided)
// Returns the length of the written string (excluding NUL).
static inline size_t sanitizeHostname(const char* in, char* out, size_t out_sz, const char* fallback) {
    if (!out || out_sz == 0) return 0;
    out[0] = '\0';
    size_t n = 0;
    if (in) {
        for (size_t i = 0; in[i] != '\0' && n + 1 < out_sz && n < HOSTNAME_MAX_LABEL; i++) {
            unsigned char c = (unsigned char)in[i];
            if (c >= 'A' && c <= 'Z') c = (unsigned char)(c - 'A' + 'a');
            if ((c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') || c == '-') {
                out[n++] = (char)c;
            } else if (c == ' ' || c == '_' || c == '.') {
                out[n++] = '-';
            }
        }
    }
    out[n] = '\0';
    size_t lead = 0;
    while (lead < n && out[lead] == '-') lead++;
    if (lead > 0) {
        memmove(out, out + lead, n - lead + 1);
        n -= lead;
    }
    while (n > 0 && out[n - 1] == '-') {
        out[--n] = '\0';
    }
    if (n == 0 && fallback && fallback[0] != '\0') {
        size_t fl = strlen(fallback);
        if (fl >= out_sz) fl = out_sz - 1;
        memcpy(out, fallback, fl);
        out[fl] = '\0';
        n = fl;
    }
    return n;
}
