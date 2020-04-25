#pragma once

#include <bitset>
#include <queue>
#include "Hazel/Core/Core.h"
#include <array>
#include <unordered_map>
#include <set>
#include "Systems.h"

#include "Hazel/Core/Timestep.h"
#include <set>
#include <glm/glm.hpp>


#include "Components.h"
#include "ECSTypeDefs.h"
#include "Hazel/Renderer/Texture.h"

namespace Hazel
{
    class EntityManager
    {
    public:
        EntityManager()
        {
            // Initialize the queue with all possible entity IDs
            for (Entity entity = 0; entity < MAX_ENTITIES; ++entity)
            {
                m_AvailableEntities.push(entity);
            }
        }

        Entity CreateEntity()
        {
            HZ_CORE_ASSERT(m_LivingEntityCount < MAX_ENTITIES, "Too many entities in existence.");

            // Take an ID from the front of the queue
            Entity id = m_AvailableEntities.front();
            m_AvailableEntities.pop();
            ++m_LivingEntityCount;

            return id;
        }

        void DestroyEntity(Entity entity)
        {
            HZ_CORE_ASSERT(entity < MAX_ENTITIES, "Entity out of range.");

            // Invalidate the destroyed entity's signature
            m_Signatures[entity].reset();

            // Put the destroyed ID at the back of the queue
            m_AvailableEntities.push(entity);
            --m_LivingEntityCount;
        }

        void SetSignature(Entity entity, Signature signature)
        {
            HZ_CORE_ASSERT(entity < MAX_ENTITIES, "Entity out of range.");

            // Put this entity's signature into the array
            m_Signatures[entity] = signature;
        }

        Signature GetSignature(Entity entity)
        {
            HZ_CORE_ASSERT(entity < MAX_ENTITIES, "Entity out of range.");

            // Get this entity's signature from the array
            return m_Signatures[entity];
        }

    private:
        // Queue of unused entity IDs
        std::queue<Entity> m_AvailableEntities;

        // Array of signatures where the index corresponds to the entity ID
        std::array<Signature, MAX_ENTITIES> m_Signatures;

        // Total living entities - used to keep limits on how many exist
        unsigned m_LivingEntityCount = 0;
    };

    class IComponentArray
    {
    public:
        virtual ~IComponentArray() = default;
        virtual void EntityDestroyed(Entity entity) = 0;

        virtual Ref<IComponentArray> Clone() const = 0;
    };


    template <typename T>
    class ComponentArray : public IComponentArray
    {
    public:

        Ref<IComponentArray> Clone() const override
        {
            return std::static_pointer_cast<IComponentArray>(std::make_shared<ComponentArray<T>>(*this));
        }

        void InsertData(Entity entity, T component)
        {
            HZ_CORE_ASSERT(
                m_EntityToIndexMap.find(entity) == m_EntityToIndexMap.end(),
                "Component added to same entity more than once.");

            // Put new entry at end and update the maps
            unsigned long long newIndex = m_Size;
            m_EntityToIndexMap[entity] = newIndex;
            m_IndexToEntityMap[newIndex] = entity;
            m_ComponentArray[newIndex] = component;
            ++m_Size;
        }

        void RemoveData(Entity entity)
        {
            HZ_CORE_ASSERT(m_EntityToIndexMap.find(entity) != m_EntityToIndexMap.end(),
                           "Removing non-existent component.");

            // Copy element at end into deleted element's place to maintain density
            unsigned long long indexOfRemovedEntity = m_EntityToIndexMap[entity];
            unsigned long long indexOfLastElement = m_Size - 1;
            m_ComponentArray[indexOfRemovedEntity] = m_ComponentArray[indexOfLastElement];

            // Update map to point to moved spot
            Entity entityOfLastElement = m_IndexToEntityMap[indexOfLastElement];
            m_EntityToIndexMap[entityOfLastElement] = indexOfRemovedEntity;
            m_IndexToEntityMap[indexOfRemovedEntity] = entityOfLastElement;

            m_EntityToIndexMap.erase(entity);
            m_IndexToEntityMap.erase(indexOfLastElement);

            --m_Size;
        }

        T& GetData(Entity entity)
        {
            HZ_CORE_ASSERT(m_EntityToIndexMap.find(entity) != m_EntityToIndexMap.end(),
                           "Retrieving non-existent component.");

            // Return a reference to the entity's component
            return m_ComponentArray[m_EntityToIndexMap[entity]];
        }

        void EntityDestroyed(Entity entity) override
        {
            if (m_EntityToIndexMap.find(entity) != m_EntityToIndexMap.end())
            {
                // Remove the entity's component if it existed
                RemoveData(entity);
            }
        }

    private:
        // The packed array of components (of generic type T),
        // set to a specified maximum amount, matching the maximum number
        // of entities allowed to exist simultaneously, so that each entity
        // has a unique spot.
        std::array<T, MAX_ENTITIES> m_ComponentArray;

        // Map from an entity ID to an array index.
        std::unordered_map<Entity, unsigned long long> m_EntityToIndexMap;

        // Map from an array index to an entity ID.
        std::unordered_map<unsigned long long, Entity> m_IndexToEntityMap;

        // Total size of valid entries in the array.
        unsigned long long m_Size{};
    };

    class ComponentManager
    {
    public:
        ComponentManager()
        {
        }

        ComponentManager(const ComponentManager& other) : m_ComponentTypes(other.m_ComponentTypes),
                                                          m_ComponentArrays(other.m_ComponentArrays),
                                                          m_NextComponentType(other.m_NextComponentType)
        {
            for (auto& [typeName, compArr] : m_ComponentArrays)
            {
                compArr = compArr->Clone();
            }
        }

        ComponentManager(ComponentManager&& other) noexcept : m_ComponentTypes(std::move(other.m_ComponentTypes)),
                                                              m_ComponentArrays(std::move(other.m_ComponentArrays)),
                                                              m_NextComponentType(other.m_NextComponentType)
        {
        }

        ComponentManager& operator=(const ComponentManager& other)
        {
            if (this == &other)
                return *this;
            m_ComponentTypes = other.m_ComponentTypes;
            m_ComponentArrays = other.m_ComponentArrays;
            m_NextComponentType = other.m_NextComponentType;
            for (auto& [typeName, compArr] : m_ComponentArrays)
            {
                compArr = compArr->Clone();
            }
            return *this;
        }

        ComponentManager& operator=(ComponentManager&& other) noexcept
        {
            if (this == &other)
                return *this;
            m_ComponentTypes = std::move(other.m_ComponentTypes);
            m_ComponentArrays = std::move(other.m_ComponentArrays);
            m_NextComponentType = other.m_NextComponentType;
            return *this;
        }

        template <typename T>
        void RegisterComponent()
        {
            const char* typeName = typeid(T).name();

            HZ_CORE_ASSERT(m_ComponentTypes.find(typeName) == m_ComponentTypes.end(),
                           "Registering component type more than once.");

            // Add this component type to the component type map
            m_ComponentTypes.insert({typeName, m_NextComponentType});

            // Create a ComponentArray pointer and add it to the component arrays map
            m_ComponentArrays.insert({typeName, CreateRef<ComponentArray<T>>()});

            // Increment the value so that the next component registered will be different
            ++m_NextComponentType;
        }

        template <typename T>
        ComponentType GetComponentType()
        {
            const char* typeName = typeid(T).name();

            HZ_CORE_ASSERT(m_ComponentTypes.find(typeName) != m_ComponentTypes.end(),
                           "Component not registered before use.");

            // Return this component's type - used for creating signatures
            return m_ComponentTypes[typeName];
        }

        template <typename T>
        void AddComponent(Entity entity, T component)
        {
            // Add a component to the array for an entity
            GetComponentArray<T>()->InsertData(entity, component);
        }

        template <typename T>
        void RemoveComponent(Entity entity)
        {
            // Remove a component from the array for an entity
            GetComponentArray<T>()->RemoveData(entity);
        }

        template <typename T>
        T& GetComponent(Entity entity)
        {
            // Get a reference to a component from the array for an entity
            return GetComponentArray<T>()->GetData(entity);
        }

        void EntityDestroyed(Entity entity)
        {
            // Notify each component array that an entity has been destroyed
            // If it has a component for that entity, it will remove it
            for (auto const& pair : m_ComponentArrays)
            {
                auto const& component = pair.second;

                component->EntityDestroyed(entity);
            }
        }

    private:
        // Map from type string pointer to a component type
        std::unordered_map<const char*, ComponentType> m_ComponentTypes;

        // Map from type string pointer to a component array
        std::unordered_map<const char*, Ref<IComponentArray>> m_ComponentArrays;

        // The component type to be assigned to the next registered component - starting at 0
        ComponentType m_NextComponentType{};

        // Convenience function to get the statically casted pointer to the ComponentArray of type T.
        template <typename T>
        Ref<ComponentArray<T>> GetComponentArray()
        {
            const char* typeName = typeid(T).name();

            HZ_CORE_ASSERT(m_ComponentTypes.find(typeName) != m_ComponentTypes.end(),
                           "Component not registered before use.");

            return std::static_pointer_cast<ComponentArray<T>>(m_ComponentArrays[typeName]);
        }
    };

    class SystemManager
    {
    public:
        SystemManager()
        {
        }

        SystemManager(const SystemManager& other) : m_Signatures(other.m_Signatures),
                                                    m_Systems(other.m_Systems)
        {
            // deep copy
            for (auto& [typeName, systemRef] : m_Systems)
            {
                systemRef = systemRef->Clone();
            }
        }

        SystemManager(SystemManager&& other) noexcept : m_Signatures(std::move(other.m_Signatures)),
                                                        m_Systems(std::move(other.m_Systems))
        {
        }

        SystemManager& operator=(const SystemManager& other)
        {
            if (this == &other)
                return *this;
            m_Signatures = other.m_Signatures;
            m_Systems = other.m_Systems;
            // deep copy
            for (auto& [typeName, systemRef] : m_Systems)
            {
                systemRef = systemRef->Clone();
            }
            return *this;
        }

        SystemManager& operator=(SystemManager&& other) noexcept
        {
            if (this == &other)
                return *this;
            m_Signatures = std::move(other.m_Signatures);
            m_Systems = std::move(other.m_Systems);
            return *this;
        }

        template <typename T>
        Ref<T> RegisterSystem(ECS* ecs)
        {
            const char* typeName = typeid(T).name();

            HZ_CORE_ASSERT(m_Systems.find(typeName) == m_Systems.end(), "Registering system more than once.");

            // Create a pointer to the system and return it so it can be used externally
            auto system = CreateRef<T>(ecs);
            m_Systems.insert({typeName, system});
            return system;
        }

        template<typename T>
        void DeregisterSystem() {
            const char* typeName = typeid(T).name();

            HZ_CORE_ASSERT(m_Systems.find(typeName) != m_Systems.end(), "System does not exist.");
            m_Systems.erase(typeName);
        }

        template <typename T>
        void SetSignature(Signature signature)
        {
            const char* typeName = typeid(T).name();

            HZ_CORE_ASSERT(m_Systems.find(typeName) != m_Systems.end(), "System used before registered.");

            // Set the signature for this system
            m_Signatures.insert({typeName, signature});
        }

        void EntityDestroyed(Entity entity)
        {
            // Erase a destroyed entity from all system lists
            // mEntities is a set so no check needed
            for (auto const& pair : m_Systems)
            {
                auto const& system = pair.second;

                system->m_Entities.erase(entity);
            }
        }

        void EntitySignatureChanged(Entity entity, Signature entitySignature)
        {
            // Notify each system that an entity's signature changed
            for (auto const& pair : m_Systems)
            {
                auto const& type = pair.first;
                auto const& system = pair.second;
                auto const& systemSignature = m_Signatures[type];

                // Entity signature matches system signature - insert into set
                if ((entitySignature & systemSignature) == systemSignature)
                {
                    system->m_Entities.insert(entity);
                    // notify the system about the change
                    system->OnEntityAdded(entity);
                }
                    // Entity signature does not match system signature - erase from set
                else
                {
                    system->m_Entities.erase(entity);
                    // notify the system about the change
                    system->OnEntityRemoved(entity);
                }
            }
        }

        template <typename T>
        Ref<T> GetSystem()
        {
            const char* typeName = typeid(T).name();
            return std::static_pointer_cast<T>(m_Systems[typeName]);
        }

        void OnUpdate(Timestep ts)
        {
            for (auto [id, system] : m_Systems)
            {
                system->OnUpdate(ts);
            }
        }

    private:
        // Map from system type string pointer to a signature
        std::unordered_map<const char*, Signature> m_Signatures;

        // Map from system type string pointer to a system pointer
        std::unordered_map<const char*, Ref<System>> m_Systems;
    };

    class ECS
    {
    public:
        ECS()
        {
        }

        ECS CreateSnapshot()
        {
            return ECS(*this);
        }

        Entity CreateEntity()
        {
            return m_EntityManager.CreateEntity();
        }

        // ---------------------------------------------------------------------------------------------
        // ---------------------------------------------------------------------------------------------

        // creating quads
        // normal quads
        Entity CreateQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color)
        {
            Entity e = CreateEntity();
            AddComponent<Transform>(e, {{position.x, position.y, 0.0f}, size, 0.0f});
            AddComponent<Colored>(e, {color});
            AddComponent<Drawable>(e, {PrimitiveGeometryType::Quad});
            return e;
        }

        Entity CreateQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color)
        {
            Entity e = CreateEntity();
            AddComponent<Transform>(e, {position, size, 0.0f});
            AddComponent<Colored>(e, {color});
            AddComponent<Drawable>(e, {PrimitiveGeometryType::Quad});
            return e;
        }

        Entity CreateQuad(const glm::vec2& position, const glm::vec2& size, const Ref<Texture2D>& texture,
                          float tilingFactor = 1.0f, const glm::vec4& tintColor = glm::vec4(1.0f))
        {
            Entity e = CreateEntity();
            AddComponent<Transform>(e, {{position.x, position.y, 0.0f}, size, 0.0f});
            AddComponent<Colored>(e, {tintColor});
            AddComponent<Drawable>(e, {PrimitiveGeometryType::Quad});
            AddComponent<Textured>(e, {texture, tilingFactor});
            return e;
        }

        Entity CreateQuad(const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D>& texture,
                          float tilingFactor = 1.0f, const glm::vec4& tintColor = glm::vec4(1.0f))
        {
            Entity e = CreateEntity();
            AddComponent<Transform>(e, {position, size, 0.0f});
            AddComponent<Colored>(e, {tintColor});
            AddComponent<Drawable>(e, {PrimitiveGeometryType::Quad});
            AddComponent<Textured>(e, {texture, tilingFactor});
            return e;
        }

        // rotated quads
        Entity CreateRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation,
                                 const glm::vec4& color)
        {
            Entity e = CreateEntity();
            AddComponent<Transform>(e, {{position.x, position.y, 0.0f}, size, rotation});
            AddComponent<Colored>(e, {color});
            AddComponent<Drawable>(e, {PrimitiveGeometryType::Quad});
            return e;
        }

        Entity CreateRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation,
                                 const glm::vec4& color)
        {
            Entity e = CreateEntity();
            AddComponent<Transform>(e, {position, size, rotation});
            AddComponent<Colored>(e, {color});
            AddComponent<Drawable>(e, {PrimitiveGeometryType::Quad});
            return e;
        }

        Entity CreateRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation,
                                 const Ref<Texture2D>& texture, float tilingFactor = 1.0f,
                                 const glm::vec4& tintColor = glm::vec4(1.0f))
        {
            Entity e = CreateEntity();
            AddComponent<Transform>(e, {{position.x, position.y, 0.0f}, size, rotation});
            AddComponent<Colored>(e, {tintColor});
            AddComponent<Drawable>(e, {PrimitiveGeometryType::Quad});
            AddComponent<Textured>(e, {texture, tilingFactor});
            return e;
        }

        Entity CreateRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation,
                                 const Ref<Texture2D>& texture, float tilingFactor = 1.0f,
                                 const glm::vec4& tintColor = glm::vec4(1.0f))
        {
            Entity e = CreateEntity();
            AddComponent<Transform>(e, {position, size, rotation});
            AddComponent<Colored>(e, {tintColor});
            AddComponent<Drawable>(e, {PrimitiveGeometryType::Quad});
            AddComponent<Textured>(e, {texture, tilingFactor});
            return e;
        }

        // ---------------------------------------------------------------------------------------------
        // ---------------------------------------------------------------------------------------------


        void DestroyEntity(Entity entity)
        {
            m_EntityManager.DestroyEntity(entity);

            m_ComponentManager.EntityDestroyed(entity);

            m_SystemManager.EntityDestroyed(entity);
        }


        // Component methods
        template <typename T>
        void RegisterComponent()
        {
            m_ComponentManager.RegisterComponent<T>();
        }

        template <typename T>
        void AddComponent(Entity entity, T component)
        {
            m_ComponentManager.AddComponent<T>(entity, component);

            auto signature = m_EntityManager.GetSignature(entity);
            signature.set(m_ComponentManager.GetComponentType<T>(), true);
            m_EntityManager.SetSignature(entity, signature);

            m_SystemManager.EntitySignatureChanged(entity, signature);
        }

        template <typename T>
        void RemoveComponent(Entity entity)
        {
            m_ComponentManager.RemoveComponent<T>(entity);

            auto signature = m_EntityManager.GetSignature(entity);
            signature.set(m_ComponentManager.GetComponentType<T>(), false);
            m_EntityManager.SetSignature(entity, signature);

            m_SystemManager.EntitySignatureChanged(entity, signature);
        }

        template <typename T>
        T& GetComponent(Entity entity)
        {
            return m_ComponentManager.GetComponent<T>(entity);
        }

        template <typename T>
        bool HasComponent(Entity entity)
        {
            const auto entitySignature = m_EntityManager.GetSignature(entity);
            const auto componentType = m_ComponentManager.GetComponentType<T>();
            return entitySignature.test(componentType);
        }

        template <typename T>
        ComponentType GetComponentType()
        {
            return m_ComponentManager.GetComponentType<T>();
        }

        // System methods
        template <typename T>
        Ref<T> RegisterSystem()
        {
            return m_SystemManager.RegisterSystem<T>(this);
        }

        template <typename T>
        void DeregisterSystem() {
            m_SystemManager.DeregisterSystem<T>();
        }

        template <typename T>
        void SetSystemSignature(Signature signature)
        {
            m_SystemManager.SetSignature<T>(signature);
        }

        template <typename T>
        Ref<T> GetSystem()
        {
            return m_SystemManager.GetSystem<T>();
        }

        void OnUpdate(Timestep ts)
        {
            m_SystemManager.OnUpdate(ts);
        }

    private:
        ComponentManager m_ComponentManager;
        EntityManager m_EntityManager;
        SystemManager m_SystemManager;
    };
}
