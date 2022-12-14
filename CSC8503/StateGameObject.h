#pragma once
#include "GameObject.h"
//#include "TutorialGame.h"
#include <vector>

namespace NCL {
    namespace CSC8503 {
        class StateMachine;
        class TutorialGame;
        class StateGameObject : public GameObject /*, public TutorialGame */{
        public:
            StateGameObject();
            ~StateGameObject();

            virtual void Update(float dt);
            

        protected:
            void MoveLeft(float dt);
            void MoveRight(float dt);
            void MovePatrol(float dt);
            void ChasePlayer(Vector3 player);

            vector <Vector3> posList = { Vector3(0, 0, -100), Vector3(90, 0, -100), Vector3(90, 0, -150), Vector3(40, 0, -150) };

            StateMachine* stateMachine;
            float counter;
            int instance;
            TutorialGame* game;

        private:
            
        };
    }
}
