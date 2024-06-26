#include <cassert>
#include <iostream>
#include <sstream>

#include "Room.h"
#include "Wall.h"
#include "Player.h"
#include "Monster.h"
#include "Door.h"

using namespace std;

Room::Room(pugi::xml_node node)
    : label { node.attribute("label").as_string() }
{
    for (auto child : node.children())
    {
        assert((child.name() == "Wall"sv) || (child.name() == "Monster"sv) || (child.name() == "Player"sv) || (child.name() == "Door"sv));
        if (child.name() == "Wall"sv) {
            entities.push_back(make_unique<Wall>(child));
        }
        if (child.name() == "Monster"sv) {
            entities.push_back(make_unique<Monster>(child));
            monster++;
        }
        if (child.name() == "Player"sv) {
            entities.push_back(make_unique<Player>(child));
        }
        if (child.name() == "Door"sv) {
            doors.push_back(make_unique<Door>(child));
        }
    }    
}

void Room::discover() {
    if (monster > 0) state = Room_State::Active;
    else clearRoom();
}

void Room::killMonster(int i) {
    monster--;
    entities.erase(entities.begin() + i);
    if (monster == 0) clearRoom();
}

void Room::clearRoom() {
    state = Room_State::Cleared;
    for (auto& door : doors) { 
        entities.push_back(move(door)); //Lorsque la room est Cleared, on met les portes dans entities, pour qu'elles soient affich�es
    }
}

void Room::render(sf::RenderWindow* mWindow) const {
    for (auto const& entity : entities) {
        entity.get()->render(mWindow);
    }
}

std::string Room::dump(std::string const& indent) const{

    ostringstream oss;
    oss << indent << "Room " << label << " [" << endl;
    for (auto const& c : entities) {
        oss << c->dump(indent + "| ");
    }
    oss << indent << "]" << endl;
    return oss.str();
}
