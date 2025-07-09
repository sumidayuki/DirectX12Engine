// �J�����̏��i�r���[�s��A�v���W�F�N�V�����s��j���i�[����萔�o�b�t�@
cbuffer CameraConstants : register(b0)
{
    float4x4 viewMatrix;
    float4x4 projMatrix;
};

// �I�u�W�F�N�g���Ƃ̏��i���[���h�ϊ��s��j���i�[����萔�o�b�t�@
cbuffer ObjectConstants : register(b1)
{
    float4x4 worldMatrix;
    float4 color; // �I�u�W�F�N�g�̐F
};

// �`��Ɏg���e�N�X�`��
Texture2D mainTex : register(t0);

// �e�N�X�`�����ǂ��������ɃT���v�����O�i�ǂݎ��j���邩�����߂�ݒ�
SamplerState smp : register(s0);

// ���_�V�F�[�_�[�̓���
struct VS_INPUT
{
    float4 position : POSITION; // ���_���W (���[�J�����)
    float3 normal : NORMAL;     // �@��
    float2 uv : TEXCOORD0;      // UV���W
};

// ���_�V�F�[�_�[�̏o�́i�s�N�Z���V�F�[�_�[�ւ̓��́j
struct PS_INPUT
{
    float4 position : SV_POSITION; // ���_���W (�N���b�v���)
    float2 uv : TEXCOORD0;
};

// ���_�V�F�[�_�[
PS_INPUT VSMain(VS_INPUT input)
{
    PS_INPUT output = (PS_INPUT) 0;

    // ���[���h�A�r���[�A�v���W�F�N�V�����s������Ԃɏ�Z���A
    // ���[�J�����W����N���b�v��ԍ��W�֕ϊ�����
    float4 worldPos = mul(input.position, worldMatrix);
    float4 viewPos = mul(worldPos, viewMatrix);
    output.position = mul(viewPos, projMatrix);
    
    output.uv = input.uv;
    
    return output;
}

// �s�N�Z���V�F�[�_�[
float4 PSMain(PS_INPUT input) : SV_TARGET
{
    // mainTex.Sample(smp, input.uv) �ŁA�w�肳�ꂽUV���W�̐F���擾����
    // ���̐F�ƁA�}�e���A���ɐݒ肳�ꂽ�F���|�����킹�邱�ƂŁA�F���̒������\�ɂ���
    return mainTex.Sample(smp, input.uv) * color;
}