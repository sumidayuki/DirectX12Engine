#include "ModelImporter.h"

// Assimpのヘッダをインクルード
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

std::vector<ComPtr<Mesh>> ModelImporter::Import(const std::string& path)
{
    std::vector<ComPtr<Mesh>> meshes;

    Assimp::Importer importer;
    // postprocessフラグで、読み込み時に行う追加処理を指定できます。
    // aiProcess_Triangulate: 全ての面を三角形にする
    // aiProcess_FlipUVs: テクスチャのUV座標を上下反転させる（多くのモデルで必要）
    // aiProcess_CalcTangentSpace: 法線マップのための接空間を計算する
    const aiScene* scene = importer.ReadFile(path,
        aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        // エラーログ
        std::string errorMsg = "Assimp Error: ";
        errorMsg += importer.GetErrorString();
        OutputDebugStringA(errorMsg.c_str());
        return meshes; // 空のリストを返す
    }

    // ルートノードから再帰的に処理を開始
    ProcessNode(scene->mRootNode, scene, meshes);

    return meshes;
}

void ModelImporter::ProcessNode(aiNode* node, const aiScene* scene, std::vector<ComPtr<Mesh>>& meshes)
{
    // このノードが持つすべてのメッシュを処理
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(ProcessMesh(mesh, scene));
    }

    // このノードの子ノードを再帰的に処理
    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        ProcessNode(node->mChildren[i], scene, meshes);
    }
}

ComPtr<Mesh> ModelImporter::ProcessMesh(aiMesh* mesh, const aiScene* scene)
{
    std::vector<Mesh::Vertex> vertices;
    std::vector<uint32_t> indices;

    // 頂点データを抽出
    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        Mesh::Vertex vertex;

        // 座標
        vertex.position.x = mesh->mVertices[i].x;
        vertex.position.y = mesh->mVertices[i].y;
        vertex.position.z = mesh->mVertices[i].z;

        // 法線
        if (mesh->HasNormals())
        {
            vertex.normal.x = mesh->mNormals[i].x;
            vertex.normal.y = mesh->mNormals[i].y;
            vertex.normal.z = mesh->mNormals[i].z;
        }

        // UV座標 (テクスチャ座標)
        // 最初のセット(0番)のみ使用。モデルによっては複数のUVセットを持つことがある。
        if (mesh->HasTextureCoords(0))
        {
            vertex.uv.x = mesh->mTextureCoords[0][i].x;
            vertex.uv.y = mesh->mTextureCoords[0][i].y;
        }
        else
        {
            vertex.uv = Vector2(0.0f, 0.0f);
        }

        vertices.push_back(vertex);
    }

    // インデックスデータを抽出
    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++)
        {
            indices.push_back(face.mIndices[j]);
        }
    }

    // エンジンのMeshオブジェクトを作成してセットアップ
    ComPtr<Mesh> newMesh;
    newMesh.Attach(new Mesh());
    newMesh->SetupMesh(vertices, indices);

    return newMesh;
}