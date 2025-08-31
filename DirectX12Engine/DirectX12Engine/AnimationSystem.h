#pragma once

/// <summary>
/// Animatorコンポーネントを処理し、ボーンの交換を更新するシステム。
/// </summary>
class AnimationSystem : public System
{
private:
	/// <summary>
	/// 指定された時間における、2つのキーフレーム間の補間された値を見つけます。
	/// </summary>
	/// <typeparam name="T"></typeparam>
	/// <param name="animationTime"></param>
	/// <param name="keyframes"></param>
	/// <returns></returns>
	template<typename T>
	int FindKeyfrmeIndex(float animationTime, const std::vector<Keyframe<T>>& keyframes);

	/// <summary>
	/// 位置を線形補間します。
	/// </summary>
	/// <param name="animationTime"></param>
	/// <param name="boneAnim"></param>
	/// <returns></returns>
	Vector3 InterpolatePosition(float animationTime, const BoneAnimation& boneAnim);

	/// <summary>
	/// 回転を球面線形補間（Slerp）します。
	/// </summary>
	/// <param name="animationTime"></param>
	/// <param name="boneAnim"></param>
	/// <returns></returns>
	Quaternion InterpolateRotation(float animationTime, const BoneAnimation& boneAnim);

	/// <summary>
	/// スケールを線形補間します。
	/// </summary>
	/// <param name="animationTime"></param>
	/// <param name="boneAnim"></param>
	/// <returns></returns>
	Vector3 InterpolateScale(float animationTime, const BoneAnimation& boneAnim);

	/// <summary>
	/// ボーンの階層を再帰的に更新します。
	/// </summary>
	/// <param name="bone"></param>
	/// <param name="parentTransform"></param>
	/// <param name="animation"></param>
	/// <param name="currentTime"></param>
	/// <param name="boneTransforms"></param>
	void CalculateBoneTransform(const Bone* bone, const Matrix4x4& parentTransform, const Animation* animation, float currentTime, std::unordered_map<std::string, Matrix4x4>& boneTransforms);

public:
	void Update(ComponentManager& cm, World& world) override;
};

template<typename T>
inline int AnimationSystem::FindKeyfrmeIndex(float animationTime, const std::vector<Keyframe<T>>& keyframes)
{
	for (size_t i = 0; i < keyframes.size() - 1; ++i)
	{
		if (animationTime < keyframes[i + 1].timeStamp)
			return i;
	}

	return 0;
}
