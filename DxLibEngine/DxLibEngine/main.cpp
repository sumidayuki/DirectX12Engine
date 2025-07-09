//----------------------------------------------------------------------------------------
// ����̃G���W���ł� "Entity-Component-System" ���g�p���Ă��܂��B
// ���ꂼ��̃��[���Ƃ��� Entity �� id ���������܂���B
// Component �͕K�� struct ���̏�񂾂��������܂��B�i�֐��͐�΂ɏ����Ȃ��j
// System �ł� world �̒��ɂ��� Entity ���� View�N���X���g�p���� System �ŏ�����
// �s������ Component �������Ă��� Entity ���擾���A���� Component �ɏ������s��
// �V�X�e���ɂȂ��Ă��܂��B
// 
// ECS���g�p�������R�Ƃ��ẮA���ƂŐ��삵�Ă���G���W�����I�u�W�F�N�g�w���x�[�X
// �̃G���W�����������߁A�I�u�W�F�N�g�w���ł͂Ȃ��R���|�[�l���g�w�����g�p����
// �G���W��������Ă݂悤�Ǝv���A���ۂ̌���ł��g�p����邱�Ƃ̂���ECS���g�p�����G���W��
// ����낤�ƍl��������ł��B
//-----------------------------------------------------------------------------------------

// �G���g���[�|�C���g
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) 
{
    // �����񏈗����̒n��(���P�[��)�̐ݒ�
    ::setlocale(LC_ALL, "japanese");

    // COM (Component Object Model) �̏�����
    if (::CoInitializeEx(nullptr, COINIT_MULTITHREADED) < 0)
    {
        assert(0);
    }

    Screen::SetResolution(1920, 1080, false);

    Application::Run();

    // COM�̏I������
    ::CoUninitialize();

    return 0;
}