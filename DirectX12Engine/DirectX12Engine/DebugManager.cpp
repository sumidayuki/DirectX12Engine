#include "DebugManager.h"

void DebugManager::DrawLine(const Vector3& start, const Vector3& end, const Color& color)
{
#ifdef _DEBUG
	m_lineRenderer->AddLine(start, end, color);
#endif
}

void DebugManager::DrawAABB(const Vector3& min, const Vector3& max, const Color& color)
{
	// 8頂点の計算
	Vector3 v0 = Vector3(min.x, min.y, min.z);	// 左下
	Vector3 v1 = Vector3(min.x, min.y, max.z);	// 左下奥
	Vector3 v2 = Vector3(max.x, min.y, min.z);	// 右下
	Vector3 v3 = Vector3(max.x, min.y, max.z);	// 右下奥
	Vector3 v4 = Vector3(min.x, max.y, min.z);	// 左上
	Vector3 v5 = Vector3(min.x, max.y, max.z);	// 左上奥
	Vector3 v6 = Vector3(max.x, max.y, min.z);	// 右上
	Vector3 v7 = Vector3(max.x, max.y, max.z);	// 右上奥

	// 12本の辺を描画
	DrawLine(v0, v1, color);	// 下側の前辺
	DrawLine(v1, v3, color);	// 下側の奥辺
	DrawLine(v3, v2, color);	// 下側の右辺
	DrawLine(v2, v0, color);	// 下側の左辺

	DrawLine(v4, v5, color);	// 上側の前辺
	DrawLine(v5, v7, color);	// 上側の奥辺
	DrawLine(v7, v6, color);	// 上側の右辺
	DrawLine(v6, v4, color);	// 上側の左辺

	DrawLine(v0, v4, color);	// 左前の垂直辺
	DrawLine(v1, v5, color);	// 左奥の垂直辺
	DrawLine(v2, v6, color);	// 右前の垂直辺
	DrawLine(v3, v7, color);	// 右奥の垂直辺
}

void DebugManager::DrawSphere(const Vector3& center, float radius, const Color& color)
{
	const uint16_t segments = 12; // 経度方向の分割数

	// 頂点数を計算する
	const int vertexCount = (segments + 1) * (segments + 1);

	// 頂点配列を作成する
	std::vector<Vector3> vertices;

	// 1周分の角度θの増分を計算する
	const float deltaTheta = 360.0f / segments;

	// 半周分の角度φの増分を計算する
	const float deltaPhi = 180.0f / segments;

	for (int j = 0; j < segments + 1; j++)
	{
		// 角度φ (-90°～ +90°)
		const float phi = -90 + deltaPhi * j;
		const float y = radius * Mathf::Sin(phi * Mathf::Deg2Rad);
		const float r = radius * Mathf::Cos(phi * Mathf::Deg2Rad);

		for (int i = 0; i < segments + 1; i++)
		{
			// 角度θ (0°～360°)
			const float theta = deltaTheta * i;
			const float x = r * Mathf::Cos(theta * Mathf::Deg2Rad);
			const float z = r * Mathf::Sin(theta * Mathf::Deg2Rad);
			vertices.emplace_back(x, y, z);
		}
	}

	for (int j = 0; j < segments; j++)
	{
		for (int i = 0; i < segments; i++)
		{
			const int first = (j * (segments + 1)) + i;
			const int second = first + segments + 1;
			// 線を描画
			m_lineRenderer->AddLine(center + vertices[first], center + vertices[second], color);
			m_lineRenderer->AddLine(center + vertices[first], center + vertices[first + 1], color);
		}
	}
}

void DebugManager::DrawText(const Vector2& position, const std::wstring& text, const Color& color)
{
}

void DebugManager::Initialize()
{
	m_isActive = false;
	m_lineRenderer = new DebugLineRenderer();
	m_lineRenderer->Initialize();
}

void DebugManager::Render()
{
	if (Keyboard::GetKeyState(KeyCode::F1).WasPressedThisFrame())
	{
		m_isActive = !m_isActive;
	}

	if (m_isActive)
	{
		m_lineRenderer->Render(Matrix4x4::identity);
	}
}

void DebugManager::Shutdown()
{
	m_lineRenderer->Shutdown();
}
