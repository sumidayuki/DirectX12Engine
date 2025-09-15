#pragma once

// �n�b�V���֐����`����
namespace std
{
    template <>
    struct hash<Entity> {
        size_t operator()(const Entity& e) const noexcept
        {
            return std::hash<uint64_t>()((static_cast<uint64_t>(e.id) << 32) | e.generation);
        }
    };
}

//---------------------------------------------------------------------------------------------------------------------
// Entity
//---------------------------------------------------------------------------------------------------------------------

// �񃁃��o�[�̔�r���Z�q���`����
inline bool operator==(const Entity& lhs, const Entity& rhs)
{
    return lhs.id == rhs.id && lhs.generation == rhs.generation;
}

inline bool operator!=(const Entity& lhs, const Entity& rhs)
{
    return !(lhs == rhs);
}

/// <summary>
/// std::pair �� unordered_map �̃L�[�Ƃ��Ďg�p����ׂ̃n�b�V���֐��ł��B
/// ���̍\���̂́Aoperator���������Ă��Ȃ����߁Aclass �ł͂Ȃ� struct �Ƃ��Ē�`���Ă��܂��B
/// </summary>
struct PairHash
{
	template <typename T1, typename T2>
	std::size_t operator()(const std::pair<T1, T2>& p) const
	{
		auto h1 = std::hash<T1>{}(p.first);
		auto h2 = std::hash<T2>{}(p.second);
		return h1 ^ (h2 << 1);
	}
};

//-----------------------------------------------------------------------------------------------------------