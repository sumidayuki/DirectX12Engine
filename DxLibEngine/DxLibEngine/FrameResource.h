#pragma once

class FrameResource : public Reference
{
private:
    // フレンド
    friend class Application;
    friend class Graphics;

    // 型エイリアス
    using GPUCompletionEventHandler = std::function<void()>;
    using GPUCompletionEventHandlers = std::vector<GPUCompletionEventHandler>;

private:
    ComPtr<ID3D12Resource>              m_backBuffer;                   // レンダーターゲット (今回はバックバッファ)
    D3D12_CPU_DESCRIPTOR_HANDLE         m_handleRTV;                    // レンダーターゲットビュー(RTV)のアドレス
    ComPtr<ID3D12Resource>              m_depthStencilBuffer;           // 深度・ステンシルバッファリソース
    ComPtr<DescriptorHeap>              m_descriptorHeapDSV;            // 深度・ステンシルバッファビュー(DSV)の格納先ディスクリプタヒープ
    D3D12_CPU_DESCRIPTOR_HANDLE         m_handleDSV;                    // 深度・ステンシルバッファビュー(DSV)のアドレス
    ComPtr<ID3D12GraphicsCommandList>   m_commandList;                  // コマンドリスト
    ComPtr<ID3D12CommandAllocator>      m_commandAllocator;             // コマンドアロケーター
    ComPtr<ID3D12Fence>                 m_fence;                        // フェンスオブジェクト
    HANDLE                              m_fenceEvent;                   // フェンス到達通知用イベントオブジェクト
    GPUCompletionEventHandlers          m_GPUCompletionEventHandlers;   // GPU処理完了時のイベントハンドラ配列

private:
    // 引数付きコンストラクタ
    FrameResource(ID3D12Resource* backBuffer, D3D12_CPU_DESCRIPTOR_HANDLE handleRTV);

    void Update();

    void Render();

public:
    /// <summary>
    /// 関連付けられたバックバッファを取得します。
    /// </summary>
    /// <returns></returns>
    ID3D12Resource* GetBuckBuffer() const { return m_backBuffer.Get(); }

    /// <summary>
    /// バックバッファに関連付けられたRTVのCPU側アドレスを取得します。
    /// </summary>
    /// <returns></returns>
    D3D12_CPU_DESCRIPTOR_HANDLE GetHandleRTV() const { return m_handleRTV; }

    /// <summary>
    /// 深度ステンシルバッファを取得します。
    /// </summary>
    /// <returns></returns>
    ID3D12Resource* GetDepthStencilBuffer() const { return m_depthStencilBuffer.Get(); }

    /// <summary>
    /// 深度ステンシルビューのCPU側アドレスを取得します。
    /// </summary>
    /// <returns></returns>
    D3D12_CPU_DESCRIPTOR_HANDLE GetHandleDSV() const { return m_handleDSV; }

    /// <summary>
    /// コマンドアロケーターを取得します。
    /// </summary>
    /// <returns></returns>
    ID3D12CommandAllocator* GetCommandAllocator() const { return m_commandAllocator.Get(); }

    /// <summary>
    /// コマンドリストを取得します。
    /// </summary>
    /// <returns></returns>
    ID3D12GraphicsCommandList* GetCommandList() const { return m_commandList.Get(); }

    /// <summary>
    /// フェンスオブジェクトを取得します。
    /// </summary>
    /// <returns></returns>
    ID3D12Fence* GetFence() const { return m_fence.Get(); }

    /// <summary>
    /// フェンスイベントを取得します。
    /// </summary>
    /// <returns></returns>
    HANDLE GetFenceEvent() const { return m_fenceEvent; }

    void AddEventAfterGPUCompletion(const GPUCompletionEventHandler& eventHandler);

    void ExecuteCommandBuffer();

    void WaitUntilReadyToRender();
};