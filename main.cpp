#include "jn.hpp"
#include <cstdlib>
#include <cstdio>
#include <cstring>

using std::printf;

static void JN_show_version(void)
{
    printf("jn version 0.5 by katahiromz\n");
}

static void JN_show_help(void)
{
    printf("jn --- Display/Speak a Japanese number\n");
    printf("Usage: jn arabic_digits\n");
}

static void JN_print_japanese_number(long long number)
{
    std::wstring str;

    str = JN_get_hiragana_number(number, false);
    printf("%d\n", str.size());
    printf("%s\n", JN_wide_to_ansi(SHIFTJIS_CODEPAGE, str.c_str()));

    str = JN_get_katakana_number(number, false);
    printf("%s\n", JN_wide_to_ansi(SHIFTJIS_CODEPAGE, str.c_str()));

    str = JN_get_kansuuji_number(number, false);
    printf("%s\n", JN_wide_to_ansi(SHIFTJIS_CODEPAGE, str.c_str()));
}

void JN_just_do_it(const char *str)
{
    long long number = std::strtoll(str, NULL, 10);
    JN_print_japanese_number(number);
    JN_speak_japanese_number(number);
}

int main(int argc, char **argv)
{
    if (argc <= 1 || std::strcmp(argv[1], "--help") == 0)
    {
        JN_show_help();
        return 0;
    }

    if (std::strcmp(argv[1], "--version") == 0)
    {
        JN_show_version();
        return 0;
    }

    if (!JN_init())
    {
        printf("ERROR: Cannot load data\n");
        return -1;
    }

    JN_just_do_it(argv[1]);

    return 0;
}
