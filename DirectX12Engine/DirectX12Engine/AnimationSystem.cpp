#include "AnimationSystem.h"

void AnimationSystem::Play(Animator& animator, const std::string& clipName)
{
	// クリップの存在をチェック
	auto it = animator.clips.find(clipName);
	if (it == animator.clips.end() || !it->second)
	{
		animator.isPlaying = false;
		animator.currentClip = nullptr;
		return;
	}

	// 現在再生中のクリップと同じ名前で、既に再生中なら何もしない
	if (animator.currentClip && animator.currentClip->GetName() == clipName && animator.isPlaying)
	{
		return;
	}

	// 再生するクリップ名を設定し、時間をリセット
	animator.currentClipName = clipName;
	animator.currentClip = it->second;
	animator.currentTime = 0.0f;
	animator.isPlaying = true;
}

Vector3 AnimationSystem::InterpolatePosition(float animationTime, const BoneAnimation& boneAnim)
{
	if (boneAnim.positions.size() == 1)
	{
		return boneAnim.positions[0].value;
	}

	int p0_index = FindKeyfrmeIndex(animationTime, boneAnim.positions);
	int p1_index = p0_index + 1;

	float t0 = boneAnim.positions[p0_index].timeStamp;
	float t1 = boneAnim.positions[p1_index].timeStamp;
	float scaleFactor = (animationTime - t0) / (t1 - t0);

	return Vector3::Lerp(boneAnim.positions[p0_index].value, boneAnim.positions[p1_index].value, scaleFactor);
}

Quaternion AnimationSystem::InterpolateRotation(float animationTime, const BoneAnimation& boneAnim)
{
	if (boneAnim.rotations.size() == 1)
	{
		return boneAnim.rotations[0].value;
	}

	int r0_index = FindKeyfrmeIndex(animationTime, boneAnim.rotations);
	int r1_index = r0_index + 1;

	float t0 = boneAnim.rotations[r0_index].timeStamp;
	float t1 = boneAnim.rotations[r1_index].timeStamp;
	float scaleFactor = (animationTime - t0) / (t1 - t0);

	return Quaternion::Slerp(boneAnim.rotations[r0_index].value, boneAnim.rotations[r1_index].value, scaleFactor);
}

Vector3 AnimationSystem::InterpolateScale(float animationTime, const BoneAnimation& boneAnim)
{
	if (boneAnim.scales.size() == 1)
	{
		return boneAnim.scales[0].value;
	}

	int s0_index = FindKeyfrmeIndex(animationTime, boneAnim.scales);
	int s1_index = s0_index + 1;

	float t0 = boneAnim.scales[s0_index].timeStamp;
	float t1 = boneAnim.scales[s1_index].timeStamp;
	float scaleFactor = (animationTime - t0) / (t1 - t0);

	return Vector3::Lerp(boneAnim.scales[s0_index].value, boneAnim.scales[s1_index].value, scaleFactor);
}

void AnimationSystem::CalculateBoneTransform(const Bone* bone, const Matrix4x4& parentTransform, const Animation* animation, float currentTime, std::unordered_map<std::string, Matrix4x4>& boneTransforms)
{
	if (!bone) return;

	std::string boneName = bone->name;
	Matrix4x4 nodeTransform = bone->localTransform;

	auto& boneAnimations = animation->GetBoneAnimations();

	if (boneAnimations.empty())
	{
		return;
	}

	if (boneAnimations.find(boneName) != boneAnimations.end())
	{
		const BoneAnimation& boneAnim = boneAnimations.at(boneName);
		Vector3 pos = InterpolatePosition(currentTime, boneAnim);
		Quaternion rot = InterpolateRotation(currentTime, boneAnim);
		Vector3 scale = InterpolateScale(currentTime, boneAnim);
		nodeTransform = Matrix4x4::TRS(pos, rot, scale);
	}

	Matrix4x4 globalTransform = nodeTransform * parentTransform;

	const auto& boneInfoMap =
		boneTransforms[boneName] = globalTransform;

	for (const auto& child : bone->children)
	{
		CalculateBoneTransform(child, globalTransform, animation, currentTime, boneTransforms);
	}
}

void AnimationSystem::Update(ComponentManager& cm, World& world)
{
	View<Animator, SkinnedMeshRenderer> view(cm);

	for (auto [entity, animator, smr] : view)
	{
		// クリップの切り替え処理
		if (!animator.currentClip || animator.currentClip->GetName() != animator.currentClipName)
		{
			auto it = animator.clips.find(animator.currentClipName);
			if (it != animator.clips.end())
			{
				// 新しいクリップを設定
				animator.currentClip = it->second;

				// 再生時間をリセット
				animator.currentTime = 0.0f;
			}
			else
			{
				// 指定された名前のクリップが見つからない場合は再生を止める
				animator.currentClip = nullptr;
				animator.isPlaying = false;
			}
		}

		if (!animator.isPlaying || !animator.currentClip)
		{
			continue;
		}

		// 時間を更新
		animator.currentTime += Time::GetDeltaTime() * animator.currentClip->GetTicksPerSecond();
		if (animator.currentTime >= animator.currentClip->GetDuration())
		{
			if (animator.isLoop)
			{
				animator.currentTime = fmod(animator.currentTime, animator.currentClip->GetDuration());
			}
			else
			{
				animator.currentTime = animator.currentClip->GetDuration();
				animator.isPlaying = false;
			}
		}

		if (!smr.skeleton)
		{
			continue;
		}

		std::unordered_map<std::string, Matrix4x4> boneTransforms;
		CalculateBoneTransform(smr.skeleton->GetRootBone(), Matrix4x4::identity, animator.currentClip.Get(), animator.currentTime, boneTransforms);

		// 最終的な行列を計算
		animator.finalBoneMatrices.resize(smr.skeleton->GetBoneCount());
		for (const auto& [boneName, boneInfo] : smr.skeleton->GetBoneInfoMap())
		{
			auto it = boneTransforms.find(boneName);
			if (it != boneTransforms.end())
			{
				// グローバル逆変換行列 * グローバルボーン変換行列 * ボーンのオフセット行列
				animator.finalBoneMatrices[boneInfo.id] = boneInfo.offsetMatrix * it->second * smr.skeleton->GetGlobalInverseTransform();
			}
		}
	}
}