#include "AssetManager.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <filesystem>
#include <vector>
#include <string>

// ワイド文字列 (wstring) をUTF-8文字列 (std::string) に変換するヘルパー関数
// WinAPIのWideCharToMultiByte関数を使用
static std::string WStringToString(const std::wstring & wstr)
{
    if (wstr.empty()) return std::string();
    // 変換に必要なバッファサイズを計算
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
    std::string strTo(size_needed, 0);
    // 実際に変換を実行
    WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
    return strTo;
}

// Assimpの行列を自作のMatrix4x4型に変換
// Assimpは行メジャー、自作クラスは列メジャーの場合があるため注意が必要
static Matrix4x4 ConvertMatrix(const aiMatrix4x4& from)
{
    Matrix4x4 to;
    // Assimpの行列から自作クラスの行列に要素をコピー
    // m[列][行]の形式でコピー
    to.m[0][0] = from.a1; to.m[1][0] = from.a2; to.m[2][0] = from.a3; to.m[3][0] = from.a4;
    to.m[0][1] = from.b1; to.m[1][1] = from.b2; to.m[2][1] = from.b3; to.m[3][1] = from.b4;
    to.m[0][2] = from.c1; to.m[1][2] = from.c2; to.m[2][2] = from.c3; to.m[3][2] = from.c4;
    to.m[0][3] = from.d1; to.m[1][3] = from.d2; to.m[2][3] = from.d3; to.m[3][3] = from.d4;
    return to;
}

// デフォルトコンストラクタ
ModelImporter::ModelImporter()
    : m_globalScale(1.0f)
    , m_calculateTangents(true)
    , m_generateNormals(true)
    , m_flipUVs(true)
    , m_joinIdenticalVertices(true)
    , m_importMaterials(true)
{
}

ModelData* ModelImporter::Import(const std::wstring& path)
{
    SetAssetPath(path.c_str());

    return Import();
}

ModelData* ModelImporter::Import()
{
    const std::wstring& path = GetAssetPath();
    if (path.empty())
    {
        return nullptr;
    }

    Assimp::Importer importer;

    // 現在の設定に基づいてインポートフラグを設定
    // aiProcess_Triangulate: すべてのプリミティブを三角形に変換
    unsigned int flags = aiProcess_Triangulate;
    if (m_flipUVs)                  flags |= aiProcess_FlipUVs;            // UV座標をY軸で反転
    if (m_generateNormals)          flags |= aiProcess_GenSmoothNormals;   // スムーズ法線を生成
    if (m_calculateTangents)        flags |= aiProcess_CalcTangentSpace;   // 接線と従法線を計算
    if (m_joinIdenticalVertices)    flags |= aiProcess_JoinIdenticalVertices; // 重複した頂点を結合

    const std::string path_s = WStringToString(path);
    if (path_s.empty())
    {
        OutputDebugStringA("ModelImporter Error: Asset path is not set or empty.\n");
        return nullptr;
    }

    // Assimpでファイルを読み込み
    const aiScene* scene = importer.ReadFile(path_s, flags);

    // インポートエラーの処理
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        OutputDebugStringA(("ASSIMP_ERROR: " + std::string(importer.GetErrorString()) + "\n").c_str());
        return nullptr;
    }

    ModelData* modelData = new ModelData();
    modelData->path = path;

    ProcessMaterials(scene, modelData);
    ProcessMeshes(scene, modelData);
    if (scene->HasAnimations())
    {
        ProcessSkeleton(scene, modelData);
        ProcessAnimations(scene, modelData);
    }
    ProcessHierarchy(scene, modelData);

    return modelData;
}

void ModelImporter::ProcessMaterials(const aiScene* scene, ModelData* modelData)
{
    modelData->materials.resize(scene->mNumMaterials);
    for (unsigned int i = 0; i < scene->mNumMaterials; ++i)
    {
        modelData->materials[i] = ProcessSingleMaterial(scene->mMaterials[i], scene);
    }
}

Material* ModelImporter::ProcessSingleMaterial(aiMaterial* mat, const aiScene* scene)
{
    Material* newMaterial = new Material();

    // ベースカラーのプロパティを読み込む
    aiColor4D color;
    if (aiGetMaterialColor(mat, AI_MATKEY_COLOR_DIFFUSE, &color) != AI_SUCCESS)
    {
        color = aiColor4D(1.0f, 1.0f, 1.0f, 1.0f);
    }
    newMaterial->SetBaseColor({ color.r, color.g, color.b, color.a });


    if (aiGetMaterialColor(mat, AI_MATKEY_COLOR_SPECULAR, &color) != AI_SUCCESS)
    {
        color = aiColor4D(1.0f, 1.0f, 1.0f, 1.0f);
    }
    newMaterial->SetColor("_SpecularColor", { color.r, color.g, color.b, color.a });

    if (aiGetMaterialColor(mat, AI_MATKEY_COLOR_AMBIENT, &color) != AI_SUCCESS)
    {
        color = aiColor4D(1.0f, 1.0f, 1.0f, 1.0f);
    }
    newMaterial->SetColor("_AmbientColor", { color.r, color.g, color.b, color.a });


    if (aiGetMaterialColor(mat, AI_MATKEY_COLOR_TRANSPARENT, &color) != AI_SUCCESS)
    {
        color = aiColor4D(1.0f, 1.0f, 1.0f, 1.0f);
    }
    newMaterial->SetColor("_TransparentColor", { color.r, color.g, color.b, color.a });

    if (aiGetMaterialColor(mat, AI_MATKEY_COLOR_REFLECTIVE, &color) != AI_SUCCESS)
    {
        color = aiColor4D(1.0f, 1.0f, 1.0f, 1.0f);
    }
    newMaterial->SetColor("_ReflectiveColor", { color.r, color.g, color.b, color.a });

    // エミッシブカラーのプロパティを読み込む
    if (aiGetMaterialColor(mat, AI_MATKEY_COLOR_EMISSIVE, &color) != AI_SUCCESS)
    {
        color = aiColor4D(0.0f, 0.0f, 0.0f, 1.0f);
    }
    newMaterial->SetColor("_EmissiveColor", { color.r, color.g, color.b, color.a });

    float fval;

    if (aiGetMaterialFloat(mat, AI_MATKEY_SHININESS, &fval) != AI_SUCCESS)
    {
        fval = 0.0f;
    }
    newMaterial->SetFloat("_Shininess", fval);

    // Roughnessの読み込み
    if (aiGetMaterialFloat(mat, AI_MATKEY_ROUGHNESS_FACTOR, &fval) != AI_SUCCESS)
    {
        fval = 0.5f;
    }
    newMaterial->SetFloat("_Roughness", fval);

    // Metallicの読み込み
    if (aiGetMaterialFloat(mat, AI_MATKEY_METALLIC_FACTOR, &fval) != AI_SUCCESS)
    {
        fval = 0.1f;
    }
    newMaterial->SetFloat("_Metallic", fval);

    if (aiGetMaterialFloat(mat, AI_MATKEY_OPACITY, &fval) != AI_SUCCESS)
    {
        fval = 1.0f;
    }
    newMaterial->SetFloat("_Opacity", fval);

    newMaterial->SetFloat("_Occlusion", 1.0f);

    // ディフューズテクスチャが存在すればロード
    if (mat->GetTextureCount(aiTextureType_DIFFUSE) > 0)
    {
        aiString path;
        mat->GetTexture(aiTextureType_DIFFUSE, 0, &path);
        Texture2D* diffuseTexture = new Texture2D();
        TextureImporter textureImporter;

        const aiTexture* embeddedTexture = scene->GetEmbeddedTexture(path.C_Str());
        if (embeddedTexture)
        {
            // 埋め込みテクスチャをロード（圧縮または非圧縮）
            if (embeddedTexture->mHeight == 0) // 圧縮形式
            {
                OutputDebugStringW((L"圧縮形式で埋め込みテクスチャを読み込み \n"));
                diffuseTexture = textureImporter.Import(embeddedTexture->pcData, embeddedTexture->mWidth);
            }
            else // 非圧縮形式
            {
                OutputDebugStringW((L"非圧縮形式で埋め込みテクスチャを読み込み \n"));

                diffuseTexture = textureImporter.Import(
                    embeddedTexture->mWidth, embeddedTexture->mHeight, DXGI_FORMAT_R8G8B8A8_UNORM,
                    embeddedTexture->pcData, embeddedTexture->mWidth * sizeof(aiTexel));
            }
        }
        else
        {
            // 外部テクスチャをファイルパスからロード
            std::filesystem::path modelPath = GetAssetPath();
            std::filesystem::path texturePath = modelPath.parent_path() / path.C_Str();

            OutputDebugStringW((L"外部テクスチャをファイルパスから" + texturePath.wstring() + L"\n").c_str());

            if (std::filesystem::exists(texturePath))
            {
                diffuseTexture = textureImporter.Import(texturePath.c_str());
            }
            else
            {
                OutputDebugStringW((L"ModelImporter Warning: External texture not found at " + texturePath.wstring() + L"\n").c_str());
            }
        }

        if (diffuseTexture)
        {
            newMaterial->SetMainTexture(diffuseTexture);
        }
    }
    else
    {
        newMaterial->SetMainTexture(AssetManager::GetInstance()->GetAsset<Texture2D>(AssetType::Texture, L"Assets/white.png"));
        OutputDebugStringW((L"ModelImporter Warning: Main texture not found at " + GetAssetPath() + L"\n").c_str());
    }

    // 法線マップ(Normal Map)の処理とフラグ設定
    if (mat->GetTextureCount(aiTextureType_NORMALS) > 0)
    {
        aiString path;
        mat->GetTexture(aiTextureType_NORMALS, 0, &path);
        Texture2D* normalTexture = new Texture2D();
        TextureImporter textureImporter;

        const aiTexture* embeddedTexture = scene->GetEmbeddedTexture(path.C_Str());
        if (embeddedTexture)
        {
            // 埋め込みテクスチャをロード（圧縮または非圧縮）
            if (embeddedTexture->mHeight == 0) // 圧縮形式
            {
                OutputDebugStringW((L"圧縮形式で埋め込みテクスチャを読み込み \n"));
                normalTexture = textureImporter.Import(embeddedTexture->pcData, embeddedTexture->mWidth);
            }
            else // 非圧縮形式
            {
                OutputDebugStringW((L"非圧縮形式で埋め込みテクスチャを読み込み \n"));

                normalTexture = textureImporter.Import(
                    embeddedTexture->mWidth, embeddedTexture->mHeight, DXGI_FORMAT_R8G8B8A8_UNORM,
                    embeddedTexture->pcData, embeddedTexture->mWidth * sizeof(aiTexel));
            }
        }
        else
        {
            // 外部テクスチャをファイルパスからロード
            std::filesystem::path modelPath = GetAssetPath();
            std::filesystem::path texturePath = modelPath.parent_path() / path.C_Str();

            OutputDebugStringW((L"外部テクスチャをファイルパスから" + texturePath.wstring() + L"\n").c_str());

            if (std::filesystem::exists(texturePath))
            {
                normalTexture = textureImporter.Import(texturePath.c_str());
            }
            else
            {
                OutputDebugStringW((L"ModelImporter Warning: External texture not found at " + texturePath.wstring() + L"\n").c_str());
            }
        }

        if (normalTexture)
        {
            newMaterial->SetTexture("_NormalTex", normalTexture);
        }
    }
    else
    {
        OutputDebugStringW((L"ModelImporter Warning: Normal texture not found at " + GetAssetPath() + L"\n").c_str());
    }

    // Metallic / Roughness Map の設定とフラグ設定
    if (mat->GetTextureCount(aiTextureType_METALNESS) > 0 || mat->GetTextureCount(aiTextureType_DIFFUSE_ROUGHNESS) > 0)
    {
        aiString path;
        aiTextureType texType = aiTextureType_UNKNOWN;

        // 優先順位: METALNESS -> DIFFUSE_ROUGHNESS
        if (mat->GetTextureCount(aiTextureType_METALNESS) > 0)
        {
            texType = aiTextureType_METALNESS;
        }
        else if (mat->GetTextureCount(aiTextureType_DIFFUSE_ROUGHNESS) > 0)
        {
            texType = aiTextureType_DIFFUSE_ROUGHNESS;
        }

        if (texType != aiTextureType_UNKNOWN)
        {
            mat->GetTexture(texType, 0, &path);

            Texture2D* metallicRoughnessTexture = nullptr;
            TextureImporter textureImporter;

            // モデルファイルに埋め込まれたテクスチャの処理
            const aiTexture* embeddedTexture = scene->GetEmbeddedTexture(path.C_Str());
            if (embeddedTexture)
            {
                // 埋め込みテクスチャをロード（圧縮または非圧縮）
                if (embeddedTexture->mHeight == 0) // 圧縮形式
                {
                    metallicRoughnessTexture = textureImporter.Import(embeddedTexture->pcData, embeddedTexture->mWidth);
                }
                else // 非圧縮形式 (aiTexel* を利用)
                {
                    metallicRoughnessTexture = textureImporter.Import(
                        embeddedTexture->mWidth, embeddedTexture->mHeight, DXGI_FORMAT_R8G8B8A8_UNORM,
                        embeddedTexture->pcData, embeddedTexture->mWidth * sizeof(aiTexel));
                }
            }
            else
            {
                // 外部テクスチャをファイルパスからロード
                std::filesystem::path modelPath = GetAssetPath();
                std::filesystem::path texturePath = modelPath.parent_path() / path.C_Str();

                if (std::filesystem::exists(texturePath))
                {
                    metallicRoughnessTexture = textureImporter.Import(texturePath.c_str());
                }
                else
                {
                    OutputDebugStringW((L"ModelImporter Warning: Metallic/Roughness texture not found at " + texturePath.wstring() + L"\n").c_str());
                }
            }

            if (metallicRoughnessTexture)
            {
                newMaterial->SetTexture("_MetallicRoughnessTex", metallicRoughnessTexture);
            }
        }
    }
    else
    {
        OutputDebugStringW((L"ModelImporter Warning: Metallic / Roughness texture not found at " + GetAssetPath() + L"\n").c_str());
    }

    // AOテクスチャの設定
    if (mat->GetTextureCount(aiTextureType_AMBIENT_OCCLUSION) > 0)
    {
        aiString path;
        mat->GetTexture(aiTextureType_AMBIENT_OCCLUSION, 0, &path);
        Texture2D* aoTexture = new Texture2D();
        TextureImporter textureImporter;

        const aiTexture* embeddedTexture = scene->GetEmbeddedTexture(path.C_Str());
        if (embeddedTexture)
        {
            // 埋め込みテクスチャをロード（圧縮または非圧縮）
            if (embeddedTexture->mHeight == 0) // 圧縮形式
            {
                OutputDebugStringW((L"圧縮形式で埋め込みテクスチャを読み込み \n"));
                aoTexture = textureImporter.Import(embeddedTexture->pcData, embeddedTexture->mWidth);
            }
            else // 非圧縮形式
            {
                OutputDebugStringW((L"非圧縮形式で埋め込みテクスチャを読み込み \n"));

                aoTexture = textureImporter.Import(
                    embeddedTexture->mWidth, embeddedTexture->mHeight, DXGI_FORMAT_R8G8B8A8_UNORM,
                    embeddedTexture->pcData, embeddedTexture->mWidth * sizeof(aiTexel));
            }
        }
        else
        {
            // 外部テクスチャをファイルパスからロード
            std::filesystem::path modelPath = GetAssetPath();
            std::filesystem::path texturePath = modelPath.parent_path() / path.C_Str();

            OutputDebugStringW((L"外部テクスチャをファイルパスから" + texturePath.wstring() + L"\n").c_str());

            if (std::filesystem::exists(texturePath))
            {
                aoTexture = textureImporter.Import(texturePath.c_str());
            }
            else
            {
                OutputDebugStringW((L"ModelImporter Warning: External texture not found at " + texturePath.wstring() + L"\n").c_str());
            }
        }

        if (aoTexture)
        {
            newMaterial->SetTexture("_AOTex", aoTexture);
        }
    }
    else
    {
        OutputDebugStringW((L"ModelImporter Warning: AO texture not found at " + GetAssetPath() + L"\n").c_str());
    }

    aiBlendMode blendMode;
    // aiGetMaterialIntegerでブレンドモードを取得し、AlphaTest/AlphaBlendのフラグを設定するロジックを追加
    if (aiGetMaterialInteger(mat, AI_MATKEY_BLEND_FUNC, (int*)&blendMode) == AI_SUCCESS)
    {
        if (blendMode == aiBlendMode_Additive || blendMode == aiBlendMode_Default)
        {
            // ここで不透明度やカットアウトの閾値（AI_MATKEY_OPACITYやAI_MATKEY_ALPHA_TEST）もチェックする
            float opacity = 1.0f;
            aiGetMaterialFloat(mat, AI_MATKEY_OPACITY, &opacity);
        }
    }

    return newMaterial;
}

void ModelImporter::ProcessMeshes(const aiScene* scene, ModelData* modelData)
{
    modelData->meshes.resize(scene->mNumMeshes);
    for (unsigned int i = 0; i < scene->mNumMeshes; ++i)
    {
        aiMesh* ai_mesh = scene->mMeshes[i];
        Mesh* newMesh = new Mesh();

        std::vector<Mesh::Vertex> vertices(ai_mesh->mNumVertices);
        for (unsigned int v = 0; v < ai_mesh->mNumVertices; ++v)
        {
            vertices[v].position = { ai_mesh->mVertices[v].x, ai_mesh->mVertices[v].y, ai_mesh->mVertices[v].z };
            if (ai_mesh->HasNormals())
            {
                vertices[v].normal = { ai_mesh->mNormals[v].x, ai_mesh->mNormals[v].y, ai_mesh->mNormals[v].z };
            }
            if (ai_mesh->HasTextureCoords(0))
            {
                vertices[v].uv = { ai_mesh->mTextureCoords[0][v].x, ai_mesh->mTextureCoords[0][v].y };
            }
            if (ai_mesh->HasTangentsAndBitangents())
            {
                vertices[v].tangent = { ai_mesh->mTangents[v].x, ai_mesh->mTangents[v].y, ai_mesh->mTangents[v].z };
            }
        }

        std::vector<uint32_t> indices;
        for (unsigned int f = 0; f < ai_mesh->mNumFaces; ++f)
        {
            for (unsigned int j = 0; j < ai_mesh->mFaces[f].mNumIndices; ++j)
            {
                indices.push_back(ai_mesh->mFaces[f].mIndices[j]);
            }
        }

        newMesh->AddVertices(vertices);
        newMesh->AddIndices(indices);
        newMesh->AddSubMesh(0, (UINT)indices.size(), 0);

        if (!scene->HasAnimations())
        {
            newMesh->SetupMesh();
        }

        modelData->meshes[i] = newMesh;
    }
}

void ModelImporter::ProcessSkeleton(const aiScene* scene, ModelData* modelData)
{
    Skeleton* skeleton = new Skeleton();
    skeleton->SetGlobalInverseTransform(ConvertMatrix(scene->mRootNode->mTransformation).Inverse());

    Bone* rootBone = new Bone();
    skeleton->SetRootBone(rootBone);
    ReadSkeletonHierarchy(rootBone, scene->mRootNode);

    for (unsigned int i = 0; i < scene->mNumMeshes; ++i)
    {
        if (scene->mMeshes[i]->HasBones())
        {
            LoadBones(modelData->meshes[i]->GetMutableVertices(), scene->mMeshes[i], skeleton);
        }
        modelData->meshes[i]->SetupMesh();
    }

    modelData->skeleton = skeleton;
}

void ModelImporter::LoadBones(std::vector<Mesh::Vertex>& vertices, aiMesh* mesh, Skeleton* skeleton)
{
    auto& boneInfoMap = skeleton->GetMutableBoneInfoMap();
    int& boneCount = skeleton->GetMutableBoneCount();

    for (unsigned int i = 0; i < mesh->mNumBones; i++)
    {
        aiBone* bone = mesh->mBones[i];
        std::string boneName = bone->mName.C_Str();
        int boneIndex = 0;

        if (boneInfoMap.find(boneName) == boneInfoMap.end())
        {
            BoneInfo newBoneInfo;
            newBoneInfo.id = boneCount;
            newBoneInfo.offsetMatrix = ConvertMatrix(bone->mOffsetMatrix);
            boneInfoMap[boneName] = newBoneInfo;
            boneIndex = boneCount;
            boneCount++;
        }
        else
        {
            boneIndex = boneInfoMap[boneName].id;
        }

        for (unsigned int j = 0; j < bone->mNumWeights; j++)
        {
            const aiVertexWeight weight = bone->mWeights[j];
            for (int k = 0; k < MAX_BONE_INFLUENCE; k++)
            {
                if (vertices[weight.mVertexId].weights[k] == 0.0f)
                {
                    vertices[weight.mVertexId].boneIDs[k] = boneIndex;
                    vertices[weight.mVertexId].weights[k] = weight.mWeight;
                    break;
                }
            }
        }
    }
}

void ModelImporter::ReadSkeletonHierarchy(Bone* parentBone, const aiNode* node)
{
    parentBone->name = node->mName.C_Str();
    parentBone->localTransform = ConvertMatrix(node->mTransformation);
    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        Bone* newBone = new Bone();
        parentBone->children.push_back(newBone);
        ReadSkeletonHierarchy(newBone, node->mChildren[i]);
    }
}

void ModelImporter::ProcessAnimations(const aiScene* scene, ModelData* modelData)
{
    for (unsigned int i = 0; i < scene->mNumAnimations; ++i)
    {
        aiAnimation* ai_animation = scene->mAnimations[i];
        Animation* newAnimation = new Animation();

        std::string animName = ai_animation->mName.C_Str();
        size_t pipePos = animName.find('|');
        if (pipePos != std::string::npos) { animName = animName.substr(pipePos + 1); }
        newAnimation->SetName(animName);
        newAnimation->SetDuration((float)ai_animation->mDuration);
        newAnimation->SetTicksPerSecond(ai_animation->mTicksPerSecond != 0 ? (float)ai_animation->mTicksPerSecond : 24.0f);

        for (unsigned int j = 0; j < ai_animation->mNumChannels; ++j)
        {
            aiNodeAnim* channel = ai_animation->mChannels[j];
            BoneAnimation boneAnim;
            for (unsigned int k = 0; k < channel->mNumPositionKeys; ++k)
            {
                boneAnim.positions.push_back(
                    {
                        {
                            channel->mPositionKeys[k].mValue.x,
                            channel->mPositionKeys[k].mValue.y,
                            channel->mPositionKeys[k].mValue.z
                        },
                        (float)channel->mPositionKeys[k].mTime
                    }
                );
            }
            for (unsigned int k = 0; k < channel->mNumRotationKeys; ++k)
            {
                boneAnim.rotations.push_back(
                    {
                        {
                            channel->mRotationKeys[k].mValue.x,
                            channel->mRotationKeys[k].mValue.y,
                            channel->mRotationKeys[k].mValue.z,
                            channel->mRotationKeys[k].mValue.w},
                            (float)channel->mRotationKeys[k].mTime
                    }
                );
            }
            for (unsigned int k = 0; k < channel->mNumScalingKeys; ++k)
            {
                boneAnim.scales.push_back(
                    {
                        {
                            channel->mScalingKeys[k].mValue.x,
                            channel->mScalingKeys[k].mValue.y,
                            channel->mScalingKeys[k].mValue.z},
                            (float)channel->mScalingKeys[k].mTime
                    }
                );
            }
            newAnimation->GetMutableBoneAnimations()[channel->mNodeName.C_Str()] = boneAnim;
        }
        modelData->animations[animName] = newAnimation;
    }
}

static void DebugPrintAssimpNode(const aiNode* node, int level)
{
    if (!node) return;

    // インデント生成
    std::string indent(level * 2, ' ');

    // ノード名取得
    std::string name = node->mName.C_Str();
    if (name.empty()) name = "[No Name]";

    // デバッグ出力 (Meshがある場合はその数も表示してみる)
    std::string log = indent + "- " + name;
    if (node->mNumMeshes > 0)
    {
        log += " (Meshes: " + std::to_string(node->mNumMeshes) + ")";
    }
    log += "\n";

    OutputDebugStringA(log.c_str());

    // 子ノードへ再帰
    for (unsigned int i = 0; i < node->mNumChildren; ++i)
    {
        DebugPrintAssimpNode(node->mChildren[i], level + 1);
    }
}

void ModelImporter::ProcessHierarchy(const aiScene* scene, ModelData* modelData)
{
    OutputDebugStringA("\n========== [ModelImporter] Assimp Node Hierarchy START ==========\n");
    DebugPrintAssimpNode(scene->mRootNode, 0);
    OutputDebugStringA("========== [ModelImporter] Assimp Node Hierarchy END ============\n\n");

    modelData->rootNode = std::make_unique<PrefabNode>();
    BuildPrefabNode(modelData->rootNode, scene->mRootNode, modelData);
}

void ModelImporter::BuildPrefabNode(std::unique_ptr<PrefabNode>& outNode, const aiNode* inNode, const ModelData* modelData)
{
    outNode->name = inNode->mName.C_Str();
    outNode->transform = ConvertMatrix(inNode->mTransformation);
    for (unsigned int i = 0; i < inNode->mNumMeshes; ++i)
    {
        outNode->meshIndices.push_back(inNode->mMeshes[i]);
    }
    for (unsigned int i = 0; i < inNode->mNumChildren; ++i)
    {
        auto child = std::make_unique<PrefabNode>();
        BuildPrefabNode(child, inNode->mChildren[i], modelData);
        outNode->children.push_back(std::move(child));
    }
}