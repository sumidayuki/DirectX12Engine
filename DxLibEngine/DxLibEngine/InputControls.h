#pragma once
#include "ButtonControl.h"


//「1軸」を表すコントロール
class AxisControl
{
private:
    float m_value;

public:
    // 値を設定します。
    void SetValue(float value) { m_value = value; }

    // 値を取得します。
    float GetValue() const { return m_value; }
};


//「2軸」を表すコントロール
class Vector2Control
{
private:
    AxisControl m_x;
    AxisControl m_y;

public:
    // 状態を更新します。
    void Update();

    // X軸コントロールを返します。
    const AxisControl& X() const { return m_x; }

    // Y軸コントロールを返します。
    const AxisControl& Y() const { return m_x; }
};


//「アナログスティック」を表すコントロール
class StickControl : public Vector2Control
{
private:
    ButtonControl m_down;
    ButtonControl m_right;
    ButtonControl m_left;
    ButtonControl m_up;

public:
    // スティックの状態を更新します。
    void Update(const Vector2& value);

    // スティックが下半分(-1.0 ～ 0.0)に入力されているかどうか
    const ButtonControl& Down() const { return m_down; }

    // スティックが右半分(0.0 ～ 1.0)に入力されているかどうか
    const ButtonControl& Right() const { return m_right; }

    // スティックが左半分(-1.0 ～ 0.0)に入力されているかどうか
    const ButtonControl& Left() const { return m_left; }

    // スティックが上半分(0.0 ～ 1.0)に入力されているかどうか
    const ButtonControl& Up() const { return m_up; }
};

