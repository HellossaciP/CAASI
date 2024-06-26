#include "Game.h"
#include <iostream>
#include <sstream>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

using namespace std;

const sf::Time Game::TimePerFrame = sf::seconds(1.f / 60.f);

void Game::load()
{
	printf("Loading\n");

	pugi::xml_document doc; //Charge le niveau depuis le fichier rooms.xml, qui permet d'initialiser les rooms
	if (auto result = doc.load_file("resources/rooms.xml"); !result)
	{
		cerr << "Could not open file rooms.xml because " << result.description() << endl;
		exit(1);
	}

	for (auto child : doc.children())
	{
		if (child.name() == "Room"sv) {
			rooms.push_back(make_unique<Room>(child));
		}
		if (child.name() == "Player"sv) {
			player = make_unique<Player>(child);
		}
	}

	currentRoom = rooms.begin();
	(*currentRoom)->discover(); //Discover permet de dire que l'on d�couvre la room, changeant son statut suivant les monstres dedans

	setCallbacks();

	cout << currentRoom->get()->dump("");
	cout << player->dump("");

	initialiseSprites();
}

void Game::onPlayerCollision(Entity* entity) {
	if (entity->getLabel() == "Monster"sv) {
		loose = true;
		looseSound.play();
	}
	if (entity->getLabel() == "Door"sv) {
		if (auto door = dynamic_cast<Door*>(entity)) { //On double-v�rife le type de l'entit�, � travers son label et un dynamic_cast
			handleCollisionPlayerDoor(door);
		}
	}
}

void Game::kill(int i) {
	currentRoom->get()->killMonster(i);
	checkIfWin(); //La victoire ne peut avoir lieu qu'apres la mort d'un monstre, on v�rifie donc ici si le jeu est gagn� ou pas
}

void Game::checkIfWin() {
	win = true;
	for (auto it = rooms.begin(); it != rooms.end(); ++it) {
		if ((*it)->getState() != Room_State::Cleared)
		{
			win = false;
		}
	}
	if (win) winSound.play();
}

void Game::render()
{
	mWindow.clear();
	currentRoom->get()->render(&mWindow);
	player->render(&mWindow);
	if (win) mWindow.draw(winSprite);
	if (loose) mWindow.draw(looseSprite);
	mWindow.display();
}

void Game::update()
{
	if (!(win || loose)) player->update(currentRoom->get()->entities);
}

void Game::save () const
{
	//TODO
	printf("Saving");
}

void Game::processEvents()
{
	sf::Event event{ sf::Event::Count }; 
	while (mWindow.pollEvent(event))
	{
		switch (event.type)
		{
		case sf::Event::KeyPressed:
			player->manageInput(event.key.code, true);
			break;

		case sf::Event::KeyReleased:
			player->manageInput(event.key.code, false);
			break;

		case sf::Event::MouseButtonPressed:
			player->shoot(sf::Mouse::getPosition(mWindow));
			break;

		case sf::Event::Closed:
			save();
			mWindow.close();
			break;

		default:
			break;
		}
	}
}

void Game::run()
{
	sf::Clock clock;
	sf::Time timeSinceLastUpdate = sf::Time::Zero;
	load();
	while (mWindow.isOpen())
	{
		sf::Time elapsedTime = clock.restart();
		timeSinceLastUpdate += elapsedTime;
		while (timeSinceLastUpdate > TimePerFrame)
		{
			timeSinceLastUpdate -= TimePerFrame;

			processEvents();
			update();
		}
		render();
	}
}

void Game::setCallbacks() {
	player->setCollisionCallback(this, &Game::onPlayerCollision);
	player->setKillCallback(this, &Game::kill);
}

void Game::handleCollisionPlayerDoor(const Door* door) {
	for (auto it = rooms.begin(); it != rooms.end(); ++it) {
		cout << (*it)->getLabel() << " == " << door->getDestination() << " : ";
		if ((*it)->getLabel() == door->getDestination()) {
			currentRoom = it;
			std::cout << "Switched to room: " << door->getDestination() << std::endl;
			if ((*it)->getState() == Room_State::Undiscovered) (*it)->discover();
			player->updatePositionWhenChangingRoom();
			return;
		}
		else { cout << "false !\n"; }
	}
}

void Game::initialiseSprites() {
	winTexture.loadFromFile("resources/sprites/Win.png");
	winSprite.setScale(sf::Vector2f(3, 3));
	winSprite.setTexture(winTexture);
	winSprite.setPosition(sf::Vector2f(((float)mWindow.getSize().x - winSprite.getGlobalBounds().width) / 2, ((float)mWindow.getSize().y - winSprite.getGlobalBounds().height) / 2));
	looseTexture.loadFromFile("resources/sprites/Loose.png");
	looseSprite.setScale(sf::Vector2f(3, 3));
	looseSprite.setTexture(looseTexture);
	looseSprite.setPosition(sf::Vector2f(((float)mWindow.getSize().x - looseSprite.getGlobalBounds().width) / 2, ((float)mWindow.getSize().y - looseSprite.getGlobalBounds().height) / 2));

	winSoundBuffer.loadFromFile("resources/audios/win.mp3");
	winSound.setBuffer(winSoundBuffer);
	looseSoundBuffer.loadFromFile("resources/audios/loose.mp3");
	looseSound.setBuffer(looseSoundBuffer);
}