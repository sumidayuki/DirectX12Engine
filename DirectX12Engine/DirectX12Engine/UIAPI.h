#pragma once

namespace UIAPI
{
    /// <summary>
    /// UIImageにスプライトを設定し、必要なGPUリソースを初期化します。
    /// </summary>
    /// <param name="uiImage">対象のUIImage</param>
    /// <param name="sprite">設定するスプライト</param>
    void SetSprite(UIImage* uiImage, Sprite* sprite);
}