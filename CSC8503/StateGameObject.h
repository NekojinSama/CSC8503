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
            void MoveLeft(float dt);
            void MoveRight(float dt);
            void MovePatrol(GameObject* player);
            void ChasePlayer(GameObject* player);

            vector <Vector3> posList = { Vector3(120, 0, -140), Vector3(120, 0, -190), Vector3(50, 0, -190), Vector3(50, 0, -140) };

            StateMachine*   stateMachine;
            bool            chase = false;
            float           counter;
            int             instance;
            GameObject*     playerObj;

        private:
            
        };
    }
}
