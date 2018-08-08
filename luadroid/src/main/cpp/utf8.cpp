
#include <malloc.h>
#include "utf8.h"
#include "lua/luaconf.h"


/**
 * Given a UTF-16 string, compute the length of the corresponding UTF-8
 * string in bytes.
 */
size_t strnlen16to8(const char16_t *utf16Str, size_t len) {
    size_t utf8Len = 0;

    while (len--) {
        unsigned int uic = *utf16Str++;

        if (uic > 0x07ff)
            utf8Len += 3;
        else if (uic > 0x7f || uic == 0)
            utf8Len += 2;
        else
            utf8Len++;
    }
    return utf8Len;
}


/**
 * Convert a UTF-16 string to UTF-8.
 *
 * Make sure you allocate "dest" with the result of strnlen16to8(),
 * not just "strlen16()".
 */
char *strndup16to8(const char16_t *s, size_t n) {
    char *ret;

    if (s == NULL) {
        return NULL;
    }

    ret = (char *) malloc(strnlen16to8(s, n) + 1);

    strncpy16to8(ret, s, n);

    return ret;
}

/**
 * Convert a Java-Style UTF-16 string + length to a JNI-Style UTF-8 string.
 *
 * This basically means: embedded \0's in the UTF-16 string are encoded
 * as "0xc0 0x80"
 *
 * Make sure you allocate "utf8Str" with the result of strlen16to8() + 1,
 * not just "len".
 *
 * Please note, a terminated \0 is always added, so your result will always
 * be "strlen16to8() + 1" bytes long.
 */
char *strncpy16to8(char *utf8Str, const char16_t *utf16Str, size_t len) {
    char *utf8cur = utf8Str;

    while (len--) {
        unsigned int uic = *utf16Str++;

        if (uic > 0x07ff) {
            *utf8cur++ = (uic >> 12) | 0xe0;
            *utf8cur++ = ((uic >> 6) & 0x3f) | 0x80;
            *utf8cur++ = (uic & 0x3f) | 0x80;
        } else if (uic > 0x7f || uic == 0) {
            *utf8cur++ = (uic >> 6) | 0xc0;
            *utf8cur++ = (uic & 0x3f) | 0x80;
        } else {
            *utf8cur++ = uic;

            if (uic == 0) {
                break;
            }
        }
    }

    *utf8cur = '\0';

    return utf8Str;
}

#define UTF16_REPLACEMENT_CHAR 0xfffd

/* Clever trick from Dianne that returns 1-4 depending on leading bit sequence*/
#define UTF8_SEQ_LENGTH(ch) (((0xe5000000 >> ((ch >> 3) & 0x1e)) & 3) + 1)

/* note: macro expands to multiple lines */
#define UTF8_SHIFT_AND_MASK(unicode, byte)  \
            (unicode)<<=6; (unicode) |= (0x3f & (byte));

#define UNICODE_UPPER_LIMIT 0x10fffd

/**
 * out_len is an out parameter (which may not be null) containing the
 * length of the UTF-16 string (which may contain embedded \0's)
 */

char16_t *strdup8to16(const char *s, size_t *out_len) {
    char16_t *ret;
    size_t len;

    if (s == NULL) return NULL;

    len = strlen8to16(s);

    // fail on overflow
    if (len && SIZE_MAX / len < sizeof(char16_t))
        return NULL;

    // no plus-one here. UTF-16 strings are not null terminated
    ret = (char16_t *) malloc(sizeof(char16_t) * len);

    return strcpy8to16(ret, s, out_len);
}

/**
 * Like "strlen", but for strings encoded with Java's modified UTF-8.
 *
 * The value returned is the number of UTF-16 characters required
 * to represent this string.
 */
size_t strlen8to16(const char *utf8Str) {
    size_t len = 0;
    int ic;
    int expected = 0;

    while ((ic = *utf8Str++) != '\0') {
        /* bytes that start 0? or 11 are lead bytes and count as characters.*/
        /* bytes that start 10 are extention bytes and are not counted */

        if ((ic & 0xc0) == 0x80) {
            /* count the 0x80 extention bytes. if we have more than
             * expected, then start counting them because strcpy8to16
             * will insert UTF16_REPLACEMENT_CHAR's
             */
            expected--;
            if (expected < 0) {
                len++;
            }
        } else {
            len++;
            expected = UTF8_SEQ_LENGTH(ic) - 1;

            /* this will result in a surrogate pair */
            if (expected == 3) {
                len++;
            }
        }
    }

    return len;
}

/*
 * Retrieve the next UTF-32 character from a UTF-8 string.
 *
 * Stops at inner \0's
 *
 * Returns UTF16_REPLACEMENT_CHAR if an invalid sequence is encountered
 *
 * Advances "*pUtf8Ptr" to the start of the next character.
 */
static inline uint32_t getUtf32FromUtf8(const char **pUtf8Ptr) {
    uint32_t ret;
    int seq_len;
    int i;

    /* Mask for leader byte for lengths 1, 2, 3, and 4 respectively*/
    static const unsigned char leaderMask[4] = {0xff, 0x1f, 0x0f, 0x07};

    /* Bytes that start with bits "10" are not leading characters. */
    if (((**pUtf8Ptr) & 0xc0) == 0x80) {
        (*pUtf8Ptr)++;
        return UTF16_REPLACEMENT_CHAR;
    }

    /* note we tolerate invalid leader 11111xxx here */
    seq_len = UTF8_SEQ_LENGTH(**pUtf8Ptr);

    ret = (unsigned char) (**pUtf8Ptr) & leaderMask[seq_len - 1];

    if (**pUtf8Ptr == '\0') return ret;

    (*pUtf8Ptr)++;
    for (i = 1; i < seq_len; i++, (*pUtf8Ptr)++) {
        if ((**pUtf8Ptr) == '\0') return UTF16_REPLACEMENT_CHAR;
        if (((**pUtf8Ptr) & 0xc0) != 0x80) return UTF16_REPLACEMENT_CHAR;

        UTF8_SHIFT_AND_MASK(ret, **pUtf8Ptr);
    }

    return ret;
}

/**
 * out_len is an out parameter (which may not be null) containing the
 * length of the UTF-16 string (which may contain embedded \0's)
 */
char16_t *strcpy8to16(char16_t *utf16Str, const char *utf8Str,
                      size_t *out_len) {
    char16_t *dest = utf16Str;

    while (*utf8Str != '\0') {
        uint32_t ret;

        ret = getUtf32FromUtf8(&utf8Str);

        if (ret <= 0xffff) {
            *dest++ = (char16_t) ret;
        } else if (ret <= UNICODE_UPPER_LIMIT) {
            /* Create surrogate pairs */
            /* See http://en.wikipedia.org/wiki/UTF-16/UCS-2#Method_for_code_points_in_Plane_1.2C_Plane_2 */

            *dest++ = 0xd800 | ((ret - 0x10000) >> 10);
            *dest++ = 0xdc00 | ((ret - 0x10000) & 0x3ff);
        } else {
            *dest++ = UTF16_REPLACEMENT_CHAR;
        }
    }

    if (out_len != NULL)*out_len = dest - utf16Str;

    return utf16Str;
}
