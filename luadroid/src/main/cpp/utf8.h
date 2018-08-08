//
// Created by Karven on 2018/1/27.
//

#ifndef LUADROID_UFT8_H
#define LUADROID_UFT8_H

size_t strnlen16to8(const char16_t *utf16Str, size_t len);

char *strndup16to8(const char16_t *s, size_t n);

size_t strlen8to16(const char *utf8Str);

char16_t *strdup8to16(const char *s, size_t *out_len);

char16_t *strcpy8to16(char16_t *utf16Str, const char *utf8Str,
                      size_t *out_len);

char *strncpy16to8(char *utf8Str, const char16_t *utf16Str, size_t len);

#endif //LUADROID_UFT8_H
