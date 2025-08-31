#pragma once

//---------------------------------------------------------------------------------------------------------------------------------------------
// �J���[
// 
//  �E�u���̎O���F(RGB)�v��p�����R���s���[�^�[�����ł̐F�̕\�����@�B
//  
//  �E�uR�v�͐ԐF������ 0.0 �` 1.0 �̒l�Ŏw�肷��B
//  �E�uG�v�͗ΐF������ 0.0 �` 1.0 �̒l�Ŏw�肷��B
//  �E�uB�v�͐F������ 0.0 �` 1.0 �̒l�Ŏw�肷��B
//  �E�uA�v��  �������� 0.0 �` 1.0 �̒l�Ŏw�肷��B
//  
//  �E�������͈�ʓI�Ɂu�s�����x�v�Ƃ��ė��p����邪�A����ȊO�̗p�r�Ŏg�p����邱�Ƃ�����B
//
//---------------------------------------------------------------------------------------------------------------------------------------------
class Color
{
public:
    // �悭�m��ꂽ�F (Well-known colors)
    static const Color clear;                       // �N���A:����(0, 0, 0, 0)
    static const Color water;                       // �E�H�[�^�[:���F
    static const Color aliceBlue;                   // �A���X�u���[(240, 248, 255, 255)
    static const Color antiqueWhite;                // �A���e�B�[�N�z���C�g(250, 235, 215, 255)
    static const Color aqua;                        // �A�N�A:����K (255, 0, 255, 255)
    static const Color aquamarine;                  // �A�N�A�}����:����,�� (127, 255, 212, 255)
    static const Color azure;                       // �A�W���[��:���� (255, 240, 255, 255)
    static const Color beige;                       // �x�[�W��:���� (220, 245, 245, 255)
    static const Color bisque;                      // �׃X�N (255, 228, 196, 255)
    static const Color black;                       // �u���b�N:�� (0, 0, 0, 255)
    static const Color blanchedAlmond;              // �u�����`�h�A�[�����h (255, 235, 205, 255)
    static const Color blue;                        // �u���[:�� (255, 0, 0, 255)
    static const Color blueViolet;                  // �u���[�o�C�I���b�g:��� (138, 43, 226, 255)
    static const Color brown;                       // �u���E��:�� (165, 42, 42, 255)
    static const Color burlyWood;                   // �o�[���[�E�b�h:���� (222, 184, 135, 255)
    static const Color cadetBlue;                   // �J�f�b�g�u���[:���h (95, 158, 160, 255)
    static const Color chartreuse;                  // �V�����g���[�Y:������ (127, 255, 0, 255)
    static const Color chocolate;                   // �`���R���[�g (210, 105, 30, 255)
    static const Color coral;                       // �R�[����:�X�� (255, 127, 80, 255)
    static const Color cornflowerBlue;              // �R�[���t�����[�u���[:��ԋe (100, 149, 237, 255)
    static const Color cornsilk;                    // �R�[���V���N:������ (255, 248, 220, 255)
    static const Color crimson;                     // �N�����]��:�[�g (220, 20, 60, 255)
    static const Color cyan;                        // �V�A��:�� (255, 0, 255, 255)
    static const Color darkBlue;                    // �_�[�N�u���[:�Ð� (0, 0, 139, 255)
    static const Color darkCyan;                    // �_�[�N�V�A��:�Ð� (0, 139, 139, 255)
    static const Color darkGoldenrod;               // �_�[�N�S�[���f�����b�h:�ÏH�i�ّ� (184, 134, 11, 255)
    static const Color darkGray;                    // �_�[�N�O���C:�ÊD (169, 169, 169, 255)
    static const Color darkGreen;                   // �_�[�N�O���[��:�×� (0, 100, 0, 255)
    static const Color darkKhaki;                   // �_�[�N�J�[�L (189, 183, 107, 255)
    static const Color darkMagenta;                 // �_�[�N�}�[���^:�Î��g (139, 0, 139, 255)
    static const Color darkOliveGreen;              // �_�[�N�I���[�u�O���[��:�É��� (85, 107, 47, 255)
    static const Color darkOrange;                  // �_�[�N�I�����W (255, 140, 0, 255)
    static const Color darkOrchid;                  // �_�[�N�I�[�L�b�h:�Î� (153, 50, 204, 255)
    static const Color darkRed;                     // �_�[�N���b�h:�Ð� (139, 0, 0, 255)
    static const Color darkSalmon;                  // �_�[�N�T�[����:�Í� (233, 150, 122, 255)
    static const Color darkSeaGreen;                // �_�[�N�V�[�O���[��:�ÊC�� (143, 188, 139, 255)
    static const Color darkSlateBlue;               // �_�[�N�X���[�g�u���[:�Ð� (72, 61, 139, 255)
    static const Color darkSlateGray;               // �_�[�N�X���[�g�O���[��:�ÊD�� (47, 79, 79, 255)
    static const Color darkTurquoise;               // �_�[�N�^�[�R�C�Y:�Ð� (0, 206, 209, 255)
    static const Color darkViolet;                  // �_�[�N�o�C�I���b�g:��� (148, 0, 211, 255)
    static const Color deepPink;                    // �f�B�[�v�s���N:�[�� (255, 20, 147, 255)
    static const Color deepSkyBlue;                 // �f�B�[�v�X�J�C�u���[:�[�� (0, 191, 255, 255)
    static const Color dimGray;                     // �f�B���O���[:���D (105, 105, 105, 255)
    static const Color dodgerBlue;                  // �h�b�W���[�u���[ (30, 144, 255, 255)
    static const Color firebrick;                   // �t�@�C���[�u���b�N:�ωΗ��� (178, 34, 34, 255)
    static const Color floralWhite;                 // �t���[�����z���C�g:�Ԕ� (255, 250, 240, 255)
    static const Color forestGreen;                 // �t�H���X�g�O���[��:�[�� (34, 139, 34, 255)
    static const Color fuchsia;                     // �t�N�V��:�Ԏ� (255, 255, 0, 255)
    static const Color gainsboro;                   // �Q�C���X�{��:�W�D (220, 220, 220, 255)
    static const Color ghostWhite;                  // �S�[�X�g�z���C�g:�ɔ� (255, 248, 248, 255)
    static const Color gold;                        // �S�[���h:�� (255, 215, 0, 255)
    static const Color goldenrod;                   // �S�[���f�����b�h:�H�i�ّ� (218, 165, 32, 255)
    static const Color gray;                        // �O���[:�D (128, 128, 128, 255)
    static const Color green;                       // �O���[��:�� (0, 255, 0, 255)
    static const Color greenYellow;                 // �O���[���C�G���[:�Ή� (173, 255, 47, 255)
    static const Color honeydew;                    // �n�j�[�f���[:�ØI (240, 255, 240, 255)
    static const Color hotPink;                     // �z�b�g�s���N:���� (255, 105, 180, 255)
    static const Color indianRed;                   // �C���f�B�A�����b�h (205, 92, 92, 255)
    static const Color indigo;                      // �C���f�B�S:�� (75, 0, 130, 255)
    static const Color ivory;                       // �A�C�{���[:�ۉ� (255, 255, 240, 255)
    static const Color khaki;                       // �J�[�L:�y�� (240, 230, 140, 255)
    static const Color lavender;                    // ���x���_�[:�� (230, 230, 250, 255)
    static const Color lavenderBlush;               // ���x���_�[�u���b�V��:���g (255, 240, 245, 255)
    static const Color lawnGreen;                   // ���[���O���[��:�ŗ� (124, 252, 0, 255)
    static const Color lemonChiffon;                // �������V�t�H��:���E�G (255, 250, 205, 255)
    static const Color lightBlue;                   // ���C�g�u���[:������ (173, 216, 230, 255)
    static const Color lightCoral;                  // ���C�g�R�[����:�����X�� (240, 128, 128, 255)
    static const Color lightCyan;                   // ���C�g�V�A��:������ (255, 224, 255, 255)
    static const Color lightGoldenrodYellow;        // ���C�g�S�[���f�����b�h�C�G���[:������ (250, 250, 210, 255)
    static const Color lightGray;                   // ���C�g�O���[:�����D (211, 211, 211, 255)
    static const Color lightGreen;                  // ���C�g�O���[��:������ (144, 238, 144, 255)
    static const Color lightPink;                   // ���C�g�s���N:������ (255, 182, 193, 255)
    static const Color lightSalmon;                 // ���C�g�T�[����:������ (255, 160, 122, 255)
    static const Color lightSeaGreen;               // ���C�g�V�[�O���[��:�����C�� (32, 178, 170, 255)
    static const Color lightSkyBlue;                // ���C�g�X�J�C�u���[:������ (135, 206, 250, 255)
    static const Color lightSlateGray;              // ���C�g�X���[�g�O���[:�����D�� (119, 136, 153, 255)
    static const Color lightSteelBlue;              // ���C�g�X�`�[���u���[:�����| (176, 196, 222, 255)
    static const Color lightYellow;                 // ���C�g�C�G���[:������ (255, 255, 224, 255)
    static const Color lime;                        // ���C�� (0, 255, 0, 255)
    static const Color limeGreen;                   // ���C���O���[�� (50, 205, 50, 255)
    static const Color linen;                       // ���j��:���� (250, 240, 230, 255)
    static const Color magenta;                     // �}�[���^:���g (255, 255, 0, 255)
    static const Color maroon;                      // �}����:�I,������,�C�V�� (128, 0, 0, 255)
    static const Color mediumAquamarine;            // ���f�B�A���A�N�A�}���� (102, 205, 170, 255)
    static const Color mediumBlue;                  // ���f�B�A���u���[ (0, 0, 205, 255)
    static const Color mediumOrchid;                // ���f�B�A���I�[�L�b�h (186, 85, 211, 255)
    static const Color mediumPurple;                // ���f�B�A���p�[�v�� (147, 112, 219, 255)
    static const Color mediumSeaGreen;              // ���f�B�A���V�[�O���[�� (60, 179, 113, 255)
    static const Color mediumSlateBlue;             // ���f�B�A���X���[�g�u���[ (123, 104, 238, 255)
    static const Color mediumSpringGreen;           // ���f�B�A���X�v�����O�O���[�� (0, 250, 154, 255)
    static const Color mediumTurquoise;             // ���f�B�A���g�[�R�C�Y (72, 209, 204, 255)
    static const Color mediumVioletRed;             // ���f�B�A���o�C�I���b�g���b�h (199, 21, 133, 255)
    static const Color midnightBlue;                // �~�b�h�i�C�g�u���[:�^��� (25, 25, 112, 255)
    static const Color mintCream;                   // �~���g�N���[�� (245, 255, 250, 255)
    static const Color mistyRose;                   // �~�X�e�B�[���[�Y:���K�N (255, 228, 225, 255)
    static const Color moccasin;                    // ���J�V�� (255, 228, 181, 255)
    static const Color navajoWhite;                 // �i�o�z�z���C�g (255, 222, 173, 255)
    static const Color navy;                        // �l�C�r�[ (0, 0, 128, 255)
    static const Color oldLace;                     // �I�[���h���C�X (253, 245, 230, 255)
    static const Color olive;                       // �I���[�u:����z (128, 128, 0, 255)
    static const Color oliveDrab;                   // �I���[�u�h���u�@(107, 142, 35, 255)
    static const Color orange;                      // �I�����W:�� (255, 165, 0, 255)
    static const Color orangeRed;                   // �I�����W���b�h:�Ԟ� (255, 69, 0, 255)
    static const Color orchid;                      // �I�[�L�b�h:�� (218, 112, 214, 255)
    static const Color paleGoldenrod;               // �y�[���S�[���f�����b�h:������ (238, 232, 170, 255)
    static const Color paleGreen;                   // �y�[���O���[��:���� (152, 251, 152, 255)
    static const Color paleTurquoise;               // �y�[���^�[�R�C�Y:���� (175, 238, 238, 255)
    static const Color paleVioletRed;               // �y�[���o�C�I���b�g���b�h:����� (219, 112, 147, 255)
    static const Color papayaWhip;                  // �p�p�C���z�C�b�v (255, 239, 213, 255)
    static const Color peachPuff;                   // �s�[�`�p�t (255, 218, 185, 255)
    static const Color peru;                        // �y���[ (205, 133, 63, 255) 
    static const Color pink;                        // �s���N:���q (255, 192, 203, 255) 
    static const Color plum;                        // �v����:���m�� (221, 160, 221, 255)
    static const Color powderBlue;                  // �p�E�_�[�u���[:�W�� (176, 224, 230, 255)
    static const Color purple;                      // �p�[�v��:�� (128, 0, 128, 255)
    static const Color red;                         // ���b�h:�� (255, 0, 0, 255)
    static const Color rosyBrown;                   // ���[�W�[�u���E��:�K�N�� (188, 143, 143, 255)
    static const Color royalBlue;                   // ���C�����u���[:���� (65, 105, 225, 255)
    static const Color saddleBrown;                 // �T�h���u���E��:�n�� (139, 69, 19, 255)
    static const Color salmon;                      // �T�[����:�� (250, 128, 114, 255)
    static const Color sandyBrown;                  // �T���f�B�[�u���E��:���� (244, 164, 96, 255)
    static const Color seaGreen;                    // �V�[�O���[��:�C�� (46, 139, 87, 255)
    static const Color seaShell;                    // �V�[�V�F��:�L�k (255, 245, 238, 255)
    static const Color sienna;                      // �V�G���i (160, 82, 45, 255)
    static const Color silver;                      // �V���o�[:�� (192, 192, 192, 255)
    static const Color skyBlue;                     // �X�J�C�u���[:�� (135, 206, 235, 255)
    static const Color slateBlue;                   // �X���[�g�u���[:�Ί� (106, 90, 205, 255)
    static const Color slateGray;                   // �X���[�g�O���[:�D�Δ� (112, 128, 144, 255)
    static const Color snow;                        // �X�m�[:�� (255, 250, 250, 255)
    static const Color springGreen;                 // �X�v�����O�O���[��:�t�� (0, 255, 127, 255)
    static const Color steelBlue;                   // �X�`�[���u���[:�| (70, 130, 180, 255)
    static const Color tan;                         // �^��:���Ă� (210, 180, 140, 255)
    static const Color teal;                        // �e�B�[��:�� (0, 128, 128, 255)
    static const Color thistle;                     // �V�X��:�H (216, 191, 216, 255)
    static const Color tomato;                      // �g�}�g (255, 99, 71, 255)
    static const Color transparentBlack;            // �g�����X�y�A�����g�u���b�N:���ߍ� (0, 0, 0, 0)
    static const Color transparentWhite;            // �g�����X�y�A�����g�z���C�g:���ߔ� (255, 255, 255, 0)
    static const Color turquoise;                   // �^�[�R�C�Y (64, 224, 208, 255)
    static const Color violet;                      // �o�C�I���b�g:� (238, 130, 238, 255)
    static const Color wheat;                       // �t�B�[�g:���� (245, 222, 179, 255)
    static const Color white;                       // �z���C�g:�� (255, 255, 255, 255)
    static const Color whiteSmoke;                  // �z���C�g�X���[�N:���� (245, 245, 245, 255)
    static const Color yellow;                      // �C�G���[:�� (255, 255, 0, 255)
    static const Color yellowGreen;                 // �C�G���[�O���[��:���� (154, 205, 50, 255)

public:
    union
    {
        // �����\��
        struct
        {
            float r;    // �Ԑ���
            float g;    // �ΐ���
            float b;    // ����
            float a;    // ������
        };

        // �z��\��
        float components[4];
    };

public:
    // �e����32�r�b�g���������_���l�ō\�����ꂽRGBA�`���̐V�����F���쐬���܂��B
    // (�e�����̒l�͏���������܂���)
    Color() = default;

    // RGB�̒l���w�肵��RGBA�`���̐V�����F���쐬���܂��B
    // (A�̒l��1.0�ɐݒ肳��܂�)
    Color(float r, float g, float b);

    // RGBA�̒l���w�肵��RGBA�`���̐V�����F���쐬���܂��B
    Color(float r, float g, float b, float a);

    // RGB�̒l���w�肵��RGBA�`���̐V�����F���쐬���܂��B
    // (A�̒l��1.0�ɐݒ肳��܂�)
    Color(const Vector3& rgb);

    // RGBA�̒l���w�肵��RGBA�`���̐V�����F���쐬���܂��B
    Color(const Vector4& rgba);

    // RGBA�̒l���w�肵��RGBA�`���̐V�����F���쐬���܂��B
    Color(const float components[4]);

    // �L���X�g���Z�q�̃I�[�o�[���[�h
    operator const float* () const { return components; }

    // ���`���
    static Color LerpUnclamped(const Color& from, const Color& to, float t);

    // �P���v���X���Z�q
    const Color& operator + () const;

    // �P���}�C�i�X���Z�q
    Color operator - () const;

    // �e�F���������Z���܂��B
    Color& operator += (const Color& rhs);

    // �e�F���������Z���܂��B
    Color& operator -= (const Color& rhs);

    // �e�F��������Z���܂��B
    Color& operator *= (const Color& rhs);

    // �e�F�������X�J���[�l�ŏ�Z���܂��B
    Color& operator *= (float scaleFactor);

    // �e�F���������Z���܂��B
    Color& operator /= (const Color& rhs);

    // �e�F�������X�J���[�l�ŏ��Z���܂��B
    Color& operator /= (float divisor);
};

// �e�F���������Z�����V����Color��Ԃ��܂��B
Color operator + (const Color& lhs, const Color& rhs);

// �e�F���������Z�����V����Color��Ԃ��܂��B
Color operator - (const Color& lhs, const Color& rhs);

// �e�F��������Z�����V����Color��Ԃ��܂��B
Color operator * (const Color& lhs, const Color& rhs);

// �e�F�������X�J���[�l�ŏ�Z�����V����Color��Ԃ��܂��B
Color operator * (const Color& lhs, float scaleFactor);

// �e�F�������X�J���[�l�ŏ�Z�����V����Color��Ԃ��܂��B
Color operator * (float scaleFactor, const Color& rhs);

// �e�F���������Z�����V����Color��Ԃ��܂��B
Color operator / (const Color& lhs, const Color& rhs);

// �e�F�������X�J���[�l�ŏ��Z�����V����Color��Ԃ��܂��B
Color operator / (const Color& lhs, float divisor);

// �Ή�����F�������������ꍇ�� true ��Ԃ��܂��B
bool operator == (const Color& lhs, const Color& rhs);

// �Ή�����F�������قȂ�ꍇ�� true ��Ԃ��܂��B
bool operator != (const Color& lhs, const Color& rhs);

