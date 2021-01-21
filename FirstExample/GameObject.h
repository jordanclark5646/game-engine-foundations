#pragma once
#include "glm\glm.hpp"
#include "glm\gtc\matrix_transform.hpp"
#include "glm\gtx\rotate_vector.hpp"
#include "vgl.h"
class GameObject {
	char type;
	glm::vec3 location;
	glm::vec3 direction;
	int timeCreated;
	bool alive;

public:
	GameObject(char type, glm::vec3 l, glm::vec3 d, int tc);

	glm::vec3 getLocation();

	glm::vec3 getDirection();

	void updateLocation();

	void setDead();

	bool isAlive();
	
	int getTimeCreated();

	void updateLocation(glm::vec3 charPos);

	char getType();

	bool checkIfTooClose(glm::vec3 bullPos);

	
	
};
