#include "Precompiled.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <filesystem> // C++17 filesystem: パス操作に便利

std::unordered_map<std::wstring, ComPtr<Model>> ModelImporter::s_modelCache;

// string (UTF-8) と wstring を相互変換するユーティリティ
static std::string WStringToString(const std::wstring& wstr)
{
    if (wstr.empty()) return std::string();
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
    std::string strTo(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
    return strTo;
}

static Matrix4x4 ConvertMatrix(const aiMatrix4x4& from)
{
    Matrix4x4 to;
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

// パスを指定してインポートするユーティリティ関数
ComPtr<Model> ModelImporter::Import(const std::wstring& path, World& world)
{
    SetAssetPath(path.c_str());
    return Import(world);
}

// メインのインポート関数
ComPtr<Model> ModelImporter::Import(World& world)
{
    const std::wstring& path = GetAssetPath();
    if (path.empty())
    {
        return nullptr;
    }

    auto it = s_modelCache.find(path);
    if (it != s_modelCache.end())
    {
        return it->second;
    }

    Assimp::Importer importer;

    // 設定に基づいて動的に前処理フラグを構築
    unsigned int flags = aiProcess_Triangulate;
    if (m_flipUVs)                 flags |= aiProcess_FlipUVs;
    if (m_generateNormals)         flags |= aiProcess_GenSmoothNormals;
    if (m_calculateTangents)       flags |= aiProcess_CalcTangentSpace;
    if (m_joinIdenticalVertices)   flags |= aiProcess_JoinIdenticalVertices;

    const std::string path_s = WStringToString(path);
    if (path_s.empty())
    {
        OutputDebugStringA("ModelImporter Error: Asset path is not set or empty.\n");
        return nullptr;
    }

    const aiScene* scene = importer.ReadFile(path_s, flags);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        OutputDebugStringA(("ASSIMP_ERROR: " + std::string(importer.GetErrorString()) + "\n").c_str());
        return nullptr;
    }

    ComPtr<Model> modelData;
    modelData.Attach(new Model());

    modelData->m_mesh.Attach(new Mesh());
    modelData->m_skeleton.Attach(new Skeleton());

    modelData->m_skeleton->SetGlobalInverseTransform(ConvertMatrix(scene->mRootNode->mTransformation).Inverse());

    // マテリアルの読み込み
    if (m_importMaterials && scene->mNumMaterials > 0)
    {
        modelData->m_materials.resize(scene->mNumMaterials);
        for (unsigned int i = 0; i < scene->mNumMaterials; i++)
        {
            modelData->m_materials[i] = ProcessMaterial(scene->mMaterials[i], scene, world.GetSrvAllocator());
        }
    }

    modelData->m_mesh.Attach(new Mesh());

    // ノードを再帰的に処理し、すべてのメッシュデータを単一のベクトルに集約
    ProcessNode(scene->mRootNode, scene, modelData.Get());

    Bone* rootBone = new Bone();
    modelData->m_skeleton->SetRootBone(rootBone);
    ReadSkeletonHierarchy(rootBone, scene->mRootNode);

    ProcessAnimations(scene, modelData.Get());

    modelData->m_mesh->SetupMesh();

    s_modelCache[path] = modelData;

    return modelData;
}

// ノードを再帰的に処理
void ModelImporter::ProcessNode(aiNode* node, const aiScene* scene, Model* modelData)
{
    // このノードに属する全てのメッシュを処理
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];

        const UINT baseVertexLocation = modelData->m_mesh->GetVertexCount();
        const UINT startIndex = modelData->m_mesh->GetTotalIndexCount();

        // 頂点とインデックスのデータを一時的に格納するベクトル
        std::vector<Mesh::Vertex> vertices;
        std::vector<uint32_t> indices;

        // 頂点を抽出
        vertices.resize(mesh->mNumVertices);
        for (unsigned int v = 0; v < mesh->mNumVertices; v++)
        {
            vertices[v].position = {
                mesh->mVertices[v].x * m_globalScale,
                mesh->mVertices[v].y * m_globalScale,
                mesh->mVertices[v].z * m_globalScale
            };
            if (mesh->HasNormals()) {
                vertices[v].normal = { mesh->mNormals[v].x, mesh->mNormals[v].y, mesh->mNormals[v].z };
            }
            if (mesh->HasTextureCoords(0)) {
                vertices[v].uv = { mesh->mTextureCoords[0][v].x, mesh->mTextureCoords[0][v].y };
            }
            if (mesh->HasTangentsAndBitangents()) {
                vertices[v].tangent = { mesh->mTangents[v].x, mesh->mTangents[v].y, mesh->mTangents[v].z };
            }
        }

        if (mesh->HasBones())
        {
            LoadBones(vertices, mesh, modelData->m_skeleton.Get());
        }

        // インデックスを抽出
        for (unsigned int f = 0; f < mesh->mNumFaces; f++)
        {
            aiFace face = mesh->mFaces[f];
            for (unsigned int j = 0; j < face.mNumIndices; j++)
            {
                indices.push_back(face.mIndices[j]);
            }
        }

        modelData->m_mesh->AddVertices(vertices);
        modelData->m_mesh->AddIndices(indices);
        modelData->m_mesh->AddSubMesh(startIndex, (UINT)indices.size(), mesh->mMaterialIndex);
    }

    // 子ノードを再帰的に処理
    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        ProcessNode(node->mChildren[i], scene, modelData);
    }
}

void ModelImporter::ProcessMesh(aiMesh* mesh, const aiScene* scene, Model* modelData)
{
}

void ModelImporter::ProcessAnimations(const aiScene* scene, Model* modelData)
{
    if (!scene->HasAnimations())
    {
        return;
    }

    for (unsigned int i = 0; i < scene->mNumAnimations; i++)
    {
        aiAnimation* animation = scene->mAnimations[i];

        // animationポインタが有効かチェック
        if (!animation)
        {
            return;
        }

        ComPtr<Animation> newAnimation;
        newAnimation.Attach(new Animation());

        newAnimation->SetName(animation->mName.C_Str());
        newAnimation->SetDuration((float)animation->mDuration);

        // mTicksPerSecondが0の場合、デフォルト値（例: 24.0f）を設定
        newAnimation->SetTicksPerSecond(animation->mTicksPerSecond !=  0 ? (float)animation->mTicksPerSecond : 24.0f);

        // アニメーションチャンネルが存在するかチェック
        if (animation->mNumChannels == 0 || !animation->mChannels)
        {
            continue;
        }

        for (unsigned int j = 0; j < animation->mNumChannels; j++)
        {
            aiNodeAnim* channel = animation->mChannels[j];

            // channelポインタが有効かチェック
            if (!channel || !channel->mNodeName.C_Str())
            {
                continue;
            }

            BoneAnimation boneAnim;

            // 位置キーフレームの読み込み
            if (channel->mNumPositionKeys > 0 && channel->mPositionKeys)
            {
                for (unsigned int k = 0; k < channel->mNumPositionKeys; k++)
                {
                    aiVectorKey key = channel->mPositionKeys[k];
                    boneAnim.positions.push_back({ {key.mValue.x, key.mValue.y, key.mValue.z}, (float)key.mTime });
                }
            }

            // 回転キーフレームの読み込み
            if (channel->mNumRotationKeys > 0 && channel->mRotationKeys)
            {
                for (unsigned int k = 0; k < channel->mNumRotationKeys; k++)
                {
                    aiQuatKey key = channel->mRotationKeys[k];
                    boneAnim.rotations.push_back({ {key.mValue.x, key.mValue.y, key.mValue.z, key.mValue.w}, (float)key.mTime });
                }
            }

            // スケールキーフレームの読み込み
            if (channel->mNumScalingKeys > 0 && channel->mNumScalingKeys)
            {
                for (unsigned int k = 0; k < channel->mNumScalingKeys; k++)
                {
                    aiVectorKey key = channel->mScalingKeys[k];
                    boneAnim.scales.push_back({ {key.mValue.x, key.mValue.y, key.mValue.z}, (float)key.mTime });
                }
            }

            // このボーンに有効なキーフレーム1つでもあれば、アニメーションに追加
            if (!boneAnim.positions.empty() || !boneAnim.rotations.empty() || !boneAnim.scales.empty())
            {
                newAnimation->GetMutableBoneAnimations()[channel->mNodeName.C_Str()] = boneAnim;
            }
        }

        // このアニメーションクリップに有効なボーンアニメーションが1つでもあれば、モデルに追加
        if (newAnimation->GetBoneAnimations().size() > 0)
        {
            modelData->m_animations.push_back(newAnimation);
        }
    }
}

void ModelImporter::ReadSkeletonHierarchy(Bone* parentBone, const aiNode* node)
{
    assert(parentBone);

    parentBone->name = node->mName.C_Str();
    parentBone->localTransform = ConvertMatrix(node->mTransformation);

    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        Bone* newBone = new Bone();
        parentBone->children.push_back(newBone);
        ReadSkeletonHierarchy(newBone, node->mChildren[i]);
    }
}

void ModelImporter::LoadBones(std::vector<Mesh::Vertex>& vertices, aiMesh* mesh, Skeleton* skeleton)
{
    auto& boneInfoMap = skeleton->GetMutableBoneInfoMap();
    int boneCount = skeleton->GetBoneCount();

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
            const aiVertexWeight& weight = bone->mWeights[j];
            unsigned int vertexId = weight.mVertexId;
            float vertexWeight = weight.mWeight;

            for (int k = 0; k < MAX_BONE_INFLUENCE; k++)
            {
                if (vertices[vertexId].weights[k] == 0.0f)
                {
                    vertices[vertexId].boneIDs[k] = boneIndex;
                    vertices[vertexId].weights[k] = vertexWeight;
                    break;
                }
            }
        }
    }
    skeleton->SetBoneCount(boneCount);
}

// マテリアルデータを抽出
ComPtr<Material> ModelImporter::ProcessMaterial(aiMaterial* mat, const aiScene* scene, DescriptorAllocator* srvAllocator)
{
    ComPtr<Material> newMaterial;
    newMaterial.Attach(new Material());

    // 色のプロパティ
    aiColor4D color;
    if (aiGetMaterialColor(mat, AI_MATKEY_COLOR_DIFFUSE, &color) == AI_SUCCESS) {
        newMaterial->SetDiffuseColor({ color.r, color.g, color.b, color.a });
    }

    // Diffuseテクスチャの処理
    if (mat->GetTextureCount(aiTextureType_DIFFUSE) > 0)
    {
        aiString path;
        mat->GetTexture(aiTextureType_DIFFUSE, 0, &path);
        ComPtr<Texture2D> diffuseTexture;
        TextureImporter textureImporter;

        const aiTexture* embeddedTexture = scene->GetEmbeddedTexture(path.C_Str());
        if (embeddedTexture)
        {
            // 埋め込みテクスチャをメモリからロード
            if (embeddedTexture->mHeight == 0) { // 圧縮形式
                diffuseTexture.Attach(textureImporter.Import(embeddedTexture->pcData, embeddedTexture->mWidth));
            }
            else { // 非圧縮形式
                diffuseTexture.Attach(textureImporter.Import(
                    embeddedTexture->mWidth, embeddedTexture->mHeight, DXGI_FORMAT_R8G8B8A8_UNORM,
                    embeddedTexture->pcData, embeddedTexture->mWidth * sizeof(aiTexel)));
            }
        }
        else
        {
            // 外部テクスチャをファイルパスから解決してロード
            std::filesystem::path modelPath = GetAssetPath();
            std::filesystem::path texturePath = modelPath.parent_path() / path.C_Str();

            if (std::filesystem::exists(texturePath)) {
                diffuseTexture.Attach(textureImporter.Import(texturePath.c_str()));
            }
            else {
                OutputDebugStringW((L"ModelImporter Warning: External texture not found at " + texturePath.wstring() + L"\n").c_str());
            }
        }

        if (diffuseTexture) {
            newMaterial->SetTexture(Material::TextureSlot::Diffuse, diffuseTexture.Get(), srvAllocator);
        }
    }

    return newMaterial;
}