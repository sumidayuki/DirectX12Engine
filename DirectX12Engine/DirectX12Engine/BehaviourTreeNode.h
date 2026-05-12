#pragma once

enum class BTStatus
{
	Success,
	Failure,
	Running
};

struct Blackboard
{
	World* world = nullptr;
	Entity self = INVALID_ENTITY;
	Entity target = INVALID_ENTITY;

	std::unordered_map<std::string, float> floats;
	std::unordered_map<std::string, Vector3> vectors;
	std::unordered_map<std::string, bool> bools;

	void SetFloat(const std::string& key, float value) { floats[key] = value; }
	float GetFloat(const std::string& key, float def = 0) const
	{
		auto it = floats.find(key);
		return it != floats.end() ? it->second : def;
	}

	void SetVector(const std::string& key, const Vector3& value) { vectors[key] = value; }
	Vector3 GetVector(const std::string& key, const Vector3& def = Vector3::zero) const
	{
		auto it = vectors.find(key);
		return it != vectors.end() ? it->second : def;
	}

	void SetBool(const std::string& key, bool value) { bools[key] = value; }
	bool GetBool(const std::string& key, bool def = false) const
	{
		auto it = bools.find(key);
		return it != bools.end() ? it->second : def;
	}
};

class BTNode
{
public:
	virtual ~BTNode() = default;
	virtual BTStatus Tick(Blackboard& blackboard) = 0;
	virtual void Reset() {}
};