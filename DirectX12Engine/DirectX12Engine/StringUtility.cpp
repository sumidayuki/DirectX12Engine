#include "StringUtility.h"

std::string StringUtility::ReadAllText(const std::wstring& path)
{
    std::ifstream ifs(path, std::ios::binary);
    if (!ifs) return {};

    return std::string((std::istreambuf_iterator<char>(ifs)),
        std::istreambuf_iterator<char>());
}

//---------------------------------------------------------------------------------------------------------------------------------------------
// マルチバイト文字列からワイド文字列に変換
//---------------------------------------------------------------------------------------------------------------------------------------------
int MBStoWCS::GetRequiredSizeInWords(const char* mbs, int sizeInWordsMBS)
{
    return ::MultiByteToWideChar(CP_ACP, 0, mbs, sizeInWordsMBS, nullptr, 0);
}


int MBStoWCS::Convert(const char* mbs, int sizeInWordsMBS, wchar_t* wcs, int sizeInWordsWCS)
{
    return ::MultiByteToWideChar(CP_ACP, 0, mbs, sizeInWordsMBS, wcs, sizeInWordsWCS);
}


std::wstring MBStoWCS::Convert(const char* mbs, int sizeInWordsMBS)
{
    std::wstring wcs;

    // マルチバイト文字列からワイド文字列に変換したときの文字列長を求める。
    const int requiredSizeInWords = GetRequiredSizeInWords(mbs, sizeInWordsMBS);
    if (requiredSizeInWords > 0)
    {
        wcs.resize(requiredSizeInWords);
        ::MultiByteToWideChar(CP_ACP, 0, mbs, sizeInWordsMBS, wcs.data(), (int)wcs.size());
    }

    return wcs;
}


//---------------------------------------------------------------------------------------------------------------------------------------------
// ワイド文字列からマルチバイト文字列に変換
//---------------------------------------------------------------------------------------------------------------------------------------------
int WCStoMBS::GetRequiredSizeInWords(const wchar_t* wcs, int sizeInWordsWCS)
{
    return ::WideCharToMultiByte(CP_ACP, 0, wcs, sizeInWordsWCS, nullptr, 0, nullptr, nullptr);
}


int WCStoMBS::Convert(const wchar_t* wcs, int sizeInWordsWCS, char* mbs, int sizeInWordsMBS)
{
    return ::WideCharToMultiByte(CP_ACP, 0, wcs, sizeInWordsWCS, mbs, sizeInWordsMBS, nullptr, nullptr);
}


std::string WCStoMBS::Convert(const wchar_t* wcs, int sizeInWordsWCS)
{
    std::string mbs;

    // ワイド文字列からマルチバイト文字列に変換したときの文字列長を求める。
    const int requiredSizeInWords = GetRequiredSizeInWords(wcs, sizeInWordsWCS);
    if (requiredSizeInWords > 0)
    {
        mbs.resize(requiredSizeInWords);
        ::WideCharToMultiByte(CP_ACP, 0, wcs, sizeInWordsWCS, mbs.data(), (int)mbs.size(), nullptr, nullptr);
    }

    return mbs;
}


//---------------------------------------------------------------------------------------------------------------------------------------------
// UTF16LEからUTF8に変換
//---------------------------------------------------------------------------------------------------------------------------------------------
int UTF16LEtoUTF8::GetRequiredSizeInWords(const wchar_t* utf16, int sizeInWords)
{
    return ::WideCharToMultiByte(CP_UTF8, 0, utf16, sizeInWords, nullptr, 0, nullptr, nullptr);
}


int UTF16LEtoUTF8::Convert(const wchar_t* utf16, int sizeInWordsUTF16, char* utf8, int sizeInWordsUTF8)
{
    return ::WideCharToMultiByte(CP_UTF8, 0, utf16, sizeInWordsUTF16, utf8, sizeInWordsUTF8, nullptr, nullptr);
}


std::string UTF16LEtoUTF8::Convert(const wchar_t* utf16, int sizeInWordsUTF16)
{
    std::string utf8;

    // UTF16LEからUTF8に変換したときの文字列長を求める。
    const int requiredSizeInWords = GetRequiredSizeInWords(utf16, sizeInWordsUTF16);
    if (requiredSizeInWords > 0)
    {
        utf8.resize(requiredSizeInWords);
        ::WideCharToMultiByte(CP_UTF8, 0, utf16, sizeInWordsUTF16, utf8.data(), (int)utf8.size(), nullptr, nullptr);
    }

    return utf8;
}


//---------------------------------------------------------------------------------------------------------------------------------------------
// UTF16LEからSJISに変換 (WCStoMBSと同じ)
//---------------------------------------------------------------------------------------------------------------------------------------------

int UTF16LEtoSJIS::GetRequiredSizeInWords(const wchar_t* utf16, int sizeInWords)
{
    return ::WideCharToMultiByte(CP_ACP, 0, utf16, sizeInWords, nullptr, 0, nullptr, nullptr);
}


int UTF16LEtoSJIS::Convert(const wchar_t* utf16, int sizeInWordsUTF16, char* sjis, int sizeInWordsSJIS)
{
    return ::WideCharToMultiByte(CP_ACP, 0, utf16, sizeInWordsUTF16, sjis, sizeInWordsSJIS, nullptr, nullptr);
}


std::string UTF16LEtoSJIS::Convert(const wchar_t* utf16, int sizeInWordsUTF16)
{
    std::string sjis;

    // UTF16LEからSJISに変換したときの文字列長を求める。
    const int requiredSizeInWords = GetRequiredSizeInWords(utf16, sizeInWordsUTF16);
    if (requiredSizeInWords > 0)
    {
        sjis.resize(requiredSizeInWords);
        ::WideCharToMultiByte(CP_ACP, 0, utf16, sizeInWordsUTF16, sjis.data(), (int)sjis.size(), nullptr, nullptr);
    }

    return sjis;
}


//---------------------------------------------------------------------------------------------------------------------------------------------
// UTF8からUTF16LEに変換
//---------------------------------------------------------------------------------------------------------------------------------------------
int UTF8toUTF16LE::GetRequiredSizeInWords(const char* utf8, int sizeInWords)
{
    return ::MultiByteToWideChar(CP_UTF8, 0, utf8, sizeInWords, nullptr, 0);
}


int UTF8toUTF16LE::Convert(const char* utf8, int sizeInWordsUTF8, wchar_t* utf16, int sizeInWordsUTF16)
{
    return ::MultiByteToWideChar(CP_UTF8, 0, utf8, sizeInWordsUTF8, utf16, sizeInWordsUTF16);
}


std::wstring UTF8toUTF16LE::Convert(const char* utf8, int sizeInWordsUTF8)
{
    std::wstring utf16;
    // UTF8からUTF16LEに変換したときの文字列長を求める。
    const int requiredSizeInWords = GetRequiredSizeInWords(utf8, sizeInWordsUTF8);
    if (requiredSizeInWords > 0)
    {
        utf16.resize(requiredSizeInWords);
        ::MultiByteToWideChar(CP_UTF8, 0, utf8, sizeInWordsUTF8, utf16.data(), (int)utf16.size());
    }
    return utf16;
}


//---------------------------------------------------------------------------------------------------------------------------------------------
// UTF8からSJISに変換
//---------------------------------------------------------------------------------------------------------------------------------------------
int UTF8toSJIS::GetRequiredSizeInWords(const char* utf8, int sizeInWords)
{
    // UTF8 ⇒ UTF16LE ⇒ SJIS
    const std::wstring utf16 = UTF8toUTF16LE::Convert(utf8, sizeInWords);
    return UTF16LEtoSJIS::GetRequiredSizeInWords(utf16.data(), (int)utf16.length());
}


int UTF8toSJIS::Convert(const char* utf8, int sizeInWordsUTF8, char* sjis, int sizeInWordsSJIS)
{
    // UTF8 ⇒ UTF16LE ⇒ SJIS
    const std::wstring utf16 = UTF8toUTF16LE::Convert(utf8, sizeInWordsUTF8);
    return UTF16LEtoSJIS::Convert(utf16.data(), (int)utf16.length(), sjis, sizeInWordsSJIS);
}


std::string UTF8toSJIS::Convert(const char* utf8, int sizeInWordsUTF8)
{
    std::string sjis;

    // UTF8からSJISに変換したときの文字列長を求める。
    const int requiredSizeInWords = GetRequiredSizeInWords(utf8, sizeInWordsUTF8);
    if (requiredSizeInWords > 0)
    {
        sjis.resize(requiredSizeInWords);
        Convert(utf8, sizeInWordsUTF8, sjis.data(), (int)sjis.size());
    }

    return sjis;
}


//---------------------------------------------------------------------------------------------------------------------------------------------
// SJISからUTF16LEに変換
//---------------------------------------------------------------------------------------------------------------------------------------------
int SJIStoUTF16LE::GetRequiredSizeInWords(const char* sjis, int sizeInWordsSJIS)
{
    return ::MultiByteToWideChar(CP_ACP, 0, sjis, sizeInWordsSJIS, nullptr, 0);
}


int SJIStoUTF16LE::Convert(const char* sjis, int sizeInWordsSJIS, wchar_t* utf16, int sizeInWordsUTF16)
{
    return ::MultiByteToWideChar(CP_ACP, 0, sjis, sizeInWordsSJIS, utf16, sizeInWordsUTF16);
}


std::wstring SJIStoUTF16LE::Convert(const char* sjis, int sizeInWordsSJIS)
{
    std::wstring utf16;

    // SJISからUTF16LEに変換したときの文字列長を求める。
    const int requiredSizeInWords = GetRequiredSizeInWords(sjis, sizeInWordsSJIS);
    if (requiredSizeInWords > 0)
    {
        utf16.resize(requiredSizeInWords);
        ::MultiByteToWideChar(CP_ACP, 0, sjis, sizeInWordsSJIS, utf16.data(), (int)utf16.size());
    }

    return utf16;
}


//---------------------------------------------------------------------------------------------------------------------------------------------
// SJISからUTF8に変換
//---------------------------------------------------------------------------------------------------------------------------------------------
int SJIStoUTF8::GetRequiredSizeInWords(const char* sjis, int sizeInWords)
{
    // SJIS ⇒ UTF16LE ⇒ UTF8
    const std::wstring utf16 = SJIStoUTF16LE::Convert(sjis, sizeInWords);
    return UTF16LEtoUTF8::GetRequiredSizeInWords(utf16.data(), (int)utf16.length());
}


int SJIStoUTF8::Convert(const char* sjis, int sizeInWordsSJIS, char* utf8, int sizeInWordsUTF8)
{
    // SJIS ⇒ UTF16LE ⇒ UTF8
    const std::wstring utf16 = SJIStoUTF16LE::Convert(sjis, sizeInWordsSJIS);
    return UTF16LEtoUTF8::Convert(utf16.data(), (int)utf16.length(), utf8, sizeInWordsUTF8);
}


std::string SJIStoUTF8::Convert(const char* sjis, int sizeInWordsSJIS)
{
    std::string utf8;

    // SJISからUTF8に変換したときの文字列長を求める。
    const int requiredSizeInWords = GetRequiredSizeInWords(sjis, sizeInWordsSJIS);
    if (requiredSizeInWords > 0)
    {
        utf8.resize(requiredSizeInWords);
        Convert(sjis, sizeInWordsSJIS, utf8.data(), (int)utf8.size());
    }

    return utf8;
}

