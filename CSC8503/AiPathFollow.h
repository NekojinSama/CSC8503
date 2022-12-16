#pragma once
#include "GameObject.h"
//#include "TutorialGame.h"
#include <vector>

namespace NCL {
    namespace CSC8503 {
        class StateMachine;
        class TutorialGame;
        class AiPathFollow : public GameObject /*, public TutorialGame */{
        public:
            AiPathFollow();
            ~AiPathFollow();

            virtual void Update(float dt);
            void TestPathfinding();
            void DisplayPathfinding();

        protected:
            void MovePatrol();

            StateMachine*   stateMachine;
            bool            forward = true;
            int             curInstance;

        private:
            
        };
    }
}
