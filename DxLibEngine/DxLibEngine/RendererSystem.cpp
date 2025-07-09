#include "RendererSystem.h"

void RendererSystem::RenderSprite(const Sprite& sprite, const Transform& transform)
{
	// スプライトの縦幅、横幅を変数に代入しておきます。
	float w = sprite.width;
	float h = sprite.height;

	// スプライトのピボットX、ピボットYを変数に代入しておきます。
	float pivotX = sprite.pivot;
	float pivotY = sprite.pivot;

	// 描画する頂点のローカル座標（左上、左下、右上、右下）を計算します。
	Vector3 localLT = Vector3(-w * pivotX, -h * pivotY, transform.position.z);
	Vector3 localRT = Vector3(w * (1.0f - pivotX), -h * pivotY, transform.position.z);
	Vector3 localLB = Vector3(-w * pivotX, h * (1.0f - pivotY), transform.position.z);
	Vector3 localRB = Vector3(w * (1.0f - pivotX), h * (1.0f - pivotY), transform.position.z);

	Vector3 lt = transform.localToWorldMatrix.MultiplyPoint3x4(localLT);
	Vector3 rt = transform.localToWorldMatrix.MultiplyPoint3x4(localRT);
	Vector3 lb = transform.localToWorldMatrix.MultiplyPoint3x4(localLB);
	Vector3 rb = transform.localToWorldMatrix.MultiplyPoint3x4(localRB);

	// sprite の RGB に合わせて色を変更します。
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

	// spritesをentityとspriteに分解して探索します。
	for (auto&& [entity, renderCommand, transform] : *m_view)
	{
		if (!entity.enabled) continue;

		// ソートするために配列に格納する。
		m_commands.emplace_back(renderCommand, transform);
	}

	// Z でソート（降順）
	std::sort(m_commands.begin(), m_commands.end(),
		[](const auto& a, const auto& b)
		{
			if ((int)std::get<0>(a).layer != (int)std::get<0>(b).layer)
				return (int)std::get<0>(a).layer < (int)std::get<0>(b).layer;

			return std::get<0>(a).orderInLayer > std::get<0>(b).orderInLayer;
		});

	// 描画処理
	for (const auto& [render, transform] : m_commands)
	{
		switch (render.type)
		{
		case RenderType::Sprite:
			// 無駄な処理を避けるためにアクティブでないスプライトはスキップする。
			if (!render.sprite.isActive) continue;
 			RenderSprite(render.sprite, transform);
			break;

		case RenderType::Circle:
			// 無駄な処理を避けるためにアクティブでないサークルはスキップする。
			if (!render.circle.isActive) continue;
			RenderCircle(render.circle, transform);
			break;

		case RenderType::Box:
			// 無駄な処理を避けるためにアクティブでないボックスはスキップする。
			if (!render.box.isActive) continue;
			RenderBox(render.box, transform);
			break;

		case RenderType::Triangle:
			// 無駄な処理を避けるためにアクティブでないトライアングルはスキップする。
			if (!render.triangle.isActive) continue;
			RenderTriangle(render.triangle, transform);
			break;
		}
	}

	m_commands.clear();
}
