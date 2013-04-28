#include "ballContactListener.h"

void BallContactListener::BeginContact(b2Contact* contact)
{
	box2dUserData* udA = static_cast<box2dUserData*>(contact->GetFixtureA()->GetBody()->GetUserData());
	box2dUserData* udB = static_cast<box2dUserData*>(contact->GetFixtureB()->GetBody()->GetUserData());
	/*box2dUserData* udA = (box2dUserData*)contact->GetFixtureA()->GetBody()->GetUserData();
	box2dUserData* udB = (box2dUserData*)contact->GetFixtureB()->GetBody()->GetUserData();*/
	//b2Body* bodyA = contact->GetFixtureA()->GetBody();
	//b2Body* bodyB = contact->GetFixtureB()->GetBody();

	// When a ball collides with the player handle the collision
	if (udA->objectType == OBJECT_TYPE_MOSHBALL && udB->objectType == OBJECT_TYPE_PLAYER)
	{
		static_cast<Moshball*>(udA->object)->StartContact();
		static_cast<Player*>(udB->object)->StartContact();
	}

	else if (udA->objectType == OBJECT_TYPE_PLAYER && udB->objectType == OBJECT_TYPE_MOSHBALL)
	{
		static_cast<Moshball*>(udB->object)->StartContact();
		static_cast<Player*>(udA->object)->StartContact();
	}

	// When the player collides with wall
	else if (udA->objectType == OBJECT_TYPE_WALL && udB->objectType == OBJECT_TYPE_PLAYER)
	{
		static_cast<Player*>(udB->object)->StartContact();
	}

	else if (udA->objectType == OBJECT_TYPE_PLAYER && udB->objectType == OBJECT_TYPE_WALL)
	{
		static_cast<Player*>(udA->object)->StartContact();
	}
}

//void BallContactListener::EndContact(b2Contact* contact)
//{
//	box2dUserData* udA = static_cast<box2dUserData*>(contact->GetFixtureA()->GetBody()->GetUserData());
//	box2dUserData* udB = static_cast<box2dUserData*>(contact->GetFixtureB()->GetBody()->GetUserData());
//
//	// When a ball collides with the player handle the collision
//	if (udA->objectType == OBJECT_TYPE_MOSHBALL && udB->objectType == OBJECT_TYPE_PLAYER)
//	{
//		static_cast<Moshball*>(udA->object)->StartContact();
//		static_cast<Player*>(udB->object)->StartContact();
//	}
//
//	else if (udA->objectType == OBJECT_TYPE_PLAYER && udB->objectType == OBJECT_TYPE_MOSHBALL)
//	{
//		static_cast<Moshball*>(udB->object)->StartContact();
//		static_cast<Player*>(udA->object)->StartContact();
//	}
//}