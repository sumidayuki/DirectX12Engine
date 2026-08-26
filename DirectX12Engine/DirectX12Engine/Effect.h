#pragma once

#include <Effekseer.h>

class Effect : public Reference
{
private:
	std::string m_name;
	Effekseer::EffectRef m_nativeEffect;

private:
	Effect(const std::string& name, const Effekseer::EffectRef& nativeEffect);

public:
	static Effect* Create(const std::string& name, const Effekseer::EffectRef& nativeEffect);

	const std::string& GetName() const { return m_name; }

	const Effekseer::EffectRef& GetNativeEffect() const { return m_nativeEffect; }

	bool IsValid() const { return m_nativeEffect != nullptr; }
};