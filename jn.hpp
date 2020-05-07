#pragma once

#include <string>

#define KEI 10000000000000000
#define CHOU 1000000000000
#define OKU 100000000
#define MAN 10000
#define SEN 1000
#define HYAKU 100
#define JUU 10
#define ICHI 1

#define SHIFTJIS_CODEPAGE 932

bool JN_init(void);
wchar_t *JN_load_string(int nID);
const char *JN_wide_to_ansi(int codepage, const wchar_t *str);
int JN_find_hiragana(int nHiraID);
int JN_find_katakana(int nKataID);
int JN_find_kansuuji(int nKanjiID);
std::wstring JN_get_hiragana(int nHiraID);
std::wstring JN_get_katakana(int nKataID);
std::wstring JN_get_kansuuji(int nKanjiID);
std::wstring JN_get_hiragana_number(long long number, bool zero_is_omitted = false);
std::wstring JN_get_katakana_number(long long number, bool zero_is_omitted = false);
std::wstring JN_get_kansuuji_number(long long number, bool zero_is_omitted = false);
int JN_longest_match_hiragana(const wchar_t *str);
int JN_longest_match_katakana(const wchar_t *str);
int JN_longest_match_kanji(const wchar_t *str);
void JN_speak_japanese_number(long long number);
