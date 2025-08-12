
// Light�\���́iC++��Light�\���̂ƈ�v�����Ă���j
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

// �J�����̏��i�r���[�s��A�v���W�F�N�V�����s��j
cbuffer CameraConstants : register(b0)
{
    float4x4 viewMatrix;
    float4x4 projMatrix;
};

// �I�u�W�F�N�g���Ƃ̏��i���[���h�s��A�}�e���A���̐F�Ȃǁj
cbuffer ObjectConstants : register(b1)
{
    ObjectConstantsLayout objectData;
};

// �V�[���S�̂̏���ǉ�
cbuffer SceneConstants : register(b2)
{
    int activeLightCount;
    float3 pad1;
    float4 cameraWorldPosition;
};

//�@���C�g�����\�����o�b�t�@�Ƃ��Ď󂯎��
StructuredBuffer<Light> lights : register(t3); // C++�Őݒ肵�����W�X�^�ԍ�

// �e�N�X�`���ƃT���v���[�̒�`
Texture2D diffuseMap : register(t0);
SamplerState linearSampler : register(s0);

// ���_�V�F�[�_�[�ւ̓���
struct VS_INPUT
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD0;
    float3 tangent : TANGENT;
};

// �s�N�Z���V�F�[�_�[�ւ̓���
struct PS_INPUT
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
    float3 normal : NORMAL;
    float3 worldPos : TEXCOORD1;
};


// ���_�V�F�[�_�[
PS_INPUT VSMain(VS_INPUT input)
{
    PS_INPUT output = (PS_INPUT) 0;

    // ���[���h�A�r���[�A�v���W�F�N�V�����s������ɓK�p���āA���_���N���b�v��Ԃ֕ϊ�
    float4 worldPos = mul(float4(input.position, 1.0f), objectData.worldMatrix);
    output.position = mul(worldPos, viewMatrix);
    output.position = mul(output.position, projMatrix);
    
    // �s�N�Z���V�F�[�_�[�Ń��C�e�B���O�v�Z���s�����߁A�@���ƃ��[���h���W��n��
    output.normal = normalize(mul(float4(input.normal, 0.0f), objectData.worldMatrix).xyz);
    output.worldPos = worldPos.xyz;
    
    // UV���W�����̂܂ܓn��
    output.uv = input.uv;

    return output;
}

// ���C�e�B���O�v�Z�̃w���p�[�֐�
void CalculateLight(Light light, ObjectConstantsLayout obj, float3 pixelPos, float3 normal, float3 viewDir,
                    out float3 outDiffuse, out float3 outSpecular)
{
    outDiffuse = float3(0, 0, 0);
    outSpecular = float3(0, 0, 0);

    float3 lightDir;
    float attenuation = 1.0;

    // ���C�g�̎�ނɂ���Čv�Z�𕪊�
    switch (light.type)
    {
        case 0: // Directional Light
        {
            lightDir = normalize(-light.direction);
            // �����͂Ȃ�
            break;
        }
        case 1: // Point Light
        {
            float3 toLightVec = light.position - pixelPos;
            float dist = length(toLightVec);
            
            // ���͈̔͊O�Ȃ�A�����ŏ������I��
            if (dist > light.range)
                    return;

            lightDir = normalize(toLightVec);
            
            // �����ɂ�錸�� (�͈͂̒[�Ŋ��炩��0�ɂȂ�)
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
            
            // �����ɂ�錸��
            float distFactor = 1.0 - smoothstep(light.range * 0.75, light.range, dist);
            attenuation = distFactor * distFactor;
            
            // �X�|�b�g���C�g�̉~���ɂ�錸��
            float spotFactor = dot(-lightDir, normalize(light.direction));
            float spotAttenuation = smoothstep(light.spotAngle, light.spotAngle + 0.05, spotFactor);
            attenuation *= spotAttenuation;
            break;
        }
    }
    
    // �����͂��Ȃ���Ώ������I��
    if (attenuation <= 0.001) return;

    // ���ʂ̃��C�e�B���O�v�Z
    // Diffuse
    float diff = saturate(dot(normal, lightDir));
    outDiffuse = diff * light.color.rgb * obj.diffuseColor.rgb;

    // Specular (Blinn-Phong)
    float3 halfVec = normalize(lightDir + viewDir);
    float spec = pow(saturate(dot(normal, halfVec)), obj.shininess);
    outSpecular = spec * light.color.rgb * obj.specularColor.rgb;
    
    // �v�Z���ʂɌ�����K�p
    outDiffuse *= attenuation;
    outSpecular *= attenuation;
}

// �s�N�Z���V�F�[�_�[
float4 PSMain(PS_INPUT input) : SV_TARGET
{
    // �e�N�X�`���J���[���擾
    float4 textureColor = diffuseMap.Sample(linearSampler, input.uv);
    
    // �@���E�����x�N�g���𐳋K��
    input.normal = normalize(input.normal);
    float3 viewDir = normalize(cameraWorldPosition.xyz - input.worldPos);

    float3 totalDiffuse = float3(0, 0, 0);
    float3 totalSpecular = float3(0, 0, 0);

    // �S���C�g�ɂ��� CalculateLight ���Ăяo���A�g�U�E���ʐ��������v
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