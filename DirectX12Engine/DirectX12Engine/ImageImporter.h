#pragma once
#include <windows.h>
#include "../External/DirectXTex/Include/DirectXTex.h"

class ImageImporter
{
public:
    struct SusiePlugin
    {
        using FunctionSusieProgress = int(__stdcall*)(int nNum, int nDenom, LONG_PTR lData);

        using FunctionIsSupported = bool(__stdcall*)(LPCSTR filename, void* dw);
        using FunctionGetPicture = int(__stdcall*)(LPCSTR buf, LONG_PTR len, unsigned int flag, HLOCAL* pHBInfo, HLOCAL* pHBm, FunctionSusieProgress lpPrgressCallback, LONG_PTR lData);
        FunctionIsSupported IsSupported;
        FunctionGetPicture GetPicture;

        using FunctionIsSupportedW = bool(__stdcall*)(LPCWSTR filename, void* dw);
        using FunctionGetPictureW = int(__stdcall*)(LPCWSTR buf, LONG_PTR len, unsigned int flag, HLOCAL* pHBInfo, HLOCAL* pHBm, FunctionSusieProgress lpPrgressCallback, LONG_PTR lData);
        FunctionIsSupportedW IsSupportedW;
        FunctionGetPictureW GetPictureW;

        std::string pluginPath;
    };

private:
    // Susueプラグインが格納されているディレクトリのパス文字列
    static inline std::vector<std::string> m_pluginDirectories;

    // Susueプラグイン
    static inline std::vector<SusiePlugin> m_plugins;

public:
    // デフォルトコンストラクタ
    ImageImporter();

    // デストラクタ
    ~ImageImporter();

    // Susueプラグインが格納されているディレクトリのパス文字列を追加します。
    static void AddPuginDirectory(const char* directoryPath);

    // Susueプラグインが再ロードします。
    static void ReloadPlugins();

    // 画像ファイルをインポートします。
    bool Import(const char* imagePath, DirectX::TexMetadata* imageMetadata, DirectX::ScratchImage& scratchImage) const;

    // 画像ファイルをインポートします。
    bool Import(const wchar_t* imagePath, DirectX::TexMetadata* imageMetadata, DirectX::ScratchImage& scratchImage) const;

private:
    // Susueプラグインを追加します。
    static void AddPlugin(const char* pluginPath);

    // 全てのSusueプラグインを削除します。
    static void UnloadAllPlugins();
};

