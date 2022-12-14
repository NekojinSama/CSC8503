#pragma once
#include "BonusInteract.h"
#include "TutorialGame.h"
#include "RenderObject.h"
#include "PhysicsObject.h"

using namespace NCL;
using namespace CSC8503;

BonusInteract::BonusInteract(GameWorld* world, TutorialGame* tutGame)
{
	tutoGame = tutGame;
	zaWorld = world;
}

BonusInteract::~BonusInteract() {

}

void BonusInteract::OnCollisionBegin(GameObject* otherObject) 
{
	if (otherObject == tutoGame->GetPlayer())
	{
		this->SetHealth(this->GetHealth() - 1);
		if (this->GetHealth() == 0)
		{
			this->isActive = false;
		}
		else {
			this->GetRenderObject()->SetColour(Debug::RED);
		}
	}
}