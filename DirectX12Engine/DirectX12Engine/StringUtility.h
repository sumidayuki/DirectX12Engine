#pragma once

// 文字列に関するユーティリティクラス
class StringUtility
{

};


class MBStoWCS
{
public:
    // マルチバイト文字列からワイド文字列に変換した場合に必要なワード数を取得します。
    static int GetRequiredSizeInWords(const char* mbs, int sizeInWordsMBS = -1);

    // マルチバイト文字列からワイド文字列に変換します。
    static int Convert(const char* mbs, int sizeInWordsMBS, wchar_t* wcs, int sizeInWordsWCS);

    // マルチバイト文字列からワイド文字列に変換します。
    static std::wstring Convert(const char* mbs, int sizeInWordsMBS = -1);

    // マルチバイト文字列からワイド文字列に変換します。
    static std::wstring Convert(const std::string& mbs) { return Convert(mbs.data(), (int)mbs.length()); }
};


class WCStoMBS
{
public:
    // ワイド文字列からマルチバイト文字列に変換した場合に必要なワード数を取得します。
    static int GetRequiredSizeInWords(const wchar_t* wcs, int sizeInWordsWCS = -1);

    // ワイド文字列からマルチバイト文字列に変換します。
    static int Convert(const wchar_t* wcs, int sizeInWordsWCS, char* mbs, int sizeInWordsMBS);

    // ワイド文字列からマルチバイト文字列に変換します。
    static std::string Convert(const wchar_t* wcs, int sizeInWordsWCS = -1);

    // ワイド文字列からマルチバイト文字列に変換します。
    static std::string Convert(const std::wstring& wcs) { return Convert(wcs.data(), (int)wcs.length()); }
};


class UTF16LEtoUTF8
{
public:
    // UTF16LEからUTF8に変換した場合に必要なワード数を取得します。
    static int GetRequiredSizeInWords(const wchar_t* utf16, int sizeInWords = -1);

    // UTF16LEからUTF8に変換します。
    static int Convert(const wchar_t* utf16, int sizeInWordsUTF16, char* utf8, int sizeInWordsUTF8);

    // UTF16LEからUTF8に変換します。
    static std::string Convert(const wchar_t* utf16, int sizeInWordsUTF16 = -1);

    // UTF16LEからUTF8に変換します。
    static std::string Convert(const std::wstring& utf16) { return Convert(utf16.data(), (int)utf16.length()); }
};


class UTF16LEtoSJIS
{
public:
    // UTF16LEからSJISに変換した場合に必要なワード数を取得します。
    static int GetRequiredSizeInWords(const wchar_t* utf16, int sizeInWords = -1);

    // UTF16LEからSJISに変換します。
    static int Convert(const wchar_t* utf16, int sizeInWordsUTF16, char* sjis, int sizeInWordsSJIS);

    // UTF16LEからSJISに変換します。
    static std::string Convert(const wchar_t* utf16, int sizeInWordsUTF16 = -1);

    // UTF16LEからSJISに変換します。
    static std::string Convert(const std::wstring& utf16) { return Convert(utf16.data(), (int)utf16.length()); }
};


class UTF8toUTF16LE
{
public:
    // UTF8からUTF16LEに変換した場合に必要なワード数を取得します。
    static int GetRequiredSizeInWords(const char* utf8, int sizeInWords = -1);

    // UTF8からUTF16LEに変換します。
    static int Convert(const char* utf8, int sizeInWordsUTF8, wchar_t* utf16, int sizeInWordsUTF16);

    // UTF8からUTF16LEに変換します。
    static std::wstring Convert(const char* utf8, int sizeInWordsUTF8 = -1);

    // UTF8からUTF16LEに変換します。
    static std::wstring Convert(const std::string& utf8) { return Convert(utf8.data(), (int)utf8.length()); }
};


class UTF8toSJIS
{
public:
    // UTF8からSJISに変換した場合に必要なワード数を取得します。
    static int GetRequiredSizeInWords(const char* utf8, int sizeInWords = -1);

    // UTF8からSJISに変換します。
    static int Convert(const char* utf8, int sizeInWordsUTF8, char* sjis, int sizeInWordsSJIS);

    // UTF8からSJISに変換します。
    static std::string Convert(const char* utf8, int sizeInWordsUTF8 = -1);

    // UTF8からSJISに変換します。
    static std::string Convert(const std::string& utf8) { return Convert(utf8.data(), (int)utf8.length()); }
};


class SJIStoUTF16LE
{
public:
    // Shift-JISからUTF16LEに変換した場合に必要なワード数を取得します。
    static int GetRequiredSizeInWords(const char* sjis, int sizeInWords = -1);

    // Shift-JISからUTF16LEに変換します。
    static int Convert(const char* sjis, int sizeInWordsSJIS, wchar_t* utf16, int sizeInWordsUTF16);

    // Shift-JISからUTF16LEに変換します。
    static std::wstring Convert(const char* sjis, int sizeInWordsSJIS = -1);

    // Shift-JISからUTF16LEに変換します。
    static std::wstring Convert(const std::string& sjis) { return Convert(sjis.data(), (int)sjis.length()); }
};


class SJIStoUTF8
{
public:
    // Shift-JISからUTF8に変換した場合に必要なワード数を取得します。
    static int GetRequiredSizeInWords(const char* sjis, int sizeInWords = -1);

    // Shift-JISからUTF8に変換します。
    static int Convert(const char* sjis, int sizeInWordsSJIS, char* utf8, int sizeInWordsUTF8);

    // Shift-JISからUTF8に変換します。
    static std::string Convert(const char* sjis, int sizeInWordsSJIS = -1);

    // Shift-JISからUTF8に変換します。
    static std::string Convert(const std::string& sjis) { return Convert(sjis.data(), (int)sjis.length()); }
};

