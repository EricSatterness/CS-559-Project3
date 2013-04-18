#pragma once
#include <Box2D/Box2D.h>
#include "Globals.h"
#include "moshball.h"

// Override the contact listener so we can handle the case where a ball collides with the player
class BallContactListener : public b2ContactListener
{
	void BeginContact(b2Contact* contact);
	//void EndContact(b2Contact* contact);
};