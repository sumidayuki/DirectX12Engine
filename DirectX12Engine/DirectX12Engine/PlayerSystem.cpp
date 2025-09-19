#include "Precompiled.h"
#include "PlayerSystem.h"
#include "PlayerTag.h"
#include "PlayerCamera.h"

void PlayerSystem::Move(Transform& transform, Input& input, Animator& animator)
{
	// �J������Transform���Ȃ���Ώ������Ȃ�
	if (!m_cameraTransform) return;

	// �J�����̌�������Ƃ����O���ƉE�������擾
	Vector3 camForward = m_cameraTransform->rotation * Vector3::forward;
	Vector3 camRight = m_cameraTransform->rotation * Vector3::right;

	// Y���̌X���𖳎����āA�����ȃx�N�g���ɂ���
	camForward.y = 0;
	camRight.y = 0;
	camForward.Normalized();
	camRight.Normalized();

	// �J�����̌����Ɠ��͂���A���[���h���W�n�ł̈ړ�����������
	Vector3 moveDirection = camForward * input.direction.y + camRight * input.direction.x;

	// �A�j���[�V�����N���b�v�̌���
	std::string targetClip = "Idle";

	// �ړ����͂����邩�ǂ�����臒l
	const float moveThreshold = 0.01f;

	if (moveDirection.SqrMagnitude() > moveThreshold)
	{
		moveDirection = moveDirection.Normalized(); // �����𐳋K��

		// �ړ����x������
		m_currentSpeed = input.dash ? RunSpeed : WalkSpeed;

		// �A�j���[�V���������� (��ɑO�i�A�j���[�V�������g�p)
		targetClip = (m_currentSpeed == RunSpeed) ? "Run_Forward" : "Walk_Forward";

		// �v���C���[�̌������A�ړ������Ɋ��炩�Ɍ�����
		Quaternion targetRotation = Quaternion::LookRotation(moveDirection, Vector3::up);
		const float rotationSpeed = 15.0f; // ��]�̑��� (�l���傫���قǑ���)
		transform.rotation = Quaternion::Slerp(transform.rotation, targetRotation, Time::GetDeltaTime() * rotationSpeed);

		// �v���C���[���ړ�������
		TransformSystem::Translate(transform, moveDirection * m_currentSpeed * Time::GetDeltaTime());
	}

	// �A�j���[�V�����̐؂�ւ�
	if (animator.currentClipName != targetClip)
	{
		AnimationSystem::Play(animator, targetClip);
	}
}

void PlayerSystem::Attack(Transform& transform, World& world)
{
	Vector3 forward = transform.rotation * Vector3::forward;
	forward.y = 0;
	forward.Normalized();

	Vector3 pos = TransformSystem::GetPosition(*m_bowTransform);

	Entity* a = world.CreateWithModel(L"Assets/Arrow.fbx", nullptr, pos, Quaternion::LookRotation(forward));
}

void PlayerSystem::Start(ComponentManager& cm, World& world)
{
	// �v���C���[�𐶐�
	Entity* player = world.CreateWithModel(L"Assets/player.fbx", nullptr, Vector3(0, 0, 0), Quaternion::identity);
	world.AddComponent<PlayerTag>(*player, PlayerTag{});
	world.AddComponent<Input>(*player, Input{});
	world.AddComponent<Velocity>(*player, Velocity{});

	// Player�̎q�G���e�B�e�B����|�𔭎˂���ꏊ�ƂȂ� Transform ���擾����B
	Transform* transform = world.GetComponent<Transform>(*player);
	m_bowTransform = TransformSystem::FindChild(transform, "mixamorig:LeftHandPinky4");

	// �v���C���[�J�����̐ݒ�
	float fov = 60.0f;
	float aspect = (float)Screen::GetWidth() / (float)Screen::GetHeight();
	float nearPlane = 0.1f;
	float farPlane = 5000.0f;
	Vector3 cameraPos = Vector3(06.0f, 1.0f, -8.0f);
	
	// �v���C���[�J�����̍쐬
	Entity* camera = world.CreateCamera3D(fov, aspect, nearPlane, farPlane, cameraPos);
	world.AddComponent<PlayerCamera>(*camera, PlayerCamera{ .player = player, .offset = Vector3(0, 150, -300), .sensitivity = 0.5f });

	m_cameraTransform = world.GetComponent<Transform>(*camera);

	// �����l
	m_currentSpeed = WalkSpeed;
	m_stateTimer = 0.0f;
	m_currentState = PlayerState::Move;
}

void PlayerSystem::Update(ComponentManager& cm, World& world)
{
	View<PlayerTag, Transform, Input, Animator> view(cm);

	for (auto [entity, playerTag, transform, input, animator] : view)
	{
		switch (m_currentState)
		{
		case PlayerState::Move:
			m_stateTimer += Time::GetDeltaTime();

			Move(transform, input, animator);

			if (input.attack)
			{
				AnimationSystem::Play(animator, "Attack_01");
				m_currentState = PlayerState::Attack;
				animator.isLoop = false;
				m_stateTimer = 0;
			}
			break;

		case PlayerState::Attack:

			m_stateTimer += Time::GetDeltaTime();

			if (!animator.isPlaying)
			{
				Attack(transform, world);
				m_currentState = PlayerState::Move;
				animator.isLoop = true;
				m_stateTimer = 0;
			}

			break;
		}

	}
}
