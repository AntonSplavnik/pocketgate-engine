#ifndef IENTITY_HPP
#define IENTITY_HPP

#include <cstddef>
#include <vector>
#include <iostream>
#include <optional>
#include <typeindex>
#include <typeinfo>
#include <utility>
#include <span>

using EntityID = size_t;

class IComponent {
public:
    IComponent() = delete;
    IComponent(EntityID ownerId) : ownerId(ownerId) {}
    virtual ~IComponent() = default;
    
    EntityID getOwnerId() const noexcept { return ownerId; }
private:
    EntityID ownerId;
};

/*
    COMPONENTS
*/
class IComponentStorage {
public:
    virtual ~IComponentStorage() {}
};

template<typename T>
class ComponentStorage final : public IComponentStorage {
public:
    template<typename... Args>
    void add(EntityID id, Args&&... args);

    void remove(EntityID id);

    T* get(EntityID id);

    std::span<T> getAll();
private:
    std::unordered_map<EntityID, size_t> entityToIndex;
    std::vector<T> data;
};

/*
    ENTITY
*/
class IEntity {
public:
    virtual ~IEntity() {}

    EntityID id() const noexcept {
        return _id;
    }
protected: // allow IEntity instantiation only from derived class
    IEntity() : _id(++idCounter) {}
private:
    const EntityID _id;
    inline static EntityID idCounter = 0;
};

/*
    REGISTRY
*/
class Registry final {
public:
    template<typename T, typename... Args>
    requires std::derived_from<T, IComponent>
    void emplace(EntityID id, Args&&... args);

    template<typename T>
    requires std::derived_from<T, IComponent>
    void removeEntityComponent(EntityID id);

    template<typename T>
    requires std::derived_from<T, IComponent>
    std::optional<std::span<T>> getComponents() const;

    template<typename T>
    requires std::derived_from<T, IComponent>
    T* getComponent(EntityID id);

    template<typename T, typename... Args>
    requires std::derived_from<T, IEntity>
    std::weak_ptr<T> createEntity(Args&&... args);

    void destroyEntity(EntityID id);

private:
    std::unordered_map<std::type_index, std::unique_ptr<IComponentStorage>> componentTypeToStorage;
    std::unordered_map<EntityID, std::shared_ptr<IEntity>> entityIdToToEntity;
};

/*
    COMPONENTS
*/
template<typename T>
template<typename... Args>
void ComponentStorage<T>::add(EntityID id, Args&&... args) {
    if (const auto it = entityToIndex.find(id); it == entityToIndex.end()) {
        data.emplace_back(T(id, std::forward<Args>(args)...));
        entityToIndex.emplace(id, data.size() - 1);
    }
}

template<typename T>
void ComponentStorage<T>::remove(EntityID id) {
    if (const auto it = entityToIndex.find(id); it == entityToIndex.end()) {
        const auto &lastComponent = data.back();
        entityToIndex[lastComponent.getOwnerId()] = it->second;
        entityToIndex.erase(id);
        data[it->second] = std::move(lastComponent);
        data.pop_back();
    }
}

template<typename T>
T* ComponentStorage<T>::get(EntityID id) {
    if (const auto it = entityToIndex.find(id); it != entityToIndex.end())
        return &data[it->second];

    return nullptr;
}

template<typename T>
std::span<T> ComponentStorage<T>::getAll() {
    return data;
}

/*
    REGISTRY
*/
template<typename T, typename... Args>
requires std::derived_from<T, IEntity>
std::weak_ptr<T> Registry::createEntity(Args&&... args) {
    auto entity = std::make_shared<T>(std::forward<Args>(args)...);
    entityIdToToEntity.emplace(entity.get()->id(), entity);

    return entity;
}

void Registry::destroyEntity(EntityID id) {
    if (auto it = entityIdToToEntity.find(id); it != entityIdToToEntity.end()) {
        entityIdToToEntity.erase(it);
        // TODO: delete all components
    }
}

template<typename T>
requires std::derived_from<T, IComponent>
void Registry::removeEntityComponent(EntityID id) {
    const auto key = std::type_index(typeid(T));
    if (const auto it = componentTypeToStorage.find(key); it != componentTypeToStorage.end()) {
        ComponentStorage<T> *store = static_cast<ComponentStorage<T>*>(it->second.get());
        store->remove(id);
    }
}

template<typename T, typename... Args>
requires std::derived_from<T, IComponent>
void Registry::emplace(EntityID id, Args&&... args) {
    const auto key = std::type_index(typeid(T));

    auto it = componentTypeToStorage.find(key);
    if (it == componentTypeToStorage.end()) {
        auto [newIt, _] = componentTypeToStorage.emplace(key, std::make_unique<ComponentStorage<T>>());
        it = newIt;
    }
    
    auto storage = static_cast<ComponentStorage<T>*>(it->second.get());
    storage->add(id, std::forward<Args>(args)...);
}

template<typename T>
requires std::derived_from<T, IComponent>
std::optional<std::span<T>> Registry::getComponents() const {
    const auto key = std::type_index(typeid(T));
    if (const auto it = componentTypeToStorage.find(key); it != componentTypeToStorage.end()) {
        ComponentStorage<T> *store = static_cast<ComponentStorage<T>*>(it->second.get());
        return store->getAll();
    }

    return std::nullopt;
}

template<typename T>
requires std::derived_from<T, IComponent>
T* Registry::getComponent(EntityID id) {
    const auto key = std::type_index(typeid(T));
    if (const auto it = componentTypeToStorage.find(key); it != componentTypeToStorage.end()) {
        ComponentStorage<T> *store = static_cast<ComponentStorage<T>*>(it->second.get());
        return store->get(id);
    }

    return nullptr;
}

#endif