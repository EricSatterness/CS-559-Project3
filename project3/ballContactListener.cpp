#include "ballContactListener.h"

// http://soundbible.com/
// http://www.soundempire.com/

void BallContactListener::BeginContact(b2Contact* contact)
{
	box2dUserData* udA = static_cast<box2dUserData*>(contact->GetFixtureA()->GetBody()->GetUserData());
	box2dUserData* udB = static_cast<box2dUserData*>(contact->GetFixtureB()->GetBody()->GetUserData());

	// When a ball collides with the player handle the collision
	if (udA->objectType == OBJECT_TYPE_MOSHBALL && udB->objectType == OBJECT_TYPE_PLAYER)
	{
		static_cast<Moshball*>(udA->object)->StartContact();
		static_cast<Player*>(udB->object)->StartContact();

		PlaySound(TEXT("./media/anvil-impact.wav"),NULL,SND_FILENAME|SND_ASYNC);
	}

	else if (udA->objectType == OBJECT_TYPE_PLAYER && udB->objectType == OBJECT_TYPE_MOSHBALL)
	{
		static_cast<Moshball*>(udB->object)->StartContact();
		static_cast<Player*>(udA->object)->StartContact();

		PlaySound(TEXT("./media/anvil-impact.wav"),NULL,SND_FILENAME|SND_ASYNC);
	}

	// Player collides with enemy
	else if (udA->objectType == OBJECT_TYPE_ENEMY && udB->objectType == OBJECT_TYPE_PLAYER)
	{
		static_cast<Enemy*>(udA->object)->StartContact();
		static_cast<Player*>(udB->object)->StartContact();

		PlaySound(TEXT("./media/anvil-impact.wav"),NULL,SND_FILENAME|SND_ASYNC);
	}
	else if (udA->objectType == OBJECT_TYPE_PLAYER && udB->objectType == OBJECT_TYPE_ENEMY)
	{
		static_cast<Enemy*>(udB->object)->StartContact();
		static_cast<Player*>(udA->object)->StartContact();

		PlaySound(TEXT("./media/anvil-impact.wav"),NULL,SND_FILENAME|SND_ASYNC);
	}

	// When a ball collides with another ball
	if (udA->objectType == OBJECT_TYPE_MOSHBALL && udB->objectType == OBJECT_TYPE_MOSHBALL)
	{
		PlaySound(TEXT("./media/anvil-impact.wav"),NULL,SND_FILENAME|SND_ASYNC);
	}


	// When the player collides with wall
	else if (udA->objectType == OBJECT_TYPE_WALL && udB->objectType == OBJECT_TYPE_PLAYER)
	{
		static_cast<Player*>(udB->object)->StartContact();

		PlaySound(TEXT("./media/metal-impact.wav"),NULL,SND_FILENAME|SND_ASYNC);
	}

	else if (udA->objectType == OBJECT_TYPE_PLAYER && udB->objectType == OBJECT_TYPE_WALL)
	{
		static_cast<Player*>(udA->object)->StartContact();

		PlaySound(TEXT("./media/metal-impact.wav"),NULL,SND_FILENAME|SND_ASYNC);
	}

	// When a ball collides with a wall
	else if (udA->objectType == OBJECT_TYPE_WALL && udB->objectType == OBJECT_TYPE_MOSHBALL)
	{
		PlaySound(TEXT("./media/metal-impact.wav"),NULL,SND_FILENAME|SND_ASYNC);
	}

	else if (udA->objectType == OBJECT_TYPE_MOSHBALL && udB->objectType == OBJECT_TYPE_WALL)
	{
		PlaySound(TEXT("./media/metal-impact.wav"),NULL,SND_FILENAME|SND_ASYNC);
	}

	// When the enemy collides with a wall
	else if (udA->objectType == OBJECT_TYPE_WALL && udB->objectType == OBJECT_TYPE_ENEMY)
	{
		PlaySound(TEXT("./media/metal-impact.wav"),NULL,SND_FILENAME|SND_ASYNC);
	}

	else if (udA->objectType == OBJECT_TYPE_ENEMY && udB->objectType == OBJECT_TYPE_WALL)
	{
		PlaySound(TEXT("./media/metal-impact.wav"),NULL,SND_FILENAME|SND_ASYNC);
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
//
//	// When the player collides with wall
//	else if (udA->objectType == OBJECT_TYPE_WALL && udB->objectType == OBJECT_TYPE_PLAYER)
//	{
//		static_cast<Player*>(udB->object)->StartContact();
//	}
//
//	else if (udA->objectType == OBJECT_TYPE_PLAYER && udB->objectType == OBJECT_TYPE_WALL)
//	{
//		static_cast<Player*>(udA->object)->StartContact();
//	}
//}

//void BallContactListener::PostSolve(b2Contact* contact, const b2ContactImpulse* impulse)
//{
//	box2dUserData* udA = static_cast<box2dUserData*>(contact->GetFixtureA()->GetBody()->GetUserData());
//	box2dUserData* udB = static_cast<box2dUserData*>(contact->GetFixtureB()->GetBody()->GetUserData());
//
//	// When a ball collides with the player handle the collision
//	if (udA->objectType == OBJECT_TYPE_MOSHBALL && udB->objectType == OBJECT_TYPE_PLAYER)
//	{
//		//static_cast<Moshball*>(udA->object)->StartContact();
//		static_cast<Player*>(udB->object)->StartContact();
//	}
//
//	else if (udA->objectType == OBJECT_TYPE_PLAYER && udB->objectType == OBJECT_TYPE_MOSHBALL)
//	{
//		//static_cast<Moshball*>(udB->object)->StartContact();
//		static_cast<Player*>(udA->object)->StartContact();
//	}
//
//	// When the player collides with wall
//	else if (udA->objectType == OBJECT_TYPE_WALL && udB->objectType == OBJECT_TYPE_PLAYER)
//	{
//		static_cast<Player*>(udB->object)->StartContact();
//	}
//
//	else if (udA->objectType == OBJECT_TYPE_PLAYER && udB->objectType == OBJECT_TYPE_WALL)
//	{
//		static_cast<Player*>(udA->object)->StartContact();
//	}
//}