// �J�����̏��i�r���[�s��A�v���W�F�N�V�����s��j
cbuffer CameraConstants : register(b0)
{
    float4x4 viewMatrix;
    float4x4 projMatrix;
};

// �I�u�W�F�N�g���Ƃ̏��i���[���h�s��A�}�e���A���̐F�Ȃǁj
cbuffer ObjectConstants : register(b1)
{
    float4x4 worldMatrix;
    float4 diffuseColor;
    float4 specularColor;
};

// �e�N�X�`��
Texture2D diffuseMap : register(t0); // �f�B�t���[�Y�}�b�v
// Texture2D normalMap    : register(t1); // �@���}�b�v�i����͖��g�p�j
// Texture2D specularMap  : register(t2); // �X�y�L�����}�b�v�i����͖��g�p�j

// �T���v���[
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


// ---------------------------------
// ���_�V�F�[�_�[
// ---------------------------------
PS_INPUT VSMain(VS_INPUT input)
{
    PS_INPUT output = (PS_INPUT) 0;

    // ���[���h�A�r���[�A�v���W�F�N�V�����s������ɓK�p���āA���_���N���b�v��Ԃ֕ϊ�
    float4 worldPos = mul(float4(input.position, 1.0f), worldMatrix);
    output.position = mul(worldPos, viewMatrix);
    output.position = mul(output.position, projMatrix);
    
    // �s�N�Z���V�F�[�_�[�Ń��C�e�B���O�v�Z���s�����߁A�@���ƃ��[���h���W��n��
    output.normal = normalize(mul(float4(input.normal, 0.0f), worldMatrix).xyz);
    output.worldPos = worldPos.xyz;
    
    // UV���W�����̂܂ܓn��
    output.uv = input.uv;

    return output;
}

// ---------------------------------
// �s�N�Z���V�F�[�_�[
// ---------------------------------
float4 PSMain(PS_INPUT input) : SV_TARGET
{
    // �e�N�X�`������f�B�t���[�Y�J���[���擾
    float4 textureColor = diffuseMap.Sample(linearSampler, input.uv);

    // ���C�e�B���O�̉������i�w�������C�g�j
    float3 lightDir = normalize(float3(0.5f, -1.0f, -0.5f));
    float3 lightColor = float3(1.0f, 1.0f, 1.0f);
    
    // ����
    float3 ambient = float3(0.2f, 0.2f, 0.2f);
    
    // �g�U���ˌ�
    float diff = max(dot(-lightDir, input.normal), 0.0);
    float3 diffuse = diff * lightColor;
    
    // �ŏI�I�ȐF
    float3 finalColor = (ambient + diffuse) * textureColor.rgb * diffuseColor.rgb;
    
    return float4(finalColor, textureColor.a * diffuseColor.a);
}