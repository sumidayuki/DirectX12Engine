#include "Texture2D.h"

Texture2D::Texture2D()
    : m_nativeTexture(nullptr)
    , m_uploadBuffer(nullptr)
    , m_descriptorHeap(nullptr)
{

}


Texture2D::~Texture2D()
{

}

