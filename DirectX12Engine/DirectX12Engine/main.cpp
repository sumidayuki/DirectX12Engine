//----------------------------------------------------------------------------------------
// 今回のエンジンでは "Entity-Component-System" を使用しています。
// それぞれのルールとして Entity は id しか持ちません。
// Component は必ず struct 等の情報だけを持ちます。（関数は絶対に書かない）
// System では world の中にある Entity から Viewクラスを使用して System で処理を
// 行いたい Component を持っている Entity を取得し、その Component に処理を行う
// システムになっています。
// 
// ECSを使用した理由としては、授業で制作しているエンジンがオブジェクト指向ベース
// のエンジンだったため、オブジェクト指向ではないコンポーネント指向を使用した
// エンジンを作ってみようと思い、実際の現場でも使用されることのあるECSを使用したエンジン
// を作ろうと考えたからです。
//-----------------------------------------------------------------------------------------

// エントリーポイント
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) 
{
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