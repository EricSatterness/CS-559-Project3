#include "player.h"

// TODO: map the box2D player object to a openGL sphere

Player::Player()
{
	// Make the player's circle
	b2BodyDef playerDef;
	playerDef.type = b2_dynamicBody;
	playerDef.position.Set(0.0f,0.0f);
	//this->body = world.CreateBody(&playerDef);
	// Make world global variable

	b2CircleShape playerShape;
	playerShape.m_radius = 1.0f;

	b2FixtureDef playerFixture;
	playerFixture.shape = &playerShape;
	playerFixture.density = 1.0f;
	playerFixture.friction = 0.3f;
	playerFixture.restitution = 0.2f;
	this->body->CreateFixture(&playerFixture);
}