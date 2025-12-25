#ifndef CAMERA_HLSLI
#define CAMERA_HLSLI

#include "TypeMarshalling.hlsli"

// カメラに関する定数
struct CameraLayout
{
    matrix view; // ビュー変換行列
    matrix proj; // プロジェクション変換行列
    float3 position; // ワールド空間でのカメラの位置
    float padding0; // パディング
};

#endif // CAMERA_HLSLI

