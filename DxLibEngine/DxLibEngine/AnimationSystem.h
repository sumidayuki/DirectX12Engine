#pragma once

/// <summary>
/// Animator�R���|�[�l���g���������A�{�[���̌������X�V����V�X�e���B
/// </summary>
class AnimationSystem : public System
{
private:
	/// <summary>
	/// �w�肳�ꂽ���Ԃɂ�����A2�̃L�[�t���[���Ԃ̕�Ԃ��ꂽ�l�������܂��B
	/// </summary>
	/// <typeparam name="T"></typeparam>
	/// <param name="animationTime"></param>
	/// <param name="keyframes"></param>
	/// <returns></returns>
	template<typename T>
	int FindKeyfrmeIndex(float animationTime, const std::vector<Keyframe<T>>& keyframes);

	/// <summary>
	/// �ʒu����`��Ԃ��܂��B
	/// </summary>
	/// <param name="animationTime"></param>
	/// <param name="boneAnim"></param>
	/// <returns></returns>
	Vector3 InterpolatePosition(float animationTime, const BoneAnimation& boneAnim);

	/// <summary>
	/// ��]�����ʐ��`��ԁiSlerp�j���܂��B
	/// </summary>
	/// <param name="animationTime"></param>
	/// <param name="boneAnim"></param>
	/// <returns></returns>
	Quaternion InterpolateRotation(float animationTime, const BoneAnimation& boneAnim);

	/// <summary>
	/// �X�P�[������`��Ԃ��܂��B
	/// </summary>
	/// <param name="animationTime"></param>
	/// <param name="boneAnim"></param>
	/// <returns></returns>
	Vector3 InterpolateScale(float animationTime, const BoneAnimation& boneAnim);

	/// <summary>
	/// �{�[���̊K�w���ċA�I�ɍX�V���܂��B
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
