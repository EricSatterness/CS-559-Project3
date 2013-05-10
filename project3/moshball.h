#pragma once
//#include <time.h>
//#include <Windows.h>
#include <limits>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <Box2D/Box2D.h>
#include "Globals.h"
#include "Sphere.h"

class Moshball
{
public:
	b2Body* body;
	float radius;
	float startTimeMinusPauses;
	bool displayTimer;
	//float rotation;
	float rotX, rotY;
	//glm::mat4 rotMat;
	glm::fquat rotQuat;

	Moshball();
	bool Initialize(glm::vec3 center, float radius, int slices, int stacks);
	void Draw(const glm::mat4 & projection, glm::mat4 modelview, const glm::ivec2 & size);
	void StartContact();
	//DWORD WINAPI Timer(LPVOID lpParameter);
	void CheckTimer(float currentTime);
	void TakeDown();
	int time;

private:
	Sphere* sphere;
};