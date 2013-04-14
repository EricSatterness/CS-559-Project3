#include "moshball.h"

using namespace std;
using namespace glm;

Moshball::Moshball()
{
	// OpenGL sphere that will represent the ball
	this->sphere = new Sphere();
}

bool Moshball::Initialize(vec3 center, float radius, int slices, int stacks)
{
	// Make the moshball's Box2D circle
	b2BodyDef ballDef;
	ballDef.type = b2_dynamicBody;
	ballDef.position.Set(center.x, center.y);
	this->body = world.CreateBody(&ballDef);

	b2CircleShape ballShape;
	ballShape.m_radius = radius;

	b2FixtureDef ballFixture;
	ballFixture.shape = &ballShape;
	ballFixture.density = 1.0f;
	ballFixture.friction = 0.3f;
	ballFixture.restitution = 0.5f;
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

void Moshball::TakeDown()
{
	this->sphere->TakeDown();
	delete this->sphere;
	world.DestroyBody(this->body);
}