#include "player.h"

using namespace std;
using namespace glm;

Player::Player()
{
	// OpenGL sphere that will represent the player
	this->sphere = new Sphere(vec3(0.0f, 0.3f, 1.0f));
}

bool Player::Initialize(float radius, int slices, int stacks)
{
	this->rotation = 0.0f;
	this->hit = false;

	// Make the player's Box2D circle
	b2BodyDef playerDef;
	playerDef.type = b2_dynamicBody;
	playerDef.position.Set(0.0f,0.0f);
	//playerDef.bullet = true;
	this->body = world.CreateBody(&playerDef);

	box2dUserData *u = new box2dUserData();
	u->objectType = OBJECT_TYPE_PLAYER;
	u->object = this;
	this->body->SetUserData(u);

	b2CircleShape playerShape;
	playerShape.m_radius = radius;

	b2FixtureDef playerFixture;
	playerFixture.shape = &playerShape;
	playerFixture.density = 1.0f;
	playerFixture.friction = 0.0f;
	playerFixture.restitution = 1.0f;
	this->body->CreateFixture(&playerFixture);

	if (!this->sphere->Initialize(radius, slices, stacks))
		return false;

	return true;
}

void Player::Draw(const glm::mat4 & projection, glm::mat4 modelview, const glm::ivec2 & size)
{
	// Translate modelview based on box2D position
	b2Vec2 position = this->body->GetPosition();
	mat4 m = translate(modelview, vec3(position.x, position.y, 0.0f));

	this->sphere->Draw(projection, m, size);
}

void Player::StartContact()
{
	this->hitVelocity = this->body->GetLinearVelocity();
	this->hit = true;
}

void Player::TakeDown()
{
	this->sphere->TakeDown();
	delete this->sphere;
	world.DestroyBody(this->body);
}