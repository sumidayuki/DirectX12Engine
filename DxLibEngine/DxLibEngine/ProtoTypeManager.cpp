#include "ProtoTypeManager.h"

ProtoTypeManager::ProtoTypeManager()
{
}

ProtoTypeManager::ProtoTypeManager(const ProtoTypeManager& source)
{
}

ProtoTypeManager::ProtoTypeManager(ProtoTypeManager&& source)
{
}

ProtoTypeManager::~ProtoTypeManager()
{
}

void ProtoTypeManager::CreateSingleton()
{
    if (!m_singleton)
    {
        m_singleton = new ProtoTypeManager();
    }
}

void ProtoTypeManager::DestroySingleton()
{
    if (m_singleton)
    {
        delete m_singleton;
        m_singleton = nullptr;
    }
}

void ProtoTypeManager::Register(const std::string& name, const ProtoType& proto)
{
    m_ptototypes[name] = proto;
}

const ProtoType* ProtoTypeManager::Get(const std::string& name) const
{
    auto it = m_ptototypes.find(name);
    if (it != m_ptototypes.end())
    {
        return &it->second;
    }

    return nullptr;
}
