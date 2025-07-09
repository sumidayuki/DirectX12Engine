#include "Texture.h"

Texture::Texture()
    : m_dimension(TextureDimension::Unknown)
    , m_width(0)
    , m_height(0)
    , m_anisoLevel(0)
    , m_wrapModeU(TextureWrapMode::Clamp)
    , m_wrapModeV(TextureWrapMode::Clamp)
    , m_wrapModeW(TextureWrapMode::Clamp)
    , m_filterMode(FilterMode::Point)
    , m_mipMapBias(0.0f)
    , m_isReadable(false)
{

}


void Texture::SetWrapMode(TextureWrapMode wrapMode)
{
    m_wrapModeU = wrapMode;
    m_wrapModeV = wrapMode;
    m_wrapModeW = wrapMode;
}
