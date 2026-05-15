
#include <stdlib.h>
#include <random>
#include <math.h>
#include <fstream>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <thread>
#include <chrono>

#include "entity.hpp"

struct Position : public IComponent
{
    Position(EntityID ownerId, int x, int y) : IComponent(ownerId), x(x), y(y)
    {
        // std::cout << "Position created" << std::endl;
    }

    int x = 0;
    int y = 0;
};

class Player : public IEntity
{
public:
    Player() : IEntity() {}
    Player(std::string_view name, int xp) : IEntity(), name(name), xp(xp) {}

private:
    const std::string name;
    int xp;
};

#define COUNT 100000

int main()
{
    // 1. create registry
    auto registry = std::make_unique<Registry>();

    // 2. add entities
    std::vector<std::weak_ptr<Player>> players;
    for (int i = 0; i < COUNT; ++i)
        players.push_back(registry->createEntity<Player>("Bob", i));

    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

    // 3. add components
    for (const auto &player : players)
    {
        if (auto p = player.lock())
            registry->emplace<Position>(p->id(), 2, 4);
    }

    int count = 0;
    if (const auto &cms = registry->getComponents<Position>())
        std::for_each(cms->begin(), cms->end(), [&count](const auto &c)
                      { ++count; });

    std::cout << "count: " << count << std::endl;

     // 4. query every entity component 1 by 1
    int count2 = 0;
    for (const auto &player : players) {
        if (auto p = player.lock()) {
            const Position *pos = registry->getComponent<Position>(p->id());
            if (pos) {
                ++count2;
            }
        }
    }

    std::cout << "count2: " << count2 << std::endl;

    // 5. delete components
    for (int i = 0; i < COUNT; ++i)
    {
        if (auto p = players[i].lock())
            registry->removeEntityComponent<Position>(p->id());
    }

    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() << "[µs]" << std::endl;
    std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin).count() << "[ns]" << std::endl;

    // 6. delete entities
    for (int i = 0; i < COUNT; ++i)
    {
        if (auto p = players[i].lock())
            registry->destroyEntity(p->id());
    }
}