#include "StateGameObject.h"
#include "StateTransition.h"
#include "StateMachine.h"
#include "State.h"
#include "PhysicsObject.h"
#include "TutorialGame.h"
#include "NavigationGrid.h"

using namespace NCL;
using namespace CSC8503;

StateGameObject::StateGameObject() {
	counter = 0.0f;
	stateMachine = new StateMachine();
	GameObject* playerInfo = GetGameObject();

	State* stateA = new State([&](float dt)->void {
		this->MovePatrol(dt);
		});
	State* stateB = new State([&](float dt)->void {
		this->MoveRight(dt);
		});

	stateMachine->AddState(stateA);
	stateMachine->AddState(stateB);

	stateMachine->AddTransition(new StateTransition(stateA, stateB, [&]()->bool {
		return false;
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
	int i = 0;
	Vector3 moveDir = posList.at(instance) - this->GetTransform().GetPosition();
	this->GetPhysicsObject()->AddForce(Vector3(moveDir.Normalised().x * 10 , 0, moveDir.Normalised().z * 10));
	if ((moveDir*Vector3(1,0,1)).Length() < 10.0f) {
		(instance == 3 ? instance = 0 : instance++);
	}
	//counter += dt;
}

void StateGameObject::MoveRight(float dt) {
	GetPhysicsObject()->ClearForces();
	GetPhysicsObject()->AddForce({ 10, 0, 0 });
	counter -= dt;
}

void StateGameObject::ChasePlayer(GameObject* player) {
	float distance = player->GetTransform().GetPosition().Length() - this->GetTransform().GetPosition().Length();
	if (distance > 20) {
		//rest state
	}
	else if (distance < 20) {
		//follow player
		/*this->GetPhysicsObject()->AddForce(TutorialGame::PlayerMovementPace(dt, 
		player->GetTransform().GetOrientation() * Vector3(0, 0, -1)) * 20);*/
	}
}

//std::vector<Vector3> testNodes;
//void TestPathfinding() {
//	NavigationGrid grid("TestGrid1.txt");
//	NavigationPath outPath;
//
//	Vector3 startPos(80, 0, 10);
//	Vector3 endPos(80, 0, 80);
//
//	/*if (TutorialGame().GetPlayerPosition().x != 0) {
//		endPos = TutorialGame().GetPlayerPosition();
//	}*/
//
//	bool found = grid.FindPath(startPos, endPos, outPath);
//
//	Vector3 pos;
//	while (outPath.PopWaypoint(pos)) {
//		testNodes.push_back(pos);
//	}
//}
//
//void DisplayPathfinding() {
//	for (int i = 1; i < testNodes.size(); ++i) {
//		Vector3 a = testNodes[i - 1];
//		Vector3 b = testNodes[i];
//
//		Debug::DrawLine(a, b, Vector4(0, 1, 0, 1));
//	}
//}