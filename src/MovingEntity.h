#pragma once
#include "Entity.h"

class MovingEntity : public Entity {
public:
	MovingEntity(pugi::xml_node node);

	void move();
	void update() override;
	virtual std::string dump(std::string const& indent) const;
protected:
	float speed;
	bool isMovingUp;
	bool isMovingDown;
	bool isMovingLeft;
	bool isMovingRight;
	sf::Vector2f direction;
};