#include "Precompiled.h"
#include "SpriteAPI.h"

void SpriteAPI::SetSprite(SpriteRenderer* spriteRenderer, Sprite* sprite)
{
	if (sprite != spriteRenderer->sprite.Get())
	{
		spriteRenderer->sprite = sprite;

		if (!spriteRenderer->constantBuffer)
		{
			// 定数バッファの作成
			spriteRenderer->constantBuffer.Attach(new GraphicsBuffer(GraphicsBuffer::Target::Constant, GraphicsBuffer::UsageFlags::LockBufferForWrite, 1, sizeof(ConstantBufferLayout)));
		}
		if (!spriteRenderer->descriptorHeap)
		{
			// ディスクリプタヒープを作成
			spriteRenderer->descriptorHeap.Attach(new DescriptorHeap(DescriptorHeapType::CBV_SRV_UAV, 1, true));
		}

		// スプライトが nullptr 出ない場合はSRVをコピーしておく
		if (spriteRenderer->sprite)
		{
			const D3D12_CPU_DESCRIPTOR_HANDLE copyTo = spriteRenderer->descriptorHeap->GetCPUDescriptorHandle(0);
			const D3D12_CPU_DESCRIPTOR_HANDLE copyFrom = spriteRenderer->sprite->GetTexture()->GetDescriptorHeap()->GetCPUDescriptorHandleForHeapStart();
			Graphics::GetD3D12Device()->CopyDescriptorsSimple(1, copyTo, copyFrom, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		}
	}
}
