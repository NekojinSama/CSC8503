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
            void goBeserk(GameObject* player);
            void ChasePlayer(GameObject* player);

            vector <Vector3> posList = { Vector3(180, 0, -130), Vector3(120, 0, -170), Vector3(80, 0, -170), Vector3(80, 0, -130) };

            StateMachine*   stateMachine;
            bool            chase = false;
            float           counter;
            int             instance;
            GameObject*     playerObj;

        private:
            
        };
    }
}
