#include "BindlessHeap.h"

BindlessHeap::BindlessHeap()
{
	m_descriptorHeap.Attach(new DescriptorHeap(DescriptorHeapType::CBV_SRV_UAV, MAX_DESCRIPTORS, true));
	m_nextSlot = 0;
	while (!m_freeList.empty()) m_freeList.pop();
}

BindlessHeap::~BindlessHeap()
{
	m_descriptorHeap = nullptr;
	m_nextSlot = 0;
	while (!m_freeList.empty()) m_freeList.pop();
}

UINT BindlessHeap::Register(Texture2D* texture)
{
	if (!texture || !texture->GetNativeResource()) return 0;

	// 既に登録済みの場合はそのインデックスを返す
	if (texture->GetBindlessIndex() != UINT32_MAX)
	{
		return texture->GetBindlessIndex();
	}

	UINT index;
	if (!m_freeList.empty())
	{
		index = m_freeList.front();
		m_freeList.pop();
	}
	else
	{
		if (m_nextSlot >= MAX_DESCRIPTORS)
		{
			OutputDebugStringA("BindlessHeap: スロット上限に達しました\n");
			return 0;
		}
		index = m_nextSlot++;
	}

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Format = texture->GetNativeResource()->GetDesc().Format;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = texture->GetNativeResource()->GetDesc().MipLevels;

	Graphics::GetD3D12Device()->CreateShaderResourceView(
		texture->GetNativeResource(), 
		&srvDesc, 
		m_descriptorHeap->GetCPUDescriptorHandle(index)
	);

	texture->SetBindlessIndex(index);
	return index;
}

UINT BindlessHeap::RegisterBuffer(ID3D12Resource* buffer, const D3D12_SHADER_RESOURCE_VIEW_DESC& srvDesc)
{
	UINT index;
	if (!m_freeList.empty())
	{
		index = m_freeList.front();
		m_freeList.pop();
	}
	else
	{
		if (m_nextSlot >= MAX_DESCRIPTORS)
		{
			OutputDebugStringA("BindlessHeap: スロット上限に達しました\n");
			return 0;
		}
		index = m_nextSlot++;
	}

	Graphics::GetD3D12Device()->CreateShaderResourceView(
		buffer, 
		&srvDesc, 
		m_descriptorHeap->GetCPUDescriptorHandle(index)
	);

	return index;
}

void BindlessHeap::Unregister(UINT index)
{
	if (index < MAX_DESCRIPTORS)
	{
		m_freeList.push(index);
	}
}
