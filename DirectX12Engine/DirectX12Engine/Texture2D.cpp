#include "Texture2D.h"

Texture2D::Texture2D()
    : m_nativeTexture(nullptr)
    , m_uploadBuffer(nullptr)
    , m_descriptorHeap(nullptr)
	, m_bindlessIndex(UINT32_MAX)
{

}


Texture2D::~Texture2D()
{

}

