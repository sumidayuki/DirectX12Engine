#pragma once

class FrameResource : public Reference
{
private:
    // �t�����h
    friend class Application;
    friend class Graphics;

    // �^�G�C���A�X
    using GPUCompletionEventHandler = std::function<void()>;
    using GPUCompletionEventHandlers = std::vector<GPUCompletionEventHandler>;

private:
    ComPtr<ID3D12Resource>              m_backBuffer;                   // �����_�[�^�[�Q�b�g (����̓o�b�N�o�b�t�@)
    D3D12_CPU_DESCRIPTOR_HANDLE         m_handleRTV;                    // �����_�[�^�[�Q�b�g�r���[(RTV)�̃A�h���X
    ComPtr<ID3D12Resource>              m_depthStencilBuffer;           // �[�x�E�X�e���V���o�b�t�@���\�[�X
    ComPtr<DescriptorHeap>              m_descriptorHeapDSV;            // �[�x�E�X�e���V���o�b�t�@�r���[(DSV)�̊i�[��f�B�X�N���v�^�q�[�v
    D3D12_CPU_DESCRIPTOR_HANDLE         m_handleDSV;                    // �[�x�E�X�e���V���o�b�t�@�r���[(DSV)�̃A�h���X
    ComPtr<ID3D12GraphicsCommandList>   m_commandList;                  // �R�}���h���X�g
    ComPtr<ID3D12CommandAllocator>      m_commandAllocator;             // �R�}���h�A���P�[�^�[
    ComPtr<ID3D12Fence>                 m_fence;                        // �t�F���X�I�u�W�F�N�g
    HANDLE                              m_fenceEvent;                   // �t�F���X���B�ʒm�p�C�x���g�I�u�W�F�N�g
    GPUCompletionEventHandlers          m_GPUCompletionEventHandlers;   // GPU�����������̃C�x���g�n���h���z��

private:
    // �����t���R���X�g���N�^
    FrameResource(ID3D12Resource* backBuffer, D3D12_CPU_DESCRIPTOR_HANDLE handleRTV);

    void Update();

    void Render();

public:
    /// <summary>
    /// �֘A�t����ꂽ�o�b�N�o�b�t�@���擾���܂��B
    /// </summary>
    /// <returns></returns>
    ID3D12Resource* GetBuckBuffer() const { return m_backBuffer.Get(); }

    /// <summary>
    /// �o�b�N�o�b�t�@�Ɋ֘A�t����ꂽRTV��CPU���A�h���X���擾���܂��B
    /// </summary>
    /// <returns></returns>
    D3D12_CPU_DESCRIPTOR_HANDLE GetHandleRTV() const { return m_handleRTV; }

    /// <summary>
    /// �[�x�X�e���V���o�b�t�@���擾���܂��B
    /// </summary>
    /// <returns></returns>
    ID3D12Resource* GetDepthStencilBuffer() const { return m_depthStencilBuffer.Get(); }

    /// <summary>
    /// �[�x�X�e���V���r���[��CPU���A�h���X���擾���܂��B
    /// </summary>
    /// <returns></returns>
    D3D12_CPU_DESCRIPTOR_HANDLE GetHandleDSV() const { return m_handleDSV; }

    /// <summary>
    /// �R�}���h�A���P�[�^�[���擾���܂��B
    /// </summary>
    /// <returns></returns>
    ID3D12CommandAllocator* GetCommandAllocator() const { return m_commandAllocator.Get(); }

    /// <summary>
    /// �R�}���h���X�g���擾���܂��B
    /// </summary>
    /// <returns></returns>
    ID3D12GraphicsCommandList* GetCommandList() const { return m_commandList.Get(); }

    /// <summary>
    /// �t�F���X�I�u�W�F�N�g���擾���܂��B
    /// </summary>
    /// <returns></returns>
    ID3D12Fence* GetFence() const { return m_fence.Get(); }

    /// <summary>
    /// �t�F���X�C�x���g���擾���܂��B
    /// </summary>
    /// <returns></returns>
    HANDLE GetFenceEvent() const { return m_fenceEvent; }

    void AddEventAfterGPUCompletion(const GPUCompletionEventHandler& eventHandler);

    void ExecuteCommandBuffer();

    void WaitUntilReadyToRender();
};