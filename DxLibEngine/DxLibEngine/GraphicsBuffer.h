#pragma once

/// <summary>
/// CPUからアクセス可能なメモリ上に記憶領域を作成してレンダリングに必要なデータを格納します。
/// このグラフィックスバッファは「アップロードバッファ」「デフォルトバッファ」両方の機能を備えています。
/// </summary>
class GraphicsBuffer : public Reference
{
public:
	// グラフィックスバッファの用途を表す列挙型
	enum class Target
	{
		Vertex,
		Index,
		Constant,
		Structured,
	};

	// グラフィックスバッファの使用方法を表す列挙型
	enum class UsageFlags
	{
		None,
		LockBufferForWrite,
	};

private:
	// 無効なインデックス
	static constexpr int InvalidIndex = -1;

private:
	Target					m_target;
	UsageFlags				m_usageFlags;
	int						m_count;
	int						m_stride;
	int						m_sizeInBytes;
	D3D12_RESOURCE_STATES	m_nativeBufferOriginallyState;
	ComPtr<ID3D12Resource>	m_nativeBuffer;
	ComPtr<ID3D12Resource>	m_uploadBuffer;
	int						m_lockStartIndex;
	int						m_lookedCount;

private:
	/// <summary>
	/// デフォルトバッファを作成します。
	/// </summary>
	/// <param name="size"></param>
	/// <param name="initialState"></param>
	/// <returns></returns>
	static ID3D12Resource* CreateDefaultBuffer(int size, D3D12_RESOURCE_STATES initialState);

	/// <summary>
	/// アップロードバッファを作成します
	/// </summary>
	/// <param name="size"></param>
	/// <returns></returns>
	static ID3D12Resource* CreateUploadBuffer(int size);

	/// <summary>
	/// 用途、使用方法に合致するリソース状態を取得します。
	/// </summary>
	/// <param name="target"></param>
	/// <param name="usageFlags"></param>
	/// <returns></returns>
	static D3D12_RESOURCE_STATES GetOriginallyResourceState(Target target, UsageFlags usageFlags);

	/// <summary>
	/// 指定した領域をロックして書き込み用のアドレスを返します。
	/// </summary>
	/// <param name="bufferStartIndex"></param>
	/// <param name="count"></param>
	/// <returns></returns>
	void* InternalLockBufferForWrite(int bufferStartIndex, int count);

	/// <summary>
	/// ロックしていた領域をアンロックして、変更分をビデオメモリに転送します。
	/// </summary>
	/// <param name="countWritten"></param>
	void InternalUnlockBufferAfterWrite(int countWritten);

public:
	/// <summary>
	/// グラフィックスバッファを作成します。
	/// </summary>
	/// <param name="target"></param>
	/// <param name="usageFlags"></param>
	/// <param name="count"></param>
	/// <param name="stride"></param>
	/// <param name="initialData"></param>
	GraphicsBuffer(Target target, UsageFlags usageFlags, int count, int stride, const void* initialData = nullptr);

	/// <summary>
	/// このグラフィックスバッファの用途を表す値を取得します。
	/// </summary>
	/// <returns></returns>
	Target GetTarget() const { return m_target; }

	/// <summary>
	/// このグラフィックスバッファの使用方法を表す値を取得します。
	/// </summary>
	/// <returns></returns>
	UsageFlags GetUsageFlags() const { return m_usageFlags; }

	/// <summary>
	/// 要素数を取得します。
	/// </summary>
	/// <returns></returns>
	int GetCount() const { return m_count; }

	/// <summary>
	/// 要素1個分のバイト数を取得します。
	/// </summary>
	/// <returns></returns>
	int GetStride() const { return m_stride; }

	/// <summary>
	/// バッファ全体のバイト数を取得します。
	/// </summary>
	/// <returns></returns>
	int GetSizeInBytes() const { return m_sizeInBytes; }

	/// <summary>
	/// バッファリソースのインターフェイスを取得します。
	/// </summary>
	/// <returns>ID3D12Resource型ポインタの値を返します。</returns>
	ID3D12Resource* GetNativeBufferPtr() const { return m_nativeBuffer.Get(); }

	/// <summary>
	/// UsageFlags::LockBufferForWriteフラグ無しで作成されたバッファを更新することができます。
	/// </summary>
	/// <param name="bufferStartIndex"></param>
	/// <param name="count"></param>
	/// <param name="data"></param>
	void UpdateSubresource(int bufferStartIndex, int count, const void* data);

	/// <summary>
	/// バッファ内の指定した領域をロックして書き込み用のアドレスを返します。
	/// </summary>
	/// <param name="bufferStartIndex"></param>
	/// <param name="count"></param>
	/// <returns></returns>
	[[nodiscard]] void* LockBufferForWrite(int bufferStartIndex, int count);

	/// <summary>
	/// バッファ全体をロックして書き込み用のアドレスを返します。
	/// </summary>
	/// <returns></returns>
	[[nodiscard]] void* LockBufferForWrite() { return LockBufferForWrite(0, m_count); }

	/// <summary>
	/// ロックしていた領域をアンロックします。
	/// </summary>
	/// <param name="countWritten"></param>
	void UnlockBufferAfterWrite(int countWritten);

	/// <summary>
	/// ロックしていた領域をアンロックをアンロックします。
	/// </summary>
	void UnlockBufferAfterWrite() { UnlockBufferAfterWrite(m_lookedCount); }
};