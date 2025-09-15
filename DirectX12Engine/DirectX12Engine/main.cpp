//----------------------------------------------------------------------------------------
// 今回のエンジンでは "Entity-Component-System" を使用しています。
// それぞれのルールとして Entity は id しか持ちません。
// Component は必ず struct 等の情報だけを持ちます。（関数は絶対に書かない）
// System では world の中にある Entity から Viewクラスを使用して System で処理を
// 行いたい Component を持っている Entity を取得し、その Component に処理を行う
// システムになっています。
// 
// ECSを使用した理由としては、授業で制作しているエンジンがオブジェクト指向ベース
// のエンジンだったため、オブジェクト指向ではないデータ指向を使用した エンジンを
// 作ってみようと思い、実際の現場でも使用されることのあるECSを使用したエンジンを
// 作ろうと考えたからです。
//-----------------------------------------------------------------------------------------

// エントリーポイント
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) 
{
    // 作業ディレクトリをプロジェクトディレクトリに変更する
    wchar_t path[MAX_PATH];
    GetModuleFileNameW(NULL, path, MAX_PATH);
    wchar_t* last_slash = wcsrchr(path, L'\\');
    if (last_slash) *last_slash = L'\0'; // exe名除去
    last_slash = wcsrchr(path, L'\\');
    if (last_slash) *last_slash = L'\0'; // Debug/Release除去
    last_slash = wcsrchr(path, L'\\');
    if (last_slash) *last_slash = L'\0'; // x64除去
    std::wstring new_path = path;
    new_path += L"\\DirectX12Engine";
    SetCurrentDirectoryW(new_path.c_str());

    // 文字列処理時の地域(ロケール)の設定
    ::setlocale(LC_ALL, "japanese");

    // COM (Component Object Model) の初期化
    if (::CoInitializeEx(nullptr, COINIT_MULTITHREADED) < 0)
    {
        assert(0);
    }

    Screen::SetResolution(1920, 1080, false);

    Application::Run();

    // COMの終了処理
    ::CoUninitialize();

    return 0;
}