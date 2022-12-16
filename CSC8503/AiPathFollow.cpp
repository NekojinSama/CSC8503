#include "AiPathFollow.h"
#include "StateTransition.h"
#include "StateMachine.h"
#include "State.h"
#include "PhysicsObject.h"
#include "TutorialGame.h"
#include "NavigationGrid.h"

#include "BehaviourNode.h"
#include "BehaviourSelector.h"
#include "BehaviourSequence.h"
#include "BehaviourAction.h"

using namespace NCL;
using namespace CSC8503;

AiPathFollow::AiPathFollow() {
	stateMachine = new StateMachine();
	TestPathfinding();

	State* stateA = new State([&](float dt)->void {
		this->MovePatrol();
		});

	stateMachine->AddState(stateA);
}

AiPathFollow::~AiPathFollow() {
	delete stateMachine;
}

void AiPathFollow::Update(float dt) {
	stateMachine->Update(dt);
	//TestBehaviourTree();
	DisplayPathfinding();
}

std::vector<Vector3> testNodes;
void AiPathFollow::TestPathfinding() {
	NavigationGrid grid("TestGrid2.txt");
	NavigationPath outPath;

	/*Vector3 startPos(this->GetTransform().GetPosition());*/
	Vector3 startPos(380,0,230);
	/*Vector3 endPos(GetGameObject()->GetTransform().GetPosition());*/
	Vector3 endPos(380,0,290);

	/*if (TutorialGame().GetPlayerPosition().x != 0) {
		endPos = TutorialGame().GetPlayerPosition();
	}*/

	bool found = grid.FindPath(startPos, endPos, outPath);

	Vector3 pos;
	while (outPath.PopWaypoint(pos)) {
		testNodes.push_back(pos);
	}
}

void AiPathFollow::DisplayPathfinding() {
	for (int i = 1; i < testNodes.size(); ++i) {
		Vector3 a = testNodes[i - 1];
		Vector3 b = testNodes[i];

		Debug::DrawLine(a, b, Vector4(0, 1, 0, 1));
	}
}

void AiPathFollow::MovePatrol() {
	GetPhysicsObject()->ClearForces();
	int i = 0;
	Vector3 moveDir = (testNodes.at(curInstance)) - this->GetTransform().GetPosition();
	Vector3 direction = moveDir.Normalised();
	//this->GetTransform().SetPosition(this->GetTransform().GetOrientation() * direction);
	//this->GetTransform().SetOrientation(Vector3(moveDir.Normalised().x, 0, moveDir.Normalised().z));
	this->GetPhysicsObject()->AddForce(Vector3(moveDir.Normalised().x, 0, moveDir.Normalised().z));
	if ((moveDir*Vector3(1,0,1)).Length() < 1.0f && forward ) {
		curInstance++;
		if (curInstance == testNodes.size() - 1) { forward = false; }
	}

	if ((moveDir * Vector3(1, 0, 1)).Length() < 1.0f && !forward) {
		curInstance--;
		if (curInstance == 0) { forward = true; }
	}
}