#include "UIAPI.h"

namespace UIAPI
{
	// 正射影行列用の定数バッファレイアウト
	struct OrthoBufferLayout
	{
		Matrix4x4 orthoMatrix;
	};

	// オブジェクト用の定数バッファレイアウト
	struct ConstantBufferLayout
	{
		Matrix4x4       world;
		Color           color;
		Vector2         minUV;
		Vector2         maxUV;
		unsigned int    flipEnable[4];
	};

	void SetSprite(UIImage* uiImage, Sprite* sprite)
	{
		if (sprite != uiImage->sprite.Get())
		{
			uiImage->sprite = sprite;

			if (!uiImage->constantBuffer)
			{
				// 定数バッファの作成
				uiImage->constantBuffer.Attach(new GraphicsBuffer(GraphicsBuffer::Target::Constant, GraphicsBuffer::UsageFlags::LockBufferForWrite, 1, sizeof(ConstantBufferLayout)));
			}
			if (!uiImage->descriptorHeap)
			{
				// ディスクリプタヒープを作成
				uiImage->descriptorHeap.Attach(new DescriptorHeap(DescriptorHeapType::CBV_SRV_UAV, 1, true));
			}

			// スプライトが nullptr でない場合はSRVをコピーしておく
			if (uiImage->sprite)
			{
				const D3D12_CPU_DESCRIPTOR_HANDLE copyTo = uiImage->descriptorHeap->GetCPUDescriptorHandle(0);
				const D3D12_CPU_DESCRIPTOR_HANDLE copyFrom = uiImage->sprite->GetTexture()->GetDescriptorHeap()->GetCPUDescriptorHandleForHeapStart();
				Graphics::GetD3D12Device()->CopyDescriptorsSimple(1, copyTo, copyFrom, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
			}
		}
	}
}
