#include "GameObject.h"

GameObject::GameObject(char fType, glm::vec3 l, glm::vec3 d, int tc) {
	type = fType;
	location = l;
	direction = d;
	timeCreated = tc;
	alive = true;
}

glm::vec3 GameObject::getLocation() {
	return location;
}

glm::vec3 GameObject::getDirection()
{
	return direction;
}

void GameObject::updateLocation() {
	location += direction * 0.02f;
}

void GameObject::setDead() {
	alive = false;
}

bool GameObject::isAlive() {
	return alive;
}

int GameObject::getTimeCreated() {
	return timeCreated;
}

void GameObject::updateLocation(glm::vec3 charPos) {
	direction = normalize(charPos - location);
	location += direction * 0.003f;

	if (glm::distance(charPos, location) < 0.1f) {
		setDead();
	}
}

char GameObject::getType() {
	return type;
}

bool GameObject::checkIfTooClose(glm::vec3 bullPos)
{
	if (glm::distance(bullPos, location) < 1.1) {
		return true;
	}

	return false;
}

