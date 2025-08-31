#pragma once

/// <summary>
/// クラス名：AssetImporter
/// 概要：外部から素材を読み込む際の共通部分を管理する基底クラスです。
/// 用途：このクラスを継承するこでファイルのパス文字列などを管理すことができます。
/// </summary>
class AssetImporter
{
private:
	std::wstring m_assetPath;					// アセットパス：アセットファイルへのパス文字列を表します。
	std::wstring m_directoryName;				// ディレクトリ
	std::wstring m_fileName;					// ファイル名
	std::wstring m_fileNameWithoutExtension;	// ファイル名
	std::wstring m_extension;					// 拡張子
	std::wstring m_userData;					// ユーザーデータ

protected:
	// デフォルトコンストラクタ
	AssetImporter() = default;

	// 仮想デストラクタ
	virtual ~AssetImporter() = default;

public:
	/// <summary>
	/// アセットファイルへのパス文字列を設定します。
	/// </summary>
	/// <param name="path">L"○○○"の形でソリューションフォルダからのパスを指定してください。</param>
	void SetAssetPath(const wchar_t* path);

	/// <summary>
	/// アセットファイルへのパス文字列を取得します。
	/// </summary>
	/// <returns>std::wstring型のpathを返します。</returns>
	const std::wstring& GetAssetPath() const { return m_assetPath; }

	/// <summary>
	/// アセットファイルのディレクトリ名を取得します。
	/// </summary>
	/// <returns>std::wstring型のpathを返します。</returns>
	const std::wstring& GetDirectoryName() const { return m_directoryName; }

	/// <summary>
	/// アセットファイルのファイル名を取得します。
	/// </summary>
	/// <returns>std::wstring型のpathを返します。</returns>
	const std::wstring& GetFileName() const { return m_fileName; }

	/// <summary>
	/// アセットファイルの拡張子なしのファイル名を取得します。
	/// </summary>
	/// <returns>std::wstring型のpathを返します。</returns>
	const std::wstring& GetFileNameWithoutExtension() const { return m_fileNameWithoutExtension; }

	/// <summary>
	/// アセットファイルの拡張子を取得します。
	/// </summary>
	/// <returns>std::wstring型のpathを返します。</returns>
	const std::wstring& GetExtension() const { return m_extension; }

	/// <summary>
	/// 任意のユーザー文字列を設定します。
	/// </summary>
	/// <param name="userData"></param>
	void SetUserData(const wchar_t* userData) { m_userData = userData; }

	/// <summary>
	/// 任意のユーザー文字列を取得します。
	/// </summary>
	/// <returns></returns>
	const std::wstring& GetUserData() const { return m_userData; }

private:
	void SplitAssetPath();
};