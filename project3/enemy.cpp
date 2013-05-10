#include "Enemy.h"

using namespace std;
using namespace glm;

Enemy::Enemy()
{
	// OpenGL sphere that will represent the Enemy
	this->sphere = new Sphere(vec3(0.0f, 0.3f, 1.0f));
}

bool Enemy::Initialize(b2Vec2 center, float radius, int slices, int stacks)
{
	this->rotation = 0.0f;
	this->hit = false;

	// Make the Enemy's Box2D circle
	b2BodyDef EnemyDef;
	EnemyDef.type = b2_dynamicBody;
	EnemyDef.position.Set(center.x, center.y);
	//EnemyDef.bullet = true;
	this->body = world.CreateBody(&EnemyDef);

	box2dUserData *u = new box2dUserData();
	u->objectType = OBJECT_TYPE_ENEMY;
	u->object = this;
	this->body->SetUserData(u);

	b2CircleShape EnemyShape;
	EnemyShape.m_radius = radius;

	b2FixtureDef EnemyFixture;
	EnemyFixture.shape = &EnemyShape;
	EnemyFixture.density = 0.1f;
	EnemyFixture.friction = 0.3f;
	EnemyFixture.restitution = 1.0f;
	this->body->CreateFixture(&EnemyFixture);

	sphere->color = vec3(0.0f, 0.0f, 1.0f);
	if (!this->sphere->Initialize(radius, slices, stacks))
		return false;

	return true;
}

void Enemy::Draw(const glm::mat4 & projection, glm::mat4 modelview, const glm::ivec2 & size)
{
	// Translate modelview based on box2D position
	b2Vec2 position = this->body->GetPosition();
	mat4 m = translate(modelview, vec3(position.x, position.y, 0.0f));

	this->sphere->Draw(projection, m, size);
}

void Enemy::StartContact()
{
	//this->hitVelocity = this->body->GetLinearVelocity();
	this->hit = true;
	b2Vec2 v = this->body->GetLinearVelocity();

	////float tan = atan2(-v.y, v.x);
	//float angle = 180.0f/3.14f * atan2(-v.y, v.x);
	////float tan = atan2(Enemy->hitVelocity.y, -Enemy->hitVelocity.x);
	////angle = 180.0f/3.14f * tan;
	//if (angle < 0)
	//	angle += 360;

	//printf("%4.2f %4.2f %4.2f\n", v.x, v.y, angle);
	//this->rotation = angle;
}

void Enemy::TakeDown()
{
	this->sphere->TakeDown();
	delete this->sphere;
	world.DestroyBody(this->body);
}