#include <windows.h>
#include <mmsystem.h>
#include <string>
#include <vector>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include "resource.h"

using std::printf;

const long long KEI = 10000000000000000;
const long long CHOU = 1000000000000;
const long long OKU = 100000000;
const long long MAN = 10000;
const long long SEN = 1000;
const long long HYAKU = 100;
const long long JUU = 10;
const long long ICHI = 1;

#define SHIFTJIS_CODEPAGE 932

void show_version(void)
{
    printf("jn version 0.5 by katahiromz\n");
}

void show_help(void)
{
    printf("jn --- Display/Speak a Japanese number\n");
    printf("Usage: jn arabic_digits\n");
}

LPSTR WideToAnsi(int codepage, const wchar_t *str)
{
    static char s_buf[1024];
    WideCharToMultiByte(codepage, 0, str, -1, s_buf, 1024, NULL, NULL);
    return s_buf;
}

struct DATA
{
    int nWaveID;
    int nHiraID;
    int nKataID;
    int nKanjiID;
    std::wstring hiragana;
    std::wstring katakana;
    std::wstring kanji;
};

#define DEFINE_DATA(wave_id, hira_id, kata_id, kanji_id, higa, kata, kanji, wave_file) \
    { wave_id, hira_id, kata_id, kanji_id },

static DATA s_data[] =
{
#include "data.h"
};

LPWSTR LoadStringDx(INT nID)
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

bool LoadData(void)
{
    size_t i, count = ARRAYSIZE(s_data);
    for (i = 0; i < count; ++i)
    {
        s_data[i].hiragana = LoadStringDx(s_data[i].nHiraID);
        s_data[i].katakana = LoadStringDx(s_data[i].nKataID);
        s_data[i].kanji = LoadStringDx(s_data[i].nKanjiID);
    }
    return true;
}

int FindHiragana(int nHiraID)
{
    size_t i, count = ARRAYSIZE(s_data);
    for (i = 0; i < count; ++i)
    {
        if (s_data[i].nHiraID == nHiraID)
            return (int)i;
    }
    return -1;
}

int FindKatakana(int nKataID)
{
    size_t i, count = ARRAYSIZE(s_data);
    for (i = 0; i < count; ++i)
    {
        if (s_data[i].nKataID == nKataID)
            return (int)i;
    }
    return -1;
}

int FindKanji(int nKanjiID)
{
    size_t i, count = ARRAYSIZE(s_data);
    for (i = 0; i < count; ++i)
    {
        if (s_data[i].nKanjiID == nKanjiID)
            return (int)i;
    }
    return -1;
}

std::wstring GetHiragana(int nHiraID)
{
    int index = FindHiragana(nHiraID);
    if (index != -1)
        return s_data[index].hiragana;
    return L"";
}

std::wstring GetKatakana(int nKataID)
{
    int index = FindKatakana(nKataID);
    if (index != -1)
        return s_data[index].katakana;
    return L"";
}

std::wstring GetKanji(int nKanjiID)
{
    int index = FindKanji(nKanjiID);
    if (index != -1)
        return s_data[index].kanji;
    return L"";
}

std::wstring get_kansuuji_number(long long number, bool zero_is_omitted)
{
    std::wstring ret;
    if (number < 0)
    {
        ret += LoadStringDx(IDS_KATAKANA_MINUS);
        ret += get_kansuuji_number(-number, zero_is_omitted);
        return ret;
    }
    if (number == 0)
    {
        if (!zero_is_omitted)
            ret += LoadStringDx(IDS_KANJI_0);
        return ret;
    }
    if (number >= KEI)
    {
        ret += get_kansuuji_number(number / KEI, zero_is_omitted);
        ret += LoadStringDx(IDS_KANJI_KEI);
        ret += get_kansuuji_number(number % KEI, true);
        return ret;
    }
    if (number >= CHOU)
    {
        ret += get_kansuuji_number(number / CHOU, zero_is_omitted);
        ret += LoadStringDx(IDS_KANJI_CHOU);
        ret += get_kansuuji_number(number % CHOU, true);
        return ret;
    }
    if (number >= OKU)
    {
        ret += get_kansuuji_number(number / OKU, zero_is_omitted);
        ret += LoadStringDx(IDS_KANJI_OKU);
        ret += get_kansuuji_number(number % OKU, true);
        return ret;
    }
    if (number >= MAN)
    {
        ret += get_kansuuji_number(number / MAN, zero_is_omitted);
        ret += LoadStringDx(IDS_KANJI_MAN);
        ret += get_kansuuji_number(number % MAN, true);
        return ret;
    }
    if (number >= SEN)
    {
        ret += LoadStringDx(IDS_KANJI_SEN_1 + (int)(number / SEN) - 1);
        ret += get_kansuuji_number(number % SEN, true);
        return ret;
    }
    if (number >= HYAKU)
    {
        ret += LoadStringDx(IDS_KANJI_100 + (int)(number / HYAKU) - 1);
        ret += get_kansuuji_number(number % HYAKU, true);
        return ret;
    }
    if (number >= 20)
    {
        ret += LoadStringDx(IDS_KANJI_20 + (int)(number / JUU) - 2);
        ret += get_kansuuji_number(number % JUU, true);
        return ret;
    }
    ret += LoadStringDx(IDS_KANJI_1 + (int)number - 1);
    return ret;
}

int longest_match_hiragana(const wchar_t *str)
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

int longest_match_katakana(const wchar_t *str)
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

int longest_match_kanji(const wchar_t *str)
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

std::wstring get_hiragana_number(long long number, bool zero_is_omitted)
{
    std::wstring kanji = get_kansuuji_number(number, false);

    std::wstring hiragana;
    const wchar_t *pch = kanji.c_str();
    while (*pch)
    {
        int i = longest_match_kanji(pch);
        hiragana += s_data[i].hiragana;
        pch += s_data[i].kanji.size();
    }

    return hiragana;
}

std::wstring get_katakana_number(long long number, bool zero_is_omitted)
{
    std::wstring kanji = get_kansuuji_number(number, false);

    std::wstring katakana;
    const wchar_t *pch = kanji.c_str();
    while (*pch)
    {
        int i = longest_match_kanji(pch);
        katakana += s_data[i].katakana;
        pch += s_data[i].kanji.size();
    }

    return katakana;
}

void print_japanese_number(long long number)
{
    std::wstring str;

    str = get_hiragana_number(number, false);
    printf("%s\n", WideToAnsi(SHIFTJIS_CODEPAGE, str.c_str()));

    str = get_katakana_number(number, false);
    printf("%s\n", WideToAnsi(SHIFTJIS_CODEPAGE, str.c_str()));

    str = get_kansuuji_number(number, false);
    printf("%s\n", WideToAnsi(SHIFTJIS_CODEPAGE, str.c_str()));
}

void speak_japanese_number(long long number)
{
    std::wstring hiragana = get_hiragana_number(number, false);

    const wchar_t *pch = hiragana.c_str();
    std::vector<int> waves;
    while (*pch)
    {
        int i = longest_match_hiragana(pch);
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

void just_do_it(const char *str)
{
    long long number = std::strtoll(str, NULL, 10);
    if (number > 999999999999999999 || std::strlen(str) > 18)
    {
        printf("%s\n", WideToAnsi(SHIFTJIS_CODEPAGE, LoadStringDx(IDS_INVALID)));
        return;
    }
    print_japanese_number(number);
    speak_japanese_number(number);
}

int main(int argc, char **argv)
{
    if (argc <= 1 || lstrcmpiA(argv[1], "--help") == 0)
    {
        show_help();
        return 0;
    }

    if (lstrcmpiA(argv[1], "--version") == 0)
    {
        show_version();
        return 0;
    }

    if (!LoadData())
    {
        printf("ERROR: Cannot load data\n");
        return -1;
    }

    just_do_it(argv[1]);

    return 0;
}
