
// 頂点シェーダーに入力されるデータの構造体
struct VSInput
{
    float2 mpos : POSITION;
    float2 uv : TEXCOORD0;
};

// 頂点シェーダーから出力されるデータの構造体
struct VSOutput
{
    float4 ppos : SV_POSITION;
    float2 uv : TEXCOORD0;
};

// カメラ用の定数バッファ
struct ConstantBufferForCamera
{
    matrix viewMatrix;
    matrix projectionMatrix;
};

// オブジェクト用の定数バッファ
struct ConstantBufferForObject
{
    matrix worldMatrix;
    float4 spriteColor;
    float2 minUV;
    float2 maxUV;
    uint4 flipEnable;
};

// 定数バッファのバインド
ConstantBuffer<ConstantBufferForCamera> cCamera : register(b0, space0);
ConstantBuffer<ConstantBufferForObject> cObject : register(b1, space0);

// 頂点シェーダーのエントリーポイント関数
VSOutput VSMain(VSInput input)
{
    // スプライトの「モデル空間内での位置」は2次元の値なので無理やり4次元に拡張する。
    // これは「4行4列の行列」との乗算を行うために必要な処理です。
    const float4 mpos = float4(input.mpos, 0, 1);
    
    // ワールド空間内での位置(x,y,z,1) = モデル空間内での位置(x,y,z,1) × ワールド変換行列
    const float4 wpos = mul(mpos, cObject.worldMatrix);

    // ビュー空間内での位置(x,y,z,1) = ワールド空間内での位置(x,y,z,1) ×ビュー変換行列
    const float4 vpos = mul(wpos, cCamera.viewMatrix);
    
    // プロジェクション空間内での位置(x,y,z,w) = ビュー空間内での位置(x,y,z,1) × プロジェクション変換行列
    const float4 ppos = mul(vpos, cCamera.projectionMatrix);
    
    float2 uv = input.uv;
    
    // 左右反転
    if(cObject.flipEnable.x)
    {
        uv.x = cObject.maxUV.x - uv.x + cObject.minUV.x;
    }
    
    // 上下反転
    if(cObject.flipEnable.y)
    {
        uv.y = cObject.maxUV.y - uv.y + cObject.minUV.y;
    }
    
    uv.y = 1.0f - uv.y;

    // VSOutput構造体型の変数に格納してから return する
    VSOutput output;
    output.ppos = ppos;
    output.uv = uv;
    
    return output;
}

// ピクセルシェーダーに入力されるデータの構造体
struct PSInput
{
    float4 ppos : SV_POSITION;
    float2 uv : TEXCOORD0;
};

// ピクセルシェーダーから出力されるデータの構造体
struct PSOutput
{
    float4 target0 : SV_TARGET0;
};

// テクスチャ
Texture2D _MainTex : register(t0, space0);

// サンプラー
SamplerState _MainTex_Sampler : register(s0, space0);

// ピクセルシェーダー
PSOutput PSMain(PSInput input)
{
    // テクスチャをサンプリング
    const float4 texelColor = _MainTex.Sample(_MainTex_Sampler, input.uv);
    
    // レンダーターゲット0に出力する色 = テクセルの色 × 補間された頂点カラー
    const float4 target0 = texelColor * cObject.spriteColor;
    
    // α値が0に極めて近い場合はこのピクセルを破棄する
    if(target0.a < 0.001f)
    {
        discard;
    }
    
    // PSOutput構造体型の変数に格納してから return する
    PSOutput output;
    output.target0 = target0;
    return output;
}