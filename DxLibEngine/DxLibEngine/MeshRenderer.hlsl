
// Light構造体（C++のLight構造体と一致させている）
struct Light
{
    float4 color;
    float3 position;
    float range;
    float3 direction;
    float spotAngle;
    uint type; // LightType
    int enabled;
    float2 padding;
};

struct ObjectConstantsLayout
{
    float4x4 worldMatrix;
    float4 diffuseColor;
    float4 specularColor;
    float shininess;
};

// カメラの情報（ビュー行列、プロジェクション行列）
cbuffer CameraConstants : register(b0)
{
    float4x4 viewMatrix;
    float4x4 projMatrix;
};

// オブジェクトごとの情報（ワールド行列、マテリアルの色など）
cbuffer ObjectConstants : register(b1)
{
    ObjectConstantsLayout objectData;
};

// シーン全体の情報を追加
cbuffer SceneConstants : register(b2)
{
    int activeLightCount;
    float3 pad1;
    float4 cameraWorldPosition;
};

//　ライト情報を構造化バッファとして受け取る
StructuredBuffer<Light> lights : register(t3); // C++で設定したレジスタ番号

// テクスチャとサンプラーの定義
Texture2D diffuseMap : register(t0);
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


// 頂点シェーダー
PS_INPUT VSMain(VS_INPUT input)
{
    PS_INPUT output = (PS_INPUT) 0;

    // ワールド、ビュー、プロジェクション行列を順に適用して、頂点をクリップ空間へ変換
    float4 worldPos = mul(float4(input.position, 1.0f), objectData.worldMatrix);
    output.position = mul(worldPos, viewMatrix);
    output.position = mul(output.position, projMatrix);
    
    // ピクセルシェーダーでライティング計算を行うため、法線とワールド座標を渡す
    output.normal = normalize(mul(float4(input.normal, 0.0f), objectData.worldMatrix).xyz);
    output.worldPos = worldPos.xyz;
    
    // UV座標をそのまま渡す
    output.uv = input.uv;

    return output;
}

// ライティング計算のヘルパー関数
void CalculateLight(Light light, ObjectConstantsLayout obj, float3 pixelPos, float3 normal, float3 viewDir,
                    out float3 outDiffuse, out float3 outSpecular)
{
    outDiffuse = float3(0, 0, 0);
    outSpecular = float3(0, 0, 0);

    float3 lightDir;
    float attenuation = 1.0;

    // ライトの種類によって計算を分岐
    switch (light.type)
    {
        case 0: // Directional Light
        {
            lightDir = normalize(-light.direction);
            // 減衰はなし
            break;
        }
        case 1: // Point Light
        {
            float3 toLightVec = light.position - pixelPos;
            float dist = length(toLightVec);
            
            // 光の範囲外なら、ここで処理を終了
            if (dist > light.range)
                    return;

            lightDir = normalize(toLightVec);
            
            // 距離による減衰 (範囲の端で滑らかに0になる)
            float distFactor = 1.0 - smoothstep(light.range * 0.75, light.range, dist);
            attenuation = distFactor * distFactor;
            break;
        }
        case 2: // Spot Light
        {
            float3 toLightVec = light.position - pixelPos;
            float dist = length(toLightVec);

            if (dist > light.range)
            return;
            
            lightDir = normalize(toLightVec);
            
            // 距離による減衰
            float distFactor = 1.0 - smoothstep(light.range * 0.75, light.range, dist);
            attenuation = distFactor * distFactor;
            
            // スポットライトの円錐による減衰
            float spotFactor = dot(-lightDir, normalize(light.direction));
            float spotAttenuation = smoothstep(light.spotAngle, light.spotAngle + 0.05, spotFactor);
            attenuation *= spotAttenuation;
            break;
        }
    }
    
    // 光が届かなければ処理を終了
    if (attenuation <= 0.001) return;

    // 共通のライティング計算
    // Diffuse
    float diff = saturate(dot(normal, lightDir));
    outDiffuse = diff * light.color.rgb * obj.diffuseColor.rgb;

    // Specular (Blinn-Phong)
    float3 halfVec = normalize(lightDir + viewDir);
    float spec = pow(saturate(dot(normal, halfVec)), obj.shininess);
    outSpecular = spec * light.color.rgb * obj.specularColor.rgb;
    
    // 計算結果に減衰を適用
    outDiffuse *= attenuation;
    outSpecular *= attenuation;
}

// ピクセルシェーダー
float4 PSMain(PS_INPUT input) : SV_TARGET
{
    // テクスチャカラーを取得
    float4 textureColor = diffuseMap.Sample(linearSampler, input.uv);
    
    // 法線・視線ベクトルを正規化
    input.normal = normalize(input.normal);
    float3 viewDir = normalize(cameraWorldPosition.xyz - input.worldPos);

    float3 totalDiffuse = float3(0, 0, 0);
    float3 totalSpecular = float3(0, 0, 0);

    // 全ライトについて CalculateLight を呼び出し、拡散・鏡面成分を合計
    for (int i = 0; i < activeLightCount; ++i)
    {
        float3 currentDiffuse, currentSpecular;
        CalculateLight(lights[i], objectData, input.worldPos, input.normal, viewDir, currentDiffuse, currentSpecular);
        totalDiffuse += currentDiffuse;
        totalSpecular += currentSpecular;
    }

    float3 ambient = float3(0.1, 0.1, 0.1) * objectData.diffuseColor.rgb;
    float3 finalColor = (ambient + totalDiffuse) * textureColor.rgb + totalSpecular;
    
    return float4(finalColor, textureColor.a * objectData.diffuseColor.a);
}