#pragma once
#include "Constraint.h"

namespace NCL {
	namespace CSC8503 {
		class GameObject;

		class PositionConstraintY : public Constraint	{
		public:
			PositionConstraintY(GameObject* a, GameObject* b, float d);
			~PositionConstraintY();

			void UpdateConstraint(float dt) override;

		protected:
			GameObject* objectA;
			GameObject* objectB;

			float distance;
		};
	}
}