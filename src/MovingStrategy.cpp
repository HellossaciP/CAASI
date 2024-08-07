#include "MovingStrategy.h"

using namespace std;

Context::Context(std::unique_ptr<MovingStrategy>&& strategy) 
    : strategy{std::move(strategy)}
{}

void Context::set_strategy(std::unique_ptr<MovingStrategy>&& strategy)
{
    this->strategy = std::move(strategy);
}

sf::Vector2f Context::updatePosition(sf::Vector2f position, sf::Vector2f pposition, float speed) const
{
    return strategy->nextPosition(position, pposition, speed);
}

sf::Vector2f FatStrategy::nextPosition(sf::Vector2f position, sf::Vector2f pposition, float speed) const
{
    return sf::Vector2f();
}

sf::Vector2f AngryStrategy::nextPosition(sf::Vector2f position, sf::Vector2f pposition, float speed) const
{
    sf::Vector2f direction = pposition - position;
    float length = sqrt(direction.x * direction.x + direction.y * direction.y);

    if (length != 0) direction = sf::Vector2f(direction.x / length, direction.y / length);

    return direction * speed;
}

sf::Vector2f DumbStrategy::nextPosition(sf::Vector2f position, sf::Vector2f pposition, float speed) const
{
    sf::Vector2f direction;
    if (position.y <= 300 && position.x >= 200)
        direction = { -1,0 };
    if (position.y >= 700 && position.x <= 1600)
        direction = { 1,0 };
    if (position.x <= 200 && position.y <= 700)
        direction = { 0,1 };
    if (position.x >= 1600 && position.y >= 300)
        direction = { 0,-1 };

    return direction * speed;
}

sf::Vector2f CrazyStrategy::nextPosition(sf::Vector2f position, sf::Vector2f pposition, float speed) const
{
    float speedX = (rand() % 200 - 80) / 20;
    float speedY = (rand() % 200 - 80) / 20;

    sf::Vector2f direction = pposition - position;
    float length = sqrt(direction.x * direction.x + direction.y * direction.y);

    if (length != 0) direction = sf::Vector2f(direction.x / length, direction.y / length);
    auto speedRandom = sf::Vector2f(speedX, speedY);

    return sf::Vector2f(direction.x * speedRandom.x * speed, direction.y * speedRandom.y * speed);
}
