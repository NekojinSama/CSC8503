#include "StateGameObject.h"
#include "StateTransition.h"
#include "StateMachine.h"
#include "State.h"
#include "PhysicsObject.h"

using namespace NCL;
using namespace CSC8503;

StateGameObject::StateGameObject() {
	counter = 0.0f;
	stateMachine = new StateMachine();
	GameObject* playerInfo = TutorialGame::player;

	State* stateA = new State([&](float dt)->void {
		this->MoveLeft(dt);
		});
	State* stateB = new State([&](float dt)->void {
		this->MoveRight(dt);
		});

	stateMachine->AddState(stateA);
	stateMachine->AddState(stateB);

	stateMachine->AddTransition(new StateTransition(stateA, stateB, [&]()->bool {
		return this->counter > 3.0f;
		}));
	stateMachine->AddTransition(new StateTransition(stateB, stateA, [&]()->bool {
		return this->counter < 0.0f;
		}));
}

StateGameObject::~StateGameObject() {
	delete stateMachine;
}

void StateGameObject::Update(float dt) {
	stateMachine->Update(dt);
}

void StateGameObject::MoveLeft(float dt) {
	GetPhysicsObject()->ClearForces();
	GetPhysicsObject()->AddForce({ -10, 0, 0 });
	counter += dt;
}

//void StateGameObject::PatrolList() {
//	posList.push_back(Vector3(50, 10, 0));
//	posList.push_back(Vector3(50, 10, 50));
//	posList.push_back(Vector3(0, 10, 50));
//	posList.push_back(Vector3(0, 10, 0));
//}

void StateGameObject::MovePatrol(float dt) {
	GetPhysicsObject()->ClearForces();
	GetPhysicsObject()->AddForce((posList.at(0) / posList.at(0)));
	GetPhysicsObject()->ClearForces();
	//for (int i = 0; i < posList.size(); ++i) {
	//	while (GetTransform().GetPosition() != posList.at(i)) {
	//		GetPhysicsObject()->AddForce((posList.at(i)/ posList.at(i)));
	//		//std::cout << GetTransform().GetPosition() << std::endl;
	//	}
	//	if (i = posList.size() - 1) { i = 0; }
	//}
	//std::cout<<GetTransform().GetPosition()<<std::endl;
	counter += dt;
}

void StateGameObject::MoveRight(float dt) {
	GetPhysicsObject()->ClearForces();
	GetPhysicsObject()->AddForce({ 10, 0, 0 });
	counter -= dt;
}

void StateGameObject::ChasePlayer(Vector3 player) {
	float distance = player.Length() - this->GetTransform().GetPosition().Length();
	if (distance > 20) {
		//rest state
	}
	else if (distance < 20) {
		//follow player
		/*this->GetPhysicsObject()->AddForce(TutorialGame::PlayerMovementPace(dt, 
		player->GetTransform().GetOrientation() * Vector3(0, 0, -1)) * 20);*/
	}
}