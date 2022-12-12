#pragma once
#include "GameObject.h"
#include "GameWorld.h"

namespace NCL {
	namespace CSC8503 {
		class TutorialGame;
		class BonusInteract : public GameObject, public GameWorld{
		public:
			BonusInteract(GameWorld* world, TutorialGame* tutGame);
			~BonusInteract();

			void OnCollisionBegin(GameObject* otherObject) override;

		protected:
			TutorialGame* tutoGame;
			GameWorld* zaWorld;
		};
	}
}
