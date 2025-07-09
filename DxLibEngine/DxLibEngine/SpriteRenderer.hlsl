
// ���_�V�F�[�_�[�ɓ��͂����f�[�^�̍\����
struct VSInput
{
    float2 mpos : POSITION;
    float2 uv : TEXCOORD0;
};

// ���_�V�F�[�_�[����o�͂����f�[�^�̍\����
struct VSOutput
{
    float4 ppos : SV_POSITION;
    float2 uv : TEXCOORD0;
};

// �J�����p�̒萔�o�b�t�@
struct ConstantBufferForCamera
{
    matrix viewMatrix;
    matrix projectionMatrix;
};

// �I�u�W�F�N�g�p�̒萔�o�b�t�@
struct ConstantBufferForObject
{
    matrix worldMatrix;
    float4 spriteColor;
    float2 minUV;
    float2 maxUV;
    uint4 flipEnable;
};

// �萔�o�b�t�@�̃o�C���h
ConstantBuffer<ConstantBufferForCamera> cCamera : register(b0, space0);
ConstantBuffer<ConstantBufferForObject> cObject : register(b1, space0);

// ���_�V�F�[�_�[�̃G���g���[�|�C���g�֐�
VSOutput VSMain(VSInput input)
{
    // �X�v���C�g�́u���f����ԓ��ł̈ʒu�v��2�����̒l�Ȃ̂Ŗ������4�����Ɋg������B
    // ����́u4�s4��̍s��v�Ƃ̏�Z���s�����߂ɕK�v�ȏ����ł��B
    const float4 mpos = float4(input.mpos, 0, 1);
    
    // ���[���h��ԓ��ł̈ʒu(x,y,z,1) = ���f����ԓ��ł̈ʒu(x,y,z,1) �~ ���[���h�ϊ��s��
    const float4 wpos = mul(mpos, cObject.worldMatrix);

    // �r���[��ԓ��ł̈ʒu(x,y,z,1) = ���[���h��ԓ��ł̈ʒu(x,y,z,1) �~�r���[�ϊ��s��
    const float4 vpos = mul(wpos, cCamera.viewMatrix);
    
    // �v���W�F�N�V������ԓ��ł̈ʒu(x,y,z,w) = �r���[��ԓ��ł̈ʒu(x,y,z,1) �~ �v���W�F�N�V�����ϊ��s��
    const float4 ppos = mul(vpos, cCamera.projectionMatrix);
    
    float2 uv = input.uv;
    
    // ���E���]
    if(cObject.flipEnable.x)
    {
        uv.x = cObject.maxUV.x - uv.x + cObject.minUV.x;
    }
    
    // �㉺���]
    if(cObject.flipEnable.y)
    {
        uv.y = cObject.maxUV.y - uv.y + cObject.minUV.y;
    }
    
    uv.y = 1.0f - uv.y;

    // VSOutput�\���̌^�̕ϐ��Ɋi�[���Ă��� return ����
    VSOutput output;
    output.ppos = ppos;
    output.uv = uv;
    
    return output;
}

// �s�N�Z���V�F�[�_�[�ɓ��͂����f�[�^�̍\����
struct PSInput
{
    float4 ppos : SV_POSITION;
    float2 uv : TEXCOORD0;
};

// �s�N�Z���V�F�[�_�[����o�͂����f�[�^�̍\����
struct PSOutput
{
    float4 target0 : SV_TARGET0;
};

// �e�N�X�`��
Texture2D _MainTex : register(t0, space0);

// �T���v���[
SamplerState _MainTex_Sampler : register(s0, space0);

// �s�N�Z���V�F�[�_�[
PSOutput PSMain(PSInput input)
{
    // �e�N�X�`�����T���v�����O
    const float4 texelColor = _MainTex.Sample(_MainTex_Sampler, input.uv);
    
    // �����_�[�^�[�Q�b�g0�ɏo�͂���F = �e�N�Z���̐F �~ ��Ԃ��ꂽ���_�J���[
    const float4 target0 = texelColor * cObject.spriteColor;
    
    // ���l��0�ɋɂ߂ċ߂��ꍇ�͂��̃s�N�Z����j������
    if(target0.a < 0.001f)
    {
        discard;
    }
    
    // PSOutput�\���̌^�̕ϐ��Ɋi�[���Ă��� return ����
    PSOutput output;
    output.target0 = target0;
    return output;
}