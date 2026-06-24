#include "AssetManager.h"

AssetManager::AssetManager()
{
}

AssetManager::~AssetManager()
{
    Clear();
}

void AssetManager::LoadAsset(AssetType type, const std::wstring& path)
{
    switch (type)
    {
        case AssetType::Texture:
        {
            if (m_textureCache.count(path))
            {
                return;
            }

            Texture2D* texture = m_texImp.Import(path.c_str());

            if (texture)
            {
                m_textureCache[path] = texture;
            }
            break;
        }
        case AssetType::Model:
        {
            if (m_modelCache.count(path))
            {
                return;
            }

            ModelData* modelData = m_modImp.Import(path);

            if (modelData)
            {
                m_modelCache[path] = modelData;
            }
            break;
        }
        case AssetType::Audio:
        {
            if (m_audioCache.count(path))
            {
                return;
            }

            AudioClip* clip = m_audImp.Import(path);
            if (clip)
            {
                m_audioCache[path] = clip;
            }
            break;
		}
    }
}

Entity AssetManager::Instantiate(const std::wstring& path, Transform* parent, const Vector3& localPosition, const Quaternion& localRotation, LayerMask layer)
{
    World& world = SceneManager::GetCurrentScene()->GetWorld();

    ModelData* modelData = GetAsset<ModelData>(AssetType::Model, path);
    if (!modelData)
    {
        return INVALID_ENTITY;
    }

    Entity rootEntity = CreateEntityFromPrefab(*modelData->rootNode, modelData, INVALID_ENTITY, layer);

    if (rootEntity != INVALID_ENTITY)
    {
        Transform* rootTransform = world.GetComponent<Transform>(rootEntity);
        TransformSystem* system = world.GetTransformSystem();

        if (parent)
        {
            system->SetParent(*rootTransform, parent);
        }
        system->SetLocalPosition(*rootTransform, localPosition);
        system->SetLocalRotation(*rootTransform, localRotation);

        if (modelData->skeleton && !modelData->animations.empty())
        {
            Animator animator;
            animator.skeleton = modelData->skeleton;
            animator.clips = modelData->animations;
            animator.currentClip = modelData->animations.begin()->second;
            animator.currentClipName = modelData->animations.begin()->first;
            world.AddComponent<Animator>(rootEntity, animator);
        }

        // レイヤー設定
        Layer layerComp;
        layerComp.layer = layer;
        world.AddComponent<Layer>(rootEntity, layerComp);
    }

    return rootEntity;
}

Entity AssetManager::CreateEntityFromPrefab(const PrefabNode& node, const ModelData* modelData, Entity parentEntity, LayerMask layer)
{
    World& world = SceneManager::GetCurrentScene()->GetWorld();

    Entity entity = world.CreateEntity(node.name);
    Transform* transform = world.GetComponent<Transform>(entity);

    // レイヤー設定
    Layer layerComp;
    layerComp.layer = layer;
    world.AddComponent<Layer>(entity, layerComp);

    if (parentEntity != INVALID_ENTITY)
    {
        TransformSystem::GetInstance()->SetParent(*transform, world.GetComponent<Transform>(parentEntity));
    }

    node.transform.Decompose(transform->scale, transform->rotation, transform->position);

    if (!node.meshIndices.empty())
    {
        int meshIndex = node.meshIndices[0];
        Mesh* mesh = modelData->meshes[meshIndex];

        Material* material = new Material();
        if (!modelData->materials.empty())
        {
            if (meshIndex < modelData->materials.size())
            {
                material = modelData->materials[meshIndex];
            }
            else
            {
                material = modelData->materials[0];
            }
        }

        if (modelData->skeleton)
        {
            MeshFilter filter;
            filter.mesh = mesh;
            world.AddComponent<MeshFilter>(entity, filter);

            SkinnedMeshRenderer renderer;
            if (material) renderer.materials.push_back(material);
            world.AddComponent<SkinnedMeshRenderer>(entity, renderer);
        }
        else
        {
            MeshFilter filter;
            filter.mesh = mesh;
            world.AddComponent<MeshFilter>(entity, filter);

            MeshRenderer renderer;
            if (material) renderer.materials.push_back(material);
            world.AddComponent<MeshRenderer>(entity, renderer);
        }
    }

    for (const auto& childNode : node.children)
    {
        CreateEntityFromPrefab(*childNode, modelData, entity, layer);
    }

    return entity;
}

// インポートされたエンティティ階層をデバッグ出力する再帰関数
void AssetManager::PrintHierarchy(Transform* transform, int level)
{
    for (int i = 0; i < level; i++)
    {
        OutputDebugStringA("    "); // 階層レベルに応じてインデント
    }

    OutputDebugStringA(transform->entity.name.c_str());
    OutputDebugStringA("\n");

    const int childCount = TransformSystem::GetInstance()->GetChildCount(transform);
    for (int i = 0; i < childCount; i++)
    {
        PrintHierarchy(TransformSystem::GetInstance()->GetChild(transform, i), level + 1);
    }
}

void AssetManager::Clear()
{
    for (auto& pair : m_modelCache)
    {
        ModelData* data = pair.second;
        for (Mesh* mesh : data->meshes) delete mesh;
        for (Material* mat : data->materials) delete mat;
        for (auto& anim_pair : data->animations) delete anim_pair.second;
        delete data->skeleton;
        delete data;
    }
    m_modelCache.clear();

    m_textureCache.clear();
	m_audioCache.clear();
}
