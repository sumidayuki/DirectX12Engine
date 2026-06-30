#include "Animation.h"

void Animation::SetEvent(std::function<void(World& world, Entity entity)> event, float timeStamp)
{
	Keyframe<std::function<void(World& world, Entity entity)>> keyframe;
	keyframe.value = event;
	keyframe.timeStamp = timeStamp;

	m_events.push_back(keyframe);

	// タイムスタンプが昇順（早い順）になるようにソートしておく
	std::sort(m_events.begin(), m_events.end(), [](const auto& a, const auto& b) {
		return a.timeStamp < b.timeStamp;
	});
}
