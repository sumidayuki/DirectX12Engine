#include "Reference.h"

Reference::Reference()
    : m_referenceCount(1)
{
}

ULONG Reference::Release()
{
    --m_referenceCount;
    if (m_referenceCount == 0)
    {
        delete this;
        return 0;
    }

    return m_referenceCount;
}

HRESULT Reference::QueryInterface(const IID& interfaceID, void** ppInterface)
{
    return E_NOTIMPL;
}
