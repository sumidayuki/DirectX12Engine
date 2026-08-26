#include "Effect.h"

Effect::Effect(const std::string& name, const Effekseer::EffectRef& nativeEffect)
	: m_name(name)
	, m_nativeEffect(nativeEffect)
{
}

Effect* Effect::Create(const std::string& name, const Effekseer::EffectRef& nativeEffect)
{
	if (!nativeEffect)
	{
		return nullptr;
	}

	return new Effect(name, nativeEffect);
}
