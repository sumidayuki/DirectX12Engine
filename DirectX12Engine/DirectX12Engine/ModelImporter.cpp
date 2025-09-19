#include "ModelImporter.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <filesystem>
#include <vector>
#include <string>

// ワイド文字列 (wstring) をUTF-8文字列 (std::string) に変換するヘルパー関数
// WinAPIのWideCharToMultiByte関数を使用
static std::string WStringToString(const std::wstring& wstr)
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

// 指定されたパスからモデルをインポートする便利な関数
Entity* ModelImporter::Import(const std::wstring& path, World& world)
{
    SetAssetPath(path.c_str());
    return Import(world);
}

// モデルのインポート処理本体
Entity* ModelImporter::Import(World& world)
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
    if (m_flipUVs)           flags |= aiProcess_FlipUVs;            // UV座標をY軸で反転
    if (m_generateNormals)    flags |= aiProcess_GenSmoothNormals;   // スムーズ法線を生成
    if (m_calculateTangents)  flags |= aiProcess_CalcTangentSpace;   // 接線と従法線を計算
    if (m_joinIdenticalVertices) flags |= aiProcess_JoinIdenticalVertices; // 重複した頂点を結合

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

    std::filesystem::path modelPath(path);
    // モデルのファイル名からルートエンティティを作成
    Entity* rootEntity = world.CreateEntity(modelPath.stem().string());

    std::vector<Material*> materials;
    // マテリアルをインポートする設定の場合、マテリアルを処理
    if (m_importMaterials && scene->mNumMaterials > 0)
    {
        materials.resize(scene->mNumMaterials);
        for (unsigned int i = 0; i < scene->mNumMaterials; i++)
        {
            materials[i] = ProcessMaterial(scene->mMaterials[i], scene, world.GetSrvAllocator());
        }
    }

    Skeleton* skeleton = new Skeleton();

    // アニメーションが存在する場合、スケルトンとアニメーションを処理
    if (scene->HasAnimations())
    {
        // グローバル逆変換行列を設定
        skeleton->SetGlobalInverseTransform(ConvertMatrix(scene->mRootNode->mTransformation).Inverse());

        Bone* rootBone = new Bone();
        skeleton->SetRootBone(rootBone);
        // Assimpのノード構造からスケルトン階層を再帰的に読み込む
        ReadSkeletonHierarchy(rootBone, scene->mRootNode);

        std::vector<Animation*> animations;
        // アニメーションデータを処理
        ProcessAnimations(scene, animations);

        Animator animator;
        animator.skeleton = skeleton;

        // アニメーションクリップをAnimatorに追加
        for (const auto& anim : animations)
        {
            animator.clips[anim->GetName()] = anim;
        }
        // 最初のアニメーションをデフォルトのクリップとして設定
        if (!animator.clips.empty())
        {
            const auto& firstClip = animations[0];
            animator.currentClip = firstClip;
            animator.currentClipName = firstClip->GetName();
        }
        world.AddComponent<Animator>(*rootEntity, animator);
    }
    // ルートノードから再帰的にノードを処理してメッシュを生成
    ProcessNode(scene->mRootNode, scene, world, rootEntity, materials, skeleton);

    // インポートされた階層構造をデバッグ出力
    PrintHierarchy(world.GetComponent<Transform>(*rootEntity), 0);

    return rootEntity;
}

// ノードとそれに付随するメッシュを処理する再帰関数
void ModelImporter::ProcessNode(aiNode* node, const aiScene* scene, World& world, Entity* parentEntity, const std::vector<Material*>& materials, Skeleton* skeleton)
{
    Entity* nodeEntity = world.CreateEntity(node->mName.C_Str());

    Transform* transform = world.GetComponent<Transform>(*nodeEntity);

    // 親子関係を設定
    if (parentEntity)
    {
        Transform* parentTransform = world.GetComponent<Transform>(*parentEntity);
        TransformSystem::SetParent(*transform, parentTransform);
    }

    // Assimpのノードのローカル変換行列から、位置、回転、スケールを分解して設定
    Matrix4x4 localTransform = ConvertMatrix(node->mTransformation);
    localTransform.Decompose(transform->scale, transform->rotation, transform->position);

    // このノードに付随するすべてのメッシュを処理
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        Mesh* newMesh = new Mesh();

        std::vector<Mesh::Vertex> vertices;
        std::vector<uint32_t> indices;

        // 頂点データのロード
        vertices.resize(mesh->mNumVertices);
        for (unsigned int v = 0; v < mesh->mNumVertices; v++)
        {
            vertices[v].position = { mesh->mVertices[v].x * m_globalScale, mesh->mVertices[v].y * m_globalScale, mesh->mVertices[v].z * m_globalScale };
            if (mesh->HasNormals())
            {
                vertices[v].normal = { mesh->mNormals[v].x, mesh->mNormals[v].y, mesh->mNormals[v].z };
            }
            if (mesh->HasTextureCoords(0))
            {
                vertices[v].uv = { mesh->mTextureCoords[0][v].x, mesh->mTextureCoords[0][v].y };
            }
            if (mesh->HasTangentsAndBitangents())
            {
                vertices[v].tangent = { mesh->mTangents[v].x, mesh->mTangents[v].y, mesh->mTangents[v].z };
            }
        }

        // ボーンウェイトのロード
        if (skeleton && mesh->HasBones())
        {
            LoadBones(vertices, mesh, skeleton);
        }

        // インデックスデータのロード（メッシュの面情報）
        for (unsigned int f = 0; f < mesh->mNumFaces; f++)
        {
            aiFace face = mesh->mFaces[f];
            for (unsigned int j = 0; j < face.mNumIndices; j++)
            {
                indices.push_back(face.mIndices[j]);
            }
        }

        // 独自のMeshクラスにデータを設定
        newMesh->AddVertices(vertices);
        newMesh->AddIndices(indices);
        newMesh->AddSubMesh(0, (UINT)indices.size(), 0);
        newMesh->SetupMesh();

        // ボーンがあればSkinnedMeshRendererを、なければMeshRendererとMeshFilterを追加
        if (skeleton && mesh->HasBones())
        {
            SkinnedMeshRenderer renderer;
            renderer.mesh = newMesh;
            if (mesh->mMaterialIndex < materials.size())
            {
                renderer.materials.push_back(materials[mesh->mMaterialIndex]);
            }
            world.AddComponent<SkinnedMeshRenderer>(*nodeEntity, renderer);
        }
        else
        {
            MeshFilter filter;
            filter.mesh = newMesh;
            world.AddComponent<MeshFilter>(*nodeEntity, filter);

            MeshRenderer renderer;
            if (mesh->mMaterialIndex < materials.size())
            {
                renderer.materials.push_back(materials[mesh->mMaterialIndex]);
            }
            world.AddComponent<MeshRenderer>(*nodeEntity, renderer);
        }
    }

    // 子ノードを再帰的に処理
    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        ProcessNode(node->mChildren[i], scene, world, nodeEntity, materials, skeleton);
    }
}

// アニメーションを処理して独自のAnimationクラスに変換
void ModelImporter::ProcessAnimations(const aiScene* scene, std::vector<Animation*>& animations)
{
    if (!scene->HasAnimations())
    {
        return;
    }

    for (unsigned int i = 0; i < scene->mNumAnimations; i++)
    {
        aiAnimation* animation = scene->mAnimations[i];

        // アニメーションポインタを検証
        if (!animation)
        {
            return;
        }

        Animation* newAnimation = new Animation();

        // アニメーション名からプレフィックス（例：Blender由来の'|'）を削除して設定
        std::string originalName = animation->mName.C_Str();
        size_t pipePos = originalName.find('|');
        if (pipePos != std::string::npos)
        {
            newAnimation->SetName(originalName.substr(pipePos + 1));
        }
        else
        {
            newAnimation->SetName(originalName);
        }

        newAnimation->SetDuration((float)animation->mDuration);

        // ticks per secondを設定。指定がない場合はデフォルトの24.0fを使用
        newAnimation->SetTicksPerSecond(animation->mTicksPerSecond != 0 ? (float)animation->mTicksPerSecond : 24.0f);

        // 有効なアニメーションチャンネルが存在するか確認
        if (animation->mNumChannels == 0 || !animation->mChannels)
        {
            continue;
        }

        // 各アニメーションチャンネル（ボーンアニメーション）を処理
        for (unsigned int j = 0; j < animation->mNumChannels; j++)
        {
            aiNodeAnim* channel = animation->mChannels[j];

            // チャンネルポインタを検証
            if (!channel || !channel->mNodeName.C_Str())
            {
                continue;
            }

            BoneAnimation boneAnim;

            // 位置のキーフレームをロード
            if (channel->mNumPositionKeys > 0 && channel->mPositionKeys)
            {
                for (unsigned int k = 0; k < channel->mNumPositionKeys; k++)
                {
                    aiVectorKey key = channel->mPositionKeys[k];
                    boneAnim.positions.push_back({ {key.mValue.x, key.mValue.y, key.mValue.z}, (float)key.mTime });
                }
            }

            // 回転のキーフレームをロード
            if (channel->mNumRotationKeys > 0 && channel->mRotationKeys)
            {
                for (unsigned int k = 0; k < channel->mNumRotationKeys; k++)
                {
                    aiQuatKey key = channel->mRotationKeys[k];
                    boneAnim.rotations.push_back({ {key.mValue.x, key.mValue.y, key.mValue.z, key.mValue.w}, (float)key.mTime });
                }
            }

            // スケーリングのキーフレームをロード
            if (channel->mNumScalingKeys > 0 && channel->mNumScalingKeys)
            {
                for (unsigned int k = 0; k < channel->mNumScalingKeys; k++)
                {
                    aiVectorKey key = channel->mScalingKeys[k];
                    boneAnim.scales.push_back({ {key.mValue.x, key.mValue.y, key.mValue.z}, (float)key.mTime });
                }
            }

            // 1つ以上のキーフレームがあれば、アニメーションに追加
            if (!boneAnim.positions.empty() || !boneAnim.rotations.empty() || !boneAnim.scales.empty())
            {
                newAnimation->GetMutableBoneAnimations()[channel->mNodeName.C_Str()] = boneAnim;
            }
        }

        // アニメーションクリップに1つ以上のボーンアニメーションがあれば、モデルに追加
        if (newAnimation->GetBoneAnimations().size() > 0)
        {
            animations.push_back(newAnimation);
        }
    }

    std::wstring debugMsg = L"ModelImporter: Processed " + std::to_wstring(animations.size()) + L" animations for model.\n";
    OutputDebugStringW(debugMsg.c_str());
}

// Assimpのノード構造からスケルトン階層を再帰的に読み込む
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

// ボーンウェイトとIDを各頂点にロードする
void ModelImporter::LoadBones(std::vector<Mesh::Vertex>& vertices, aiMesh* mesh, Skeleton* skeleton)
{
    auto& boneInfoMap = skeleton->GetMutableBoneInfoMap();
    int boneCount = skeleton->GetBoneCount();

    for (unsigned int i = 0; i < mesh->mNumBones; i++)
    {
        aiBone* bone = mesh->mBones[i];
        std::string boneName = bone->mName.C_Str();
        int boneIndex = 0;

        // まだマップに存在しないボーンであれば、新規に情報を追加
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

        // ボーンウェイトを対応する頂点に割り当て
        for (unsigned int j = 0; j < bone->mNumWeights; j++)
        {
            const aiVertexWeight& weight = bone->mWeights[j];
            unsigned int vertexId = weight.mVertexId;
            float vertexWeight = weight.mWeight;

            // 頂点のボーンデータに空きスロットを見つけて格納
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

// マテリアルデータとテクスチャを処理してロード
Material* ModelImporter::ProcessMaterial(aiMaterial* mat, const aiScene* scene, DescriptorAllocator* srvAllocator)
{
    Material* newMaterial = new Material();

    // ディフューズカラーのプロパティを読み込む
    aiColor4D color;
    if (aiGetMaterialColor(mat, AI_MATKEY_COLOR_DIFFUSE, &color) == AI_SUCCESS) {
        newMaterial->SetDiffuseColor({ color.r, color.g, color.b, color.a });
    }

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
            if (embeddedTexture->mHeight == 0) { // 圧縮形式
                diffuseTexture = textureImporter.Import(embeddedTexture->pcData, embeddedTexture->mWidth);
            }
            else { // 非圧縮形式
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

            if (std::filesystem::exists(texturePath)) {
                diffuseTexture = textureImporter.Import(texturePath.c_str());
            }
            else {
                OutputDebugStringW((L"ModelImporter Warning: External texture not found at " + texturePath.wstring() + L"\n").c_str());
            }
        }

        if (diffuseTexture)
        {
            newMaterial->SetTexture(Material::TextureSlot::Diffuse, diffuseTexture, srvAllocator);
        }
    }

    return newMaterial;
}

// インポートされたエンティティ階層をデバッグ出力する再帰関数
void ModelImporter::PrintHierarchy(Transform* transform, int level)
{
    for (int i = 0; i < level; i++)
    {
        OutputDebugStringA("    "); // 階層レベルに応じてインデント
    }

    OutputDebugStringA(transform->entity->name.c_str());
    OutputDebugStringA("\n");

    const int childCount = TransformSystem::GetChildCount(transform);
    for (int i = 0; i < childCount; i++)
    {
        PrintHierarchy(TransformSystem::GetChild(transform, i), level + 1);
    }
}