#pragma once
#include "GameObject.h"
#include <vector>

namespace NCL {
    namespace CSC8503 {
        class StateMachine;
        class StateGameObject : public GameObject  {
        public:
            StateGameObject();
            ~StateGameObject();

            virtual void Update(float dt);
            

        protected:
            void MoveLeft(float dt);
            void MoveRight(float dt);
            void MovePatrol(float dt);
            void PatrolList();

            vector <Vector3> posList = { Vector3(50, 10, 0), Vector3(50, 10, 50), Vector3(0, 10, 50), Vector3(0, 10, 0) };

            StateMachine* stateMachine;
            float counter;

        private:
            
        };
    }
}
