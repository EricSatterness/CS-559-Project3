#include "player.h"

using namespace std;
using namespace glm;

Player::Player()
{
	// OpenGL sphere that will represent the player
	this->sphere = new Sphere(vec3(0.0f, 0.3f, 1.0f));
}

bool Player::Initialize(b2Vec2 center, float radius, int slices, int stacks)
{
	this->rotation = 0.0f;
	this->hit = false;

	// Make the player's Box2D circle
	b2BodyDef playerDef;
	playerDef.type = b2_dynamicBody;
	playerDef.position.Set(center.x, center.y);
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
	playerFixture.density = 0.1f;
	playerFixture.friction = 0.3f;
	playerFixture.restitution = 1.0f;
	this->body->CreateFixture(&playerFixture);

	sphere->color = vec3(0.0f, 0.0f, 1.0f);
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
	//this->hitVelocity = this->body->GetLinearVelocity();
	this->hit = true;
	b2Vec2 v = this->body->GetLinearVelocity();

	////float tan = atan2(-v.y, v.x);
	//float angle = 180.0f/3.14f * atan2(-v.y, v.x);
	////float tan = atan2(player->hitVelocity.y, -player->hitVelocity.x);
	////angle = 180.0f/3.14f * tan;
	//if (angle < 0)
	//	angle += 360;

	//printf("%4.2f %4.2f %4.2f\n", v.x, v.y, angle);
	//this->rotation = angle;
}

void Player::TakeDown()
{
	this->sphere->TakeDown();
	delete this->sphere;
	world.DestroyBody(this->body);
}