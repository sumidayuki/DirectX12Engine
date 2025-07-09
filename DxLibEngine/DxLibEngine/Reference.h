#pragma once

/// <summary>
/// 参照カウント方式のメモリ管理クラスです。
/// このオブジェクトを参照しているものが1つ以上存在する場合、このオブジェクトはメモリに残り続けます。
/// 参照カウントが 0 になったとき、自動的に自身を delete してメモリから解放されます。
/// 
/// IUnknown を継承することで COM の準拠のインターフェイスを持つようになり、
/// COMオブジェクトと同様に扱うことができます。（AddRef、Release、QueryInterFaceの提供）
/// </summary>
class Reference : IUnknown	
{
private:
	// 参照カウント
	ULONG m_referenceCount;

protected:
	// デフォルトコンストラクタ
	Reference();

	// 仮想デストラクタ
	virtual ~Reference() = default;

public:
	/// <summary>
	/// 参照カウントを取得します。
	/// </summary>
	/// <returns></returns>
	ULONG GetReferenceCount() const { return m_referenceCount; }

	/// <summary>
	/// 参照カウントを 1 増やします。
	/// </summary>
	/// <returns></returns>
	ULONG AddRef() override { return ++m_referenceCount; }

	/// <summary>
	/// 参照カウントを 1 減らし、0になった場合はこのインスタンスを delete します。
	/// </summary>
	/// <returns></returns>
	ULONG Release() override;

	/// <summary>
	/// インターフェイスIDを指定して、インターフェイスを取得します。
	/// </summary>
	/// <param name="interfaceID"></param>
	/// <param name="ppInterface"></param>
	/// <returns></returns>
	HRESULT QueryInterface(const IID& interfaceID, void** ppInterface) override;
};