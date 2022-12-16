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
            void SetGameObject(GameObject* player) {
                playerObj = player;
            }
            GameObject* GetGameObject(){
                return playerObj;
            }
            
            void TestPathfinding();
            void DisplayPathfinding();
            void TestBehaviourTree();

        protected:
            void MovePatrol(GameObject* player);
            void ChasePlayer(GameObject* player);

            vector <Vector3> posList = { Vector3(120, 0, -140), Vector3(120, 0, -190), Vector3(50, 0, -190), Vector3(50, 0, -140) };

            StateMachine*   stateMachine;
            bool            chase = false;
            bool            forward = true;
            float           counter;
            int             curInstance;
            GameObject*     playerObj;

        private:
            
        };
    }
}
