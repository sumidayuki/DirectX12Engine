// カメラの情報（ビュー行列、プロジェクション行列）
cbuffer CameraConstants : register(b0)
{
    float4x4 viewMatrix;
    float4x4 projMatrix;
};

// オブジェクトごとの情報（ワールド行列、マテリアルの色など）
cbuffer ObjectConstants : register(b1)
{
    float4x4 worldMatrix;
    float4 diffuseColor;
    float4 specularColor;
};

// テクスチャ
Texture2D diffuseMap : register(t0); // ディフューズマップ
// Texture2D normalMap    : register(t1); // 法線マップ（今回は未使用）
// Texture2D specularMap  : register(t2); // スペキュラマップ（今回は未使用）

// サンプラー
SamplerState linearSampler : register(s0);

// 頂点シェーダーへの入力
struct VS_INPUT
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD0;
    float3 tangent : TANGENT;
};

// ピクセルシェーダーへの入力
struct PS_INPUT
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
    float3 normal : NORMAL;
    float3 worldPos : TEXCOORD1;
};


// ---------------------------------
// 頂点シェーダー
// ---------------------------------
PS_INPUT VSMain(VS_INPUT input)
{
    PS_INPUT output = (PS_INPUT) 0;

    // ワールド、ビュー、プロジェクション行列を順に適用して、頂点をクリップ空間へ変換
    float4 worldPos = mul(float4(input.position, 1.0f), worldMatrix);
    output.position = mul(worldPos, viewMatrix);
    output.position = mul(output.position, projMatrix);
    
    // ピクセルシェーダーでライティング計算を行うため、法線とワールド座標を渡す
    output.normal = normalize(mul(float4(input.normal, 0.0f), worldMatrix).xyz);
    output.worldPos = worldPos.xyz;
    
    // UV座標をそのまま渡す
    output.uv = input.uv;

    return output;
}

// ---------------------------------
// ピクセルシェーダー
// ---------------------------------
float4 PSMain(PS_INPUT input) : SV_TARGET
{
    // テクスチャからディフューズカラーを取得
    float4 textureColor = diffuseMap.Sample(linearSampler, input.uv);

    // ライティングの仮実装（指向性ライト）
    float3 lightDir = normalize(float3(0.5f, -1.0f, -0.5f));
    float3 lightColor = float3(1.0f, 1.0f, 1.0f);
    
    // 環境光
    float3 ambient = float3(0.2f, 0.2f, 0.2f);
    
    // 拡散反射光
    float diff = max(dot(-lightDir, input.normal), 0.0);
    float3 diffuse = diff * lightColor;
    
    // 最終的な色
    float3 finalColor = (ambient + diffuse) * textureColor.rgb * diffuseColor.rgb;
    
    return float4(finalColor, textureColor.a * diffuseColor.a);
}