#include "wall.h"

using namespace std;
using namespace glm;

// TODO: Change the OpenGL object to be a 3D rectangle

Wall::Wall()
{
	// OpenGL rectangle to represent the wall
	this->square = new Square();
}

// NOTE: width, height, and depth are measured from the center of the shape
bool Wall::Initialize(vec3 center, float width, float height, float depth)
{
	// Make a wall
	// Scale: 1 meter = 50 feet
	//float wall_t = 0.5f;
	// Want player area to be 5280sqft, so add wall thickness to make the lengths correct
	//float wall_l = 105.6f + wall+_t;

	//float height_2 = height / 2.0f;
	//float width_2 = width / 2.0f;


	b2BodyDef wallBodyDef;
	wallBodyDef.position.Set(center.x, center.y);
	this->body = world.CreateBody(&wallBodyDef);

	b2PolygonShape wallShape;
	wallShape.SetAsBox(width, height);
	this->body->CreateFixture(&wallShape, 1.0f);

	/*vec3 p1(center.x + width, center.y + height, center.z);
	vec3 p3(center.x - width, center.y - height, center.z);
	vec3 p2(center.x + width, center.y - height, center.z);
	vec3 p4(center.x - width, center.y + height, center.z);*/

	vec3 p1(width, height, center.z);
	vec3 p3(-width, -height, center.z);
	vec3 p2(width, -height, center.z);
	vec3 p4(-width, height, center.z);

	if (!this->square->Initialize(p1, p2, p3, p4))
		return false;

	return true;
}

void Wall::Draw(const glm::mat4 & projection, glm::mat4 modelview, const glm::ivec2 & size)
{
	// Translate modelview based on box2D position
	b2Vec2 position = this->body->GetPosition();
	mat4 m = translate(modelview, vec3(position.x, position.y, 0.0f));

	this->square->Draw(projection, m, size);
}

void Wall::TakeDown()
{
	this->square->TakeDown();
	delete this->square;
	world.DestroyBody(this->body);
}