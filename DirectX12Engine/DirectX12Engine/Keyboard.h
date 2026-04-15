#pragma once

// キーボードからの入力を扱う静的クラス
class Keyboard
{
private:
    friend class InputManager;

    // ボタンの個数
    static constexpr int KeyCount = 256;

private:
    // キーボードのキーの状態を格納する配列
    static inline ButtonControl m_keys[KeyCount];

    // キーボードが接続されている場合は true
    static inline bool m_isConnected;

    // 何かキーが押されている場合は true
    static inline bool m_isPressedAnyKey;

private:
    // キーボード入力システムを初期化します。
    static void StaticConstructor();

    // キーボード入力システムを終了します。
    static void StaticDestructor();

    // キーボードの入力情報を更新します。
    static void Update();

public:
    // キーボードが接続されている場合は true を返します。
    static bool IsConnected() { return m_isConnected; }

    // 1つ以上のキーが押し下げられている時にはtrueを返します。
    static bool IsPressedAnyKey() { return m_isPressedAnyKey; }

    // キーの状態を取得します。
    static const ButtonControl& GetKeyState(KeyCode key);
};
