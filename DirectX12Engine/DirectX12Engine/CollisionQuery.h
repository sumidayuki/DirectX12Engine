#pragma once

namespace CollisionQuery
{
    template<typename T, typename Fn>
    void ForEachEnter(const CollisionEvents& events, World& world, Fn fn)
    {
        for (int i = 0; i < events.enterCount; ++i)
        {
            T* comp = world.GetComponent<T>(events.enterContacts[i].other);
            if (comp)
            {
                fn(events.enterContacts[i].other, *comp, events.enterContacts[i]);
            }
        }
    }


    template<typename T, typename Fn>
    void ForEachStay(const CollisionEvents& events, World& world, Fn fn)
    {
        for (int i = 0; i < events.stayCount; ++i)
        {
            T* comp = world.GetComponent<T>(events.stayContacts[i].other);
            if (comp)
            {
                fn(events.stayContacts[i].other, *comp, events.stayContacts[i]);
            }
        }
    }

    template<typename T, typename Fn>
    void ForEachExit(const CollisionEvents& events, World& world, Fn fn)
    {
        for (int i = 0; i < events.exitCount; ++i)
        {
            T* comp = world.GetComponent<T>(events.exitContacts[i].other);
            if (comp)
            {
                fn(events.exitContacts[i].other, *comp, events.exitContacts[i]);
            }
        }
    }

    template<typename T, typename Fn>
    void ForEachHit(const CollisionEvents& events, World& world, Fn fn)
    {
        ForEachEnter<T>(events, world, fn);
        ForEachStay<T>(events, world, fn);
    }

    template<typename T>
    Entity FindEnter(const CollisionEvents& events, World& world)
    {
        for (int i = 0; i < events.enterCount; ++i)
        {
            if (world.GetComponent<T>(events.enterContacts[i].other))
            {
                return events.enterContacts[i].other;
            }
        }
        return INVALID_ENTITY;
    }

    template<typename T>
    Entity FindHit(const CollisionEvents& events, World& world)
    {
        for (int i = 0; i < events.enterCount; ++i)
        {
            if (world.GetComponent<T>(events.enterContacts[i].other))
                return events.enterContacts[i].other;
        }
        for (int i = 0; i < events.stayCount; ++i)
        {
            if (world.GetComponent<T>(events.stayContacts[i].other))
                return events.stayContacts[i].other;
        }
        return INVALID_ENTITY;
    }

    template<typename T>
    bool HasEnter(const CollisionEvents& events, World& world)
    {
        return FindEnter<T>(events, world) != INVALID_ENTITY;
    }


    template<typename T>
    bool HasHit(const CollisionEvents& events, World& world)
    {
        return FindHit<T>(events, world) != INVALID_ENTITY;
    }
}
