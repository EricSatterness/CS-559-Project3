#pragma once
#include <Box2D/Box2D.h>
#include "Globals.h"
#include "moshball.h"

// Make UserData variable on the bodies to indicate when a ball has collided with the player or another ball
// Use contact listener and check the bodies that collided to determine if this needs to be set
// Then, have an method that handles the spehere's reactions to the collisions that gets run after every world step (this is where we will clear the variable, once it has been handled)
// http://www.iforce2d.net/b2dtut/collision-callbacks

// UserData
// -Object type: player, ball, wall
// -Collided with ball: true or false. Return to false after timer is updated on ball (only used by moshballs)

// Override the contact listener so we can handle the case where a ball collides with the player or another ball
class BallContactListener : public b2ContactListener
{
	void BeginContact(b2Contact* contact);
	//void EndContact(b2Contact* contact);
};