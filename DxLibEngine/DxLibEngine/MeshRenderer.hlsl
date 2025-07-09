// カメラの情報（ビュー行列、プロジェクション行列）を格納する定数バッファ
cbuffer CameraConstants : register(b0)
{
    float4x4 viewMatrix;
    float4x4 projMatrix;
};

// オブジェクトごとの情報（ワールド変換行列）を格納する定数バッファ
cbuffer ObjectConstants : register(b1)
{
    float4x4 worldMatrix;
    float4 color; // オブジェクトの色
};

// 描画に使うテクスチャ
Texture2D mainTex : register(t0);

// テクスチャをどういう風にサンプリング（読み取り）するかを決める設定
SamplerState smp : register(s0);

// 頂点シェーダーの入力
struct VS_INPUT
{
    float4 position : POSITION; // 頂点座標 (ローカル空間)
    float3 normal : NORMAL;     // 法線
    float2 uv : TEXCOORD0;      // UV座標
};

// 頂点シェーダーの出力（ピクセルシェーダーへの入力）
struct PS_INPUT
{
    float4 position : SV_POSITION; // 頂点座標 (クリップ空間)
    float2 uv : TEXCOORD0;
};

// 頂点シェーダー
PS_INPUT VSMain(VS_INPUT input)
{
    PS_INPUT output = (PS_INPUT) 0;

    // ワールド、ビュー、プロジェクション行列を順番に乗算し、
    // ローカル座標からクリップ空間座標へ変換する
    float4 worldPos = mul(input.position, worldMatrix);
    float4 viewPos = mul(worldPos, viewMatrix);
    output.position = mul(viewPos, projMatrix);
    
    output.uv = input.uv;
    
    return output;
}

// ピクセルシェーダー
float4 PSMain(PS_INPUT input) : SV_TARGET
{
    // mainTex.Sample(smp, input.uv) で、指定されたUV座標の色を取得する
    // その色と、マテリアルに設定された色を掛け合わせることで、色味の調整も可能にする
    return mainTex.Sample(smp, input.uv) * color;
}