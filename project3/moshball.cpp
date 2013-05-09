#include "moshball.h"

using namespace std;
using namespace glm;

Moshball::Moshball()
{
	// OpenGL sphere that will represent the ball
	this->sphere = new Sphere(vec3(0.27f, 1.0f, 0.0f));
	this->radius = 0.0f;
	this->startTimeMinusPauses = -numeric_limits<float>::max();
	this->displayTimer = false;
	//this->rotation = 0.0f;
	this->rotX = 0.0f;
	this->rotY = 0.0f;
	this->rotMat = mat4(1.0f);

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
	
	this->radius = radius;

	if (!this->sphere->Initialize(radius, slices, stacks))
		return false;

	return true;
}

void Moshball::Draw(const glm::mat4 & projection, glm::mat4 modelview, const glm::ivec2 & size)
{
	// Translate modelview based on box2D position
	b2Vec2 position = this->body->GetPosition();
	mat4 m = translate(modelview, vec3(position.x, position.y, 0.0f));

	// Make sphere roll. velocity = radius * angular_velocity
	b2Vec2 velocityB2 = this->body->GetLinearVelocity();
	if (velocityB2.x != 0.0f || velocityB2.y != 0.0f)
	{
		// This only handles one direction of rotation. If the velocity changes directions, we get a jumping motion in the texture
		vec3 velocity(velocityB2.x, velocityB2.y, 0.0f);
		vec3 rotationAxis = normalize(cross(vec3(0.0f, 0.0f, 1.0f), normalize(velocity)));
		float speed = sqrtf(pow(velocity.x, 2.0f) + pow(velocity.y, 2.0f));
		float rotation = speed / (this->radius *2* 3.141592654f);
		/*this->rotation += rotation;
		if (this->rotation > 360)
			this->rotation -= 360;
		else if (this->rotation < 0)
			this->rotation += 360;

		m = rotate(m, this->rotation, rotationAxis);*/

		float newRotX = velocityB2.x / (this->radius * 3.0f);
		float newRotY = velocityB2.y / (this->radius * 3.0f);
		/*this->rotX += newRotX;
		this->rotY += newRotY;

		if (this->rotX > 360)
			this->rotX -= 360;
		else if (this->rotX < 0)
			this->rotX += 360;
		if (this->rotY > 360)
			this->rotY -= 360;
		else if (this->rotY < 0)
			this->rotY += 360;

		m = rotate(m, this->rotX, vec3(0.0f, 1.0f, 0.0f));
		m = rotate(m, this->rotY, vec3(-1.0f, 0.0f, 0.0f));*/

		//this->rotMat = rotate(this->rotMat, newRotX, vec3(0.0f, 1.0f, 0.0f));
		//this->rotMat = rotate(this->rotMat, newRotY, vec3(-1.0f, 0.0f, 0.0f));
		this->rotMat = rotate(this->rotMat, rotation, rotationAxis);
	}

	m = m * this->rotMat;

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

	// Add time paused so that we have
	this->startTimeMinusPauses = currentTime - totalTimePaused;

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

void Moshball::CheckTimer(float currentTimeMinusPauses)
{
	// Only change the variables once
	time = countDownTimerSeconds - (currentTimeMinusPauses - this->startTimeMinusPauses);
	if ((currentTimeMinusPauses - this->startTimeMinusPauses) > countDownTimerSeconds && this->displayTimer == true)
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