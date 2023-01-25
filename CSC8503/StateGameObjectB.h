#pragma once
#include "GameObject.h"
#include "BehaviourSelector.h"
#include "BehaviourSequence.h"
//#include "TutorialGame.h"
#include <vector>

namespace NCL {
    namespace CSC8503 {
        class StateMachine;
        class TutorialGame;
        class GameWorld;
        class StateGameObjectB : public GameObject /*, public TutorialGame */{
        public:
            StateGameObjectB();
            ~StateGameObjectB();

            virtual void Update(float dt);
            void SetGameObject(GameObject* player) {
                playerObj = player;
            }
            GameObject* GetGameObject(){
                return playerObj;
            }

            void SetWorld(GameWorld* ptr) {
                world = ptr;
            }
            GameWorld* GetWorld() {
                return world;
            }

            BehaviourState getState() {
                return state;
            }
            
            void TestBehaviourTree();

        protected:
            void drawRay(GameObject* player);
            void MovePatrol(GameObject* player);
            void goBeserk(GameObject* player);
            void ChasePlayer(GameObject* player);

            vector <Vector3> posList2 = { Vector3(100, 0, -140), Vector3(50, 0, -140), Vector3(50, 0, -60), Vector3(100, 0, -60) };

            StateMachine*   stateMachine;
            bool            chase = false;
            float           counter;
            int             instance;
            GameObject*     playerObj;
            GameWorld*          world;
            BehaviourSequence* rootSequence;
            BehaviourState  state;

        private:
            
        };
    }
}