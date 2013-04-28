#include "moshball.h"

using namespace std;
using namespace glm;

Moshball::Moshball()
{
	// OpenGL sphere that will represent the ball
	this->sphere = new Sphere(vec3(0.27f, 1.0f, 0.0f));
	this->startTime = -numeric_limits<float>::max();
	this->displayTimer = false;

	// Don't use a timer or threading. We already have a timer: the display method
	//Start the timer
	//this->Timer();
	//CreateThread(NULL, 0, this->Timer, 
}

bool Moshball::Initialize(vec3 center, float radius, int slices, int stacks)
{
	// Make the moshball's Box2D circle
	b2BodyDef ballDef;
	ballDef.type = b2_dynamicBody;
	ballDef.position.Set(center.x, center.y);
	this->body = world.CreateBody(&ballDef);

	box2dUserData *u = new box2dUserData();
	u->objectType = OBJECT_TYPE_MOSHBALL;
	u->object = this;
	this->body->SetUserData(u);

	b2CircleShape ballShape;
	ballShape.m_radius = radius;

	b2FixtureDef ballFixture;
	ballFixture.shape = &ballShape;
	ballFixture.density = 1.0f;
	ballFixture.friction = 0.0f;
	ballFixture.restitution = 1.0f;
	this->body->CreateFixture(&ballFixture);
	this->body->SetLinearDamping(0.25f);
	

	if (!this->sphere->Initialize(radius, slices, stacks))
		return false;

	return true;
}

void Moshball::Draw(const glm::mat4 & projection, glm::mat4 modelview, const glm::ivec2 & size)
{
	// Translate modelview based on box2D position
	b2Vec2 position = this->body->GetPosition();
	mat4 m = translate(modelview, vec3(position.x, position.y, 0.0f));

	this->sphere->Draw(projection, m, size);
}

void Moshball::StartContact()
{
	// Change the sphere's color and reset the timer

	// This works but it slows down the rendering. Visually, we see the delay
	/*this->sphere->TakeDown();
	delete this->sphere;
	this->sphere = new Sphere(vec3(1.0f, 0.27f, 0.0f));
	this->sphere->Initialize(1.0f, 30, 30);*/

	this->startTime = currentTime;

	if (this->displayTimer == false)
	{
		//this->sphere->ChangeColor(vec3(1.0f, 0.27f, 0.0f));
		this->displayTimer = true;
		targetsRemaining -= 1;
	}
}

//DWORD WINAPI Moshball::Timer(LPVOID lpParameter)
//{
//	// Decrement the counter every 1 second
//	time_t timer;
//
//	while(true)
//	{
//		time(&timer);
//		double seconds = difftime(timer, this->timeStart);
//
//		if (seconds < 4 && seconds > 0)
//		{
//			// Display timer
//		}
//		else if (seconds <= 0)
//		{
//			// Reset the color
//			this->sphere->ChangeColor(vec3(0.27f, 1.0f, 0.0f));
//		}
//
//		//Sleep(1000);
//	}
//}

void Moshball::CheckTimer(float currentTime)
{
	// Only change the variables once
	if ((currentTime - this->startTime) > countDownTimerSeconds && this->displayTimer == true)
	{
		//this->sphere->ChangeColor(vec3(0.27f, 1.0f, 0.0f));
		this->displayTimer = false;
		targetsRemaining += 1;
	}
}

void Moshball::TakeDown()
{
	this->sphere->TakeDown();
	delete this->sphere;
	world.DestroyBody(this->body);
}