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
		
		switch (this->GetLayer()) {
		case 6:
			this->SetHealth(this->GetHealth() - 1);
			if (this->GetHealth() == 0)
			{
				this->isActive = false;
			}
			else {
				this->GetRenderObject()->SetColour(Debug::RED);
			}
			break;

		case 7:
			break;

		case 8:
			this->GetRenderObject()->SetColour(Debug::GREEN);
			BonusInteract::timer -= 0.1f;
			if (timer < 0.0f) {
				this->GetRenderObject()->SetColour(Debug::WHITE);
			}
			break;
		}
	}
}