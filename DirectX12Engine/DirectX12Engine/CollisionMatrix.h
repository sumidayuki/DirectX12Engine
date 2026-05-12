#pragma once

class CollisionMatrix
{
private:
    uint32_t m_matrix[32];

    CollisionMatrix()
    {
        for (int i = 0; i < 32; ++i)
        {
            m_matrix[i] = 0xFFFFFFFF;
        }
    }

public:
    static CollisionMatrix& GetInstance()
    {
        static CollisionMatrix instance;
        return instance;
    }

    void SetLayerCollision(int layerA, int layerB, bool canCollide)
    {
        if (layerA < 0 || layerA >= 32 || layerB < 0 || layerB >= 32) return;

        if (canCollide)
        {
            m_matrix[layerA] |= (1u << layerB);
            m_matrix[layerB] |= (1u << layerA);
        }
        else
        {
            m_matrix[layerA] &= ~(1u << layerB);
            m_matrix[layerB] &= ~(1u << layerA);
        }
    }

    bool CanCollide(LayerMask maskA, LayerMask maskB) const
    {
        for (int i = 0; i < 32; ++i)
        {
            if (maskA & (1u << i))
            {
                if (m_matrix[i] & maskB)
                {
                    return true;
                }
            }
        }
        return false;
    }

    void IgnoreLayerCollision(int layerA, int layerB)
    {
        SetLayerCollision(layerA, layerB, false);
    }

    uint32_t GetCollisionMask(int layerIndex) const
    {
        if (layerIndex < 0 || layerIndex >= 32) return 0;
        return m_matrix[layerIndex];
    }
};
