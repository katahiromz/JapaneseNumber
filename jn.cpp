#ifdef _WIN32
    #include <windows.h>
    #include <mmsystem.h>
#endif
#include "jn.hpp"
#include <vector>
#include <cstdlib>
#include <cstring>
#include "resource.h"

const char *JN_wide_to_ansi(int codepage, const wchar_t *str)
{
    static char s_buf[1024];
    WideCharToMultiByte(codepage, 0, str, -1, s_buf, ARRAYSIZE(s_buf), NULL, NULL);
    return s_buf;
}

struct DATA
{
    long long value;
    int nWaveID;
    int nHiraID;
    int nKataID;
    int nKanjiID;
    std::wstring hiragana;
    std::wstring katakana;
    std::wstring kanji;
};

#define DEFINE_DATA(value, wave_id, hira_id, kata_id, kanji_id, higa, kata, kanji, wave_file) \
    { value, wave_id, hira_id, kata_id, kanji_id },

static DATA s_data[] =
{
#include "data.h"
};

wchar_t *JN_load_string(int nID)
{
    static int s_index = 0;
    const int cchBuffMax = 1024;
    static wchar_t s_sz[4][cchBuffMax];

    wchar_t *pszBuff = s_sz[s_index];
    s_index = (s_index + 1) % ARRAYSIZE(s_sz);
    pszBuff[0] = 0;
    LoadStringW(NULL, nID, pszBuff, cchBuffMax);
    return pszBuff;
}

bool JN_init(void)
{
    size_t i, count = ARRAYSIZE(s_data);
    for (i = 0; i < count; ++i)
    {
        s_data[i].hiragana = JN_load_string(s_data[i].nHiraID);
        s_data[i].katakana = JN_load_string(s_data[i].nKataID);
        s_data[i].kanji = JN_load_string(s_data[i].nKanjiID);
    }
    return true;
}

int JN_find_hiragana(int nHiraID)
{
    size_t i, count = ARRAYSIZE(s_data);
    for (i = 0; i < count; ++i)
    {
        if (s_data[i].nHiraID == nHiraID)
            return (int)i;
    }
    return -1;
}

int JN_find_katakana(int nKataID)
{
    size_t i, count = ARRAYSIZE(s_data);
    for (i = 0; i < count; ++i)
    {
        if (s_data[i].nKataID == nKataID)
            return (int)i;
    }
    return -1;
}

int JN_find_kansuuji(int nKanjiID)
{
    size_t i, count = ARRAYSIZE(s_data);
    for (i = 0; i < count; ++i)
    {
        if (s_data[i].nKanjiID == nKanjiID)
            return (int)i;
    }
    return -1;
}

std::wstring JN_get_hiragana(int nHiraID)
{
    int index = JN_find_hiragana(nHiraID);
    if (index != -1)
        return s_data[index].hiragana;
    return L"";
}

std::wstring JN_get_katakana(int nKataID)
{
    int index = JN_find_katakana(nKataID);
    if (index != -1)
        return s_data[index].katakana;
    return L"";
}

std::wstring JN_get_kansuuji(int nKanjiID)
{
    int index = JN_find_kansuuji(nKanjiID);
    if (index != -1)
        return s_data[index].kanji;
    return L"";
}

std::wstring JN_get_kansuuji_number(long long number, bool zero_is_omitted)
{
    std::wstring ret;
    if (number > 999999999999999999)
    {
        return JN_load_string(IDS_INVALID);
    }
    if (number < 0)
    {
        ret += JN_load_string(IDS_KATAKANA_MINUS);
        ret += JN_get_kansuuji_number(-number, zero_is_omitted);
        return ret;
    }
    if (number == 0)
    {
        if (!zero_is_omitted)
            ret += JN_load_string(IDS_KANJI_0);
        return ret;
    }
    if (number >= KEI)
    {
        ret += JN_get_kansuuji_number(number / KEI, zero_is_omitted);
        ret += JN_load_string(IDS_KANJI_KEI);
        ret += JN_get_kansuuji_number(number % KEI, true);
        return ret;
    }
    if (number >= CHOU)
    {
        ret += JN_get_kansuuji_number(number / CHOU, zero_is_omitted);
        ret += JN_load_string(IDS_KANJI_CHOU);
        ret += JN_get_kansuuji_number(number % CHOU, true);
        return ret;
    }
    if (number >= OKU)
    {
        ret += JN_get_kansuuji_number(number / OKU, zero_is_omitted);
        ret += JN_load_string(IDS_KANJI_OKU);
        ret += JN_get_kansuuji_number(number % OKU, true);
        return ret;
    }
    if (number >= MAN)
    {
        ret += JN_get_kansuuji_number(number / MAN, zero_is_omitted);
        ret += JN_load_string(IDS_KANJI_MAN);
        ret += JN_get_kansuuji_number(number % MAN, true);
        return ret;
    }
    if (number >= SEN)
    {
        ret += JN_load_string(IDS_KANJI_SEN_1 + (int)(number / SEN) - 1);
        ret += JN_get_kansuuji_number(number % SEN, true);
        return ret;
    }
    if (number >= HYAKU)
    {
        ret += JN_load_string(IDS_KANJI_100 + (int)(number / HYAKU) - 1);
        ret += JN_get_kansuuji_number(number % HYAKU, true);
        return ret;
    }
    if (number >= 20)
    {
        ret += JN_load_string(IDS_KANJI_20 + (int)(number / JUU) - 2);
        ret += JN_get_kansuuji_number(number % JUU, true);
        return ret;
    }
    ret += JN_load_string(IDS_KANJI_1 + (int)number - 1);
    return ret;
}

int JN_longest_match_hiragana(const wchar_t *str)
{
    int maxi = -1;
    size_t i, maxlen = 0, count = ARRAYSIZE(s_data);
    for (i = 0; i < count; ++i)
    {
        size_t len = s_data[i].hiragana.size();
        if (std::memcmp(s_data[i].hiragana.c_str(), str, len * sizeof(wchar_t)) == 0)
        {
            if (maxlen < len)
            {
                maxi = (int)i;
                maxlen = len;
            }
        }
    }
    return maxi;
}

int JN_longest_match_katakana(const wchar_t *str)
{
    int maxi = -1;
    size_t i, maxlen = 0, count = ARRAYSIZE(s_data);
    for (i = 0; i < count; ++i)
    {
        size_t len = s_data[i].katakana.size();
        if (std::memcmp(s_data[i].katakana.c_str(), str, len * sizeof(wchar_t)) == 0)
        {
            if (maxlen < len)
            {
                maxi = (int)i;
                maxlen = len;
            }
        }
    }
    return maxi;
}

int JN_longest_match_kanji(const wchar_t *str)
{
    int maxi = -1;
    size_t i, maxlen = 0, count = ARRAYSIZE(s_data);
    for (i = 0; i < count; ++i)
    {
        size_t len = s_data[i].kanji.size();
        if (std::memcmp(s_data[i].kanji.c_str(), str, len * sizeof(wchar_t)) == 0)
        {
            if (maxlen < len)
            {
                maxi = (int)i;
                maxlen = len;
            }
        }
    }
    return maxi;
}

std::wstring JN_get_hiragana_number(long long number, bool zero_is_omitted)
{
    if (number > 999999999999999999)
    {
        return JN_load_string(IDS_INVALID);
    }

    std::wstring kanji = JN_get_kansuuji_number(number, false);

    std::wstring hiragana;
    const wchar_t *pch = kanji.c_str();
    while (*pch)
    {
        int i = JN_longest_match_kanji(pch);
        hiragana += s_data[i].hiragana;
        pch += s_data[i].kanji.size();
    }

    return hiragana;
}

std::wstring JN_get_katakana_number(long long number, bool zero_is_omitted)
{
    if (number > 999999999999999999)
    {
        return JN_load_string(IDS_INVALID);
    }

    std::wstring kanji = JN_get_kansuuji_number(number, false);

    std::wstring katakana;
    const wchar_t *pch = kanji.c_str();
    while (*pch)
    {
        int i = JN_longest_match_kanji(pch);
        katakana += s_data[i].katakana;
        pch += s_data[i].kanji.size();
    }

    return katakana;
}

void JN_speak_japanese_number(long long number)
{
    std::wstring hiragana = JN_get_hiragana_number(number, false);

    const wchar_t *pch = hiragana.c_str();
    std::vector<int> waves;
    while (*pch)
    {
        int i = JN_longest_match_hiragana(pch);
        waves.push_back(i);
        pch += s_data[i].hiragana.size();
    }

    for (auto i : waves)
    {
        PlaySound(MAKEINTRESOURCE(s_data[i].nWaveID),
                  GetModuleHandle(NULL),
                  SND_SYNC | SND_NODEFAULT | SND_NOSTOP | SND_RESOURCE);
    }
}
