#include "wall.h"

using namespace std;
using namespace glm;

// TODO: Change the OpenGL object to be a 3D rectangle

Wall::Wall()
{
	// OpenGL rectangle to represent the wall
	this->cube = new Cube();
}

// NOTE: width, height, and depth are measured from the center of the shape
bool Wall::Initialize(vec3 center, float width, float height, float depth)
{
	this->width = width;
	this->height = height;
	this->depth = depth;
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

	box2dUserData *u = new box2dUserData();
	u->objectType = OBJECT_TYPE_WALL;
	u->object = this;
	this->body->SetUserData(u);

	b2PolygonShape wallShape;
	wallShape.SetAsBox(width, height);
	this->body->CreateFixture(&wallShape, 1.0f);

	if(!this->cube->Initialize(1))
		return false;

	return true;
}

void Wall::Draw(const glm::mat4 & projection, glm::mat4 modelview, const glm::ivec2 & size)
{
	// Translate modelview based on box2D position
	b2Vec2 position = this->body->GetPosition();
	mat4 m = translate(modelview, vec3(position.x, position.y, 0.0f));

	this->cube->Draw(projection, scale(m, vec3(2*width, 2*height, 2*depth)), size);
}

void Wall::TakeDown()
{
	this->cube->TakeDown();
	delete this->cube;
	world.DestroyBody(this->body);
}