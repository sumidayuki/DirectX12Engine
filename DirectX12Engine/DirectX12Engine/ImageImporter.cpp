#include "ImageImporter.h"
#include "StringUtility.h"


ImageImporter::ImageImporter()
{
}


ImageImporter::~ImageImporter()
{
}


void ImageImporter::AddPuginDirectory(const char* directoryPath)
{
    m_pluginDirectories.push_back(directoryPath);
}


void ImageImporter::ReloadPlugins()
{
    UnloadAllPlugins();

    // .spi   Susie 32bit Plug - in
    // .sph   Susie 64bit(x64) Plug - in
    // .spha  Susie 64bit(ARM64) Plug - in
    // .PLG   Susie 16bit Plug - in
    for (const std::string& path : m_pluginDirectories)
    {
        const std::string withWildcard = path + "/*";

        // 検索開始
        WIN32_FIND_DATA findData;
        HANDLE hFind = ::FindFirstFileA(withWildcard.c_str(), &findData);

        // 見つからなかった
        if (hFind == INVALID_HANDLE_VALUE)
        {
            continue;
        }

        // ディレクトリではない
        if (!(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
        {
            ::FindClose(hFind);
            continue;
        }

        // ディレクトリ直下のファイルを列挙
        while (::FindNextFileA(hFind, &findData))
        {
            if (!(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
            {
                const char* lastPeriod = strrchr(findData.cFileName, '.');
                if (lastPeriod && (_stricmp(lastPeriod, ".sph") == 0))
                {
                    const std::string pluginPath = path + "/" + findData.cFileName;
                    AddPlugin(pluginPath.c_str());
                }
            }
        }

        ::FindClose(hFind);
    }
}


bool ImageImporter::Import(const char* imagePath, DirectX::TexMetadata* imageMetadata, DirectX::ScratchImage& scratchImage) const
{
    wchar_t wcs[MAX_PATH];
    //StringUtility::ConvertMBSToWCS(imagePath, -1, wcs, _countof(wcs));
    MBStoWCS::Convert(imagePath, -1, wcs, _countof(wcs));
    return Import(wcs, imageMetadata, scratchImage);
}


static int AlignUp(int value, int alignment)
{
    return (value + (alignment - 1)) & ~(alignment - 1);
}

static void Copy1BitPixels(DirectX::Image& image, const BITMAPINFO& info, const uint8_t* pixels) { assert(0); }
static void Copy2BitPixels(DirectX::Image& image, const BITMAPINFO& info, const uint8_t* pixels) { assert(0); }
static void Copy4BitPixels(DirectX::Image& image, const BITMAPINFO& info, const uint8_t* pixels) { assert(0); }

static void Copy8BitPixels(DirectX::Image& image, const BITMAPINFO& info, const uint8_t* pixels)
{
    const int rowPitch = AlignUp(info.bmiHeader.biWidth * 1, 4);
    for (int y = 0; y < info.bmiHeader.biHeight; y++)
    {
        const uint8_t* source = pixels + (info.bmiHeader.biHeight - y - 1) * rowPitch;
        uint8_t* destination = image.pixels + y * image.rowPitch;
        for (int x = 0; x < info.bmiHeader.biWidth; x++)
        {
            const uint8_t index = *source;
            destination[0] = info.bmiColors[index].rgbRed;
            destination[1] = info.bmiColors[index].rgbGreen;
            destination[2] = info.bmiColors[index].rgbBlue;
            destination[3] = 255;
            source += 1;
            destination += 4;
        }
    }
}


static void Copy16BitPixels(DirectX::Image& image, const BITMAPINFO& info, const uint8_t* pixels)
{
    assert(info.bmiHeader.biCompression != BI_RGB);

    const int rowPitch = AlignUp(info.bmiHeader.biWidth * 2, 4);
    for (int y = 0; y < info.bmiHeader.biHeight; y++)
    {
        const uint8_t* source = pixels + (info.bmiHeader.biHeight - y - 1) * rowPitch;
        uint8_t* destination = image.pixels + y * image.rowPitch;
        for (int x = 0; x < info.bmiHeader.biWidth; x++)
        {
            const uint16_t value = *(const uint16_t*)source;
            destination[0] = (uint8_t)((((value >> 10) & 0x1F) / 16.0f) * 255);
            destination[1] = (uint8_t)((((value >> 5) & 0x1F) / 16.0f) * 255);
            destination[2] = (uint8_t)((((value >> 0) & 0x1F) / 16.0f) * 255);
            destination[3] = 255;
            source += 2;
            destination += 4;
        }
    }
}


static void Copy24BitPixels(DirectX::Image& image, const BITMAPINFO& info, const uint8_t* pixels)
{
    const int rowPitch = AlignUp(info.bmiHeader.biWidth * 3, 4);
    for (int y = 0; y < info.bmiHeader.biHeight; y++)
    {
        const uint8_t* source = pixels + (info.bmiHeader.biHeight - y - 1) * rowPitch;
        uint8_t* destination = image.pixels + y * image.rowPitch;
        for (int x = 0; x < info.bmiHeader.biWidth; x++)
        {
            destination[0] = source[2];
            destination[1] = source[1];
            destination[2] = source[0];
            destination[3] = 255;
            source += 3;
            destination += 4;
        }
    }
}


static void Copy32BitPixels(DirectX::Image& image, const BITMAPINFO& info, const uint8_t* pixels)
{
    const int rowPitch = info.bmiHeader.biWidth * 4;
    for (int y = 0; y < info.bmiHeader.biHeight; y++)
    {
        const uint8_t* source = pixels + (info.bmiHeader.biHeight - y - 1) * rowPitch;
        uint8_t* destination = image.pixels + y * image.rowPitch;
        for (int x = 0; x < info.bmiHeader.biWidth; x++)
        {
            destination[0] = source[2];
            destination[1] = source[1];
            destination[2] = source[0];
            destination[3] = source[3];
            source += 4;
            destination += 4;
        }
    }
}


bool ImageImporter::Import(const wchar_t* imagePath, DirectX::TexMetadata* imageMetadata, DirectX::ScratchImage& scratchImage) const
{
    // ファイルを開く
    FILE* fp;
    if (_wfopen_s(&fp, imagePath, L"rb"))
    {
        return false;
    }

    // ファイルサイズを取得
    fseek(fp, 0, SEEK_END);
    const size_t fileSize = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    // ファイルの先頭から4KB分読み込む
    char buffer[4 * 1024];
    const size_t readSize = fileSize < sizeof(buffer) ? fileSize : sizeof(buffer);
    if (fread(buffer, 1, readSize, fp) <= 0)
    {
        fclose(fp);
        return false;
    }

    // ファイルを閉じる
    fclose(fp);

    // サポートしているプラグインを探す
    for (const SusiePlugin& plugin : m_plugins)
    {
        if (plugin.IsSupportedW(imagePath, buffer))
        {
            BITMAPINFO* bminfo;
            void* bitmap = nullptr;

            // 画像データを取得
            if (!plugin.GetPictureW(imagePath, 0, 0x00000000, (HANDLE*)&bminfo, (HANDLE*)&bitmap, nullptr, 0))
            {
                DirectX::Image image;
                image.width = bminfo->bmiHeader.biWidth;
                image.height = bminfo->bmiHeader.biHeight;
                image.format = DXGI_FORMAT_R8G8B8A8_UNORM;
                image.rowPitch = AlignUp(bminfo->bmiHeader.biWidth * 4, 4);
                image.slicePitch = image.rowPitch * bminfo->bmiHeader.biHeight;
                image.pixels = new uint8_t[image.rowPitch * image.height];

                switch (bminfo->bmiHeader.biBitCount)
                {
                case  1: Copy1BitPixels(image, *bminfo, (const uint8_t*)bitmap); break;
                case  2: Copy2BitPixels(image, *bminfo, (const uint8_t*)bitmap); break;
                case  4: Copy4BitPixels(image, *bminfo, (const uint8_t*)bitmap); break;
                case  8: Copy8BitPixels(image, *bminfo, (const uint8_t*)bitmap); break;
                case 16: Copy16BitPixels(image, *bminfo, (const uint8_t*)bitmap); break;
                case 24: Copy24BitPixels(image, *bminfo, (const uint8_t*)bitmap); break;
                case 32: Copy32BitPixels(image, *bminfo, (const uint8_t*)bitmap); break;
                default: assert(0); break;
                }

                if (bminfo) LocalFree(bminfo);
                if (bitmap) LocalFree(bitmap);

                if (FAILED(scratchImage.InitializeFromImage(image, false, DirectX::CP_FLAGS_NONE)))
                {
                    assert(0);
                }

                if (imageMetadata)
                {
                    *imageMetadata = scratchImage.GetMetadata();
                }
            }

            return true;
        }
    }

    return false;
}


void ImageImporter::AddPlugin(const char* pluginPath)
{
    // DLLをロード
    HMODULE hModule = ::LoadLibraryExA(pluginPath, nullptr, 0);
    if (!hModule)
    {
        return;
    }

    // 関数ポインタを取得
    SusiePlugin susiePlugin;
    susiePlugin.pluginPath = pluginPath;
    susiePlugin.IsSupported = (SusiePlugin::FunctionIsSupported)GetProcAddress(hModule, "IsSupported");
    susiePlugin.GetPicture = (SusiePlugin::FunctionGetPicture)GetProcAddress(hModule, "GetPicture");
    susiePlugin.IsSupportedW = (SusiePlugin::FunctionIsSupportedW)GetProcAddress(hModule, "IsSupportedW");
    susiePlugin.GetPictureW = (SusiePlugin::FunctionGetPictureW)GetProcAddress(hModule, "GetPictureW");

    if (!susiePlugin.IsSupported || !susiePlugin.GetPicture || !susiePlugin.IsSupportedW || !susiePlugin.GetPictureW)
    {
        ::FreeLibrary(hModule);
        return;
    }

    m_plugins.push_back(susiePlugin);
}


void ImageImporter::UnloadAllPlugins()
{
    // Susueプラグインをクリア
    m_plugins.clear();
}

