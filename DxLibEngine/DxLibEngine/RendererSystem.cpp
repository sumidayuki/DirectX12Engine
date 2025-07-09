#include "RendererSystem.h"

void RendererSystem::RenderSprite(const Sprite& sprite, const Transform& transform)
{
	// �X�v���C�g�̏c���A������ϐ��ɑ�����Ă����܂��B
	float w = sprite.width;
	float h = sprite.height;

	// �X�v���C�g�̃s�{�b�gX�A�s�{�b�gY��ϐ��ɑ�����Ă����܂��B
	float pivotX = sprite.pivot;
	float pivotY = sprite.pivot;

	// �`�悷�钸�_�̃��[�J�����W�i����A�����A�E��A�E���j���v�Z���܂��B
	Vector3 localLT = Vector3(-w * pivotX, -h * pivotY, transform.position.z);
	Vector3 localRT = Vector3(w * (1.0f - pivotX), -h * pivotY, transform.position.z);
	Vector3 localLB = Vector3(-w * pivotX, h * (1.0f - pivotY), transform.position.z);
	Vector3 localRB = Vector3(w * (1.0f - pivotX), h * (1.0f - pivotY), transform.position.z);

	Vector3 lt = transform.localToWorldMatrix.MultiplyPoint3x4(localLT);
	Vector3 rt = transform.localToWorldMatrix.MultiplyPoint3x4(localRT);
	Vector3 lb = transform.localToWorldMatrix.MultiplyPoint3x4(localLB);
	Vector3 rb = transform.localToWorldMatrix.MultiplyPoint3x4(localRB);

	// sprite �� RGB �ɍ��킹�ĐF��ύX���܂��B
	SetDrawBright(sprite.r, sprite.g, sprite.b);

	DrawModiGraphF(
		lt.x, lt.y,
		rt.x, rt.y,
		rb.x, rb.y,
		lb.x, lb.y,
		sprite.handle,
		TRUE
	);
}

void RendererSystem::RenderCircle(const Circle& circle, const Transform& transform)
{
	DrawCircle(transform.position.x, transform.position.y, circle.radius, GetColor(circle.r, circle.g, circle.b), TRUE);
}

void RendererSystem::RenderBox(const Box& box, const Transform& transform)
{
	DrawBox(transform.position.x, transform.position.y, transform.position.x + (int)box.x, transform.position.y + (int)box.y, GetColor(box.r, box.g, box.b), TRUE);
}

void RendererSystem::RenderTriangle(const Triangle& triangle, const Transform& transform)
{
	DrawTriangle
	(
		(int)triangle.x1 + transform.position.x, (int)triangle.y1 + transform.position.y,
		(int)triangle.x2 + transform.position.x, (int)triangle.y2 + transform.position.y,
		(int)triangle.x3 + transform.position.x, (int)triangle.y3 + transform.position.y,
		GetColor(triangle.r, triangle.g, triangle.b),
		TRUE
	);
}

void RendererSystem::Start(ComponentManager& cm, World& world)
{
	m_view = std::make_unique<View<RenderCommand, Transform>>(cm);
}

void RendererSystem::Draw(ComponentManager& cm, World& world)
{
	m_view->Update();

	// sprites��entity��sprite�ɕ������ĒT�����܂��B
	for (auto&& [entity, renderCommand, transform] : *m_view)
	{
		if (!entity.enabled) continue;

		// �\�[�g���邽�߂ɔz��Ɋi�[����B
		m_commands.emplace_back(renderCommand, transform);
	}

	// Z �Ń\�[�g�i�~���j
	std::sort(m_commands.begin(), m_commands.end(),
		[](const auto& a, const auto& b)
		{
			if ((int)std::get<0>(a).layer != (int)std::get<0>(b).layer)
				return (int)std::get<0>(a).layer < (int)std::get<0>(b).layer;

			return std::get<0>(a).orderInLayer > std::get<0>(b).orderInLayer;
		});

	// �`�揈��
	for (const auto& [render, transform] : m_commands)
	{
		switch (render.type)
		{
		case RenderType::Sprite:
			// ���ʂȏ���������邽�߂ɃA�N�e�B�u�łȂ��X�v���C�g�̓X�L�b�v����B
			if (!render.sprite.isActive) continue;
 			RenderSprite(render.sprite, transform);
			break;

		case RenderType::Circle:
			// ���ʂȏ���������邽�߂ɃA�N�e�B�u�łȂ��T�[�N���̓X�L�b�v����B
			if (!render.circle.isActive) continue;
			RenderCircle(render.circle, transform);
			break;

		case RenderType::Box:
			// ���ʂȏ���������邽�߂ɃA�N�e�B�u�łȂ��{�b�N�X�̓X�L�b�v����B
			if (!render.box.isActive) continue;
			RenderBox(render.box, transform);
			break;

		case RenderType::Triangle:
			// ���ʂȏ���������邽�߂ɃA�N�e�B�u�łȂ��g���C�A���O���̓X�L�b�v����B
			if (!render.triangle.isActive) continue;
			RenderTriangle(render.triangle, transform);
			break;
		}
	}

	m_commands.clear();
}
