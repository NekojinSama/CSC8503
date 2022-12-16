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
	counter = 0.0f;
	stateMachine = new StateMachine();
	GameObject* playerInfo = GetGameObject();
	TestPathfinding();

	State* stateA = new State([&](float dt)->void {
		this->MovePatrol(GetGameObject());
		});
	State* stateB = new State([&](float dt)->void {
		this->ChasePlayer(GetGameObject());
		});

	stateMachine->AddState(stateA);
	stateMachine->AddState(stateB);

	stateMachine->AddTransition(new StateTransition(stateA, stateB, [&]()->bool {
		return chase;
		}));
	stateMachine->AddTransition(new StateTransition(stateB, stateA, [&]()->bool {
		return !chase;
		}));
}

AiPathFollow::~AiPathFollow() {
	delete stateMachine;
}

void AiPathFollow::Update(float dt) {
	stateMachine->Update(dt);
	//TestBehaviourTree();
	DisplayPathfinding();
}



void AiPathFollow::ChasePlayer(GameObject* player) {
	Vector2 Dis, Obj;
	Dis = Vector2(player->GetTransform().GetPosition().x, player->GetTransform().GetPosition().z);
	Obj = Vector2(this->GetTransform().GetPosition().x, this->GetTransform().GetPosition().z);
	float distance = (Dis - Obj).Length();
	Vector3 moveDir = player->GetTransform().GetPosition() - this->GetTransform().GetPosition();
	this->GetPhysicsObject()->AddForce(Vector3(moveDir.Normalised().x * 10, 0, moveDir.Normalised().z * 10));
	if (distance > 20) {
		chase = false;
	}
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

void AiPathFollow::MovePatrol(GameObject* player) {
	GetPhysicsObject()->ClearForces();
	int i = 0;
	Vector3 moveDir = (testNodes.at(curInstance)) - this->GetTransform().GetPosition();
	this->GetPhysicsObject()->AddForce(Vector3(moveDir.Normalised().x, 0, moveDir.Normalised().z));
	if ((moveDir*Vector3(1,0,1)).Length() < 1.0f && forward ) {
		curInstance++;
		if (curInstance == testNodes.size() - 1) { forward = false; }
	}

	if ((moveDir * Vector3(1, 0, 1)).Length() < 1.0f && !forward) {
		curInstance--;
		if (curInstance == 0) { forward = true; }
	}
	
	Vector2 Dis, Obj;
	Dis = Vector2(player->GetTransform().GetPosition().x, player->GetTransform().GetPosition().z);
	Obj = Vector2(this->GetTransform().GetPosition().x, this->GetTransform().GetPosition().z);
	float distance = (Dis - Obj).Length();
	if (distance < 20) {
		chase = true;
	}
}

void AiPathFollow::TestBehaviourTree() {
	float behaviourTimer;
	float distanceToTarget;
	BehaviourAction* findKey = new BehaviourAction("Find Key", [&](float dt, BehaviourState state)->BehaviourState {
		if (state == Initialise) {
			std::cout << "Looking for a key\n";
			behaviourTimer = rand() % 100;
			state = Ongoing;
		}
		else if (state == Ongoing) {
			behaviourTimer -= dt;
			if (behaviourTimer <= 0.0f) {
				std::cout << "Found a key!\n";
				return Success;
			}
		}
	return state;
		});

	BehaviourAction* goToRoom = new BehaviourAction("Go To Room", [&](float dt, BehaviourState state)->BehaviourState {
		if (state == Initialise) {
			std::cout << "Going to loot room\n";
			state = Ongoing;
		}
		else if (state == Ongoing) {
			distanceToTarget -= dt;
			if (distanceToTarget <= 0.0f) {
				std::cout << "Reached room!\n";
				return Success;
			}
		}
	return state;
		});

	BehaviourAction* openDoor = new BehaviourAction("Open Door", [&](float dt, BehaviourState state)->BehaviourState {
		if (state == Initialise) {
			std::cout << "Opening Door\n";
			return Success;
		}
	return state;
		});

	BehaviourAction* lookForTreasure = new BehaviourAction("Look For Treasure", [&](float dt, BehaviourState state)->BehaviourState {
		if (state == Initialise) {
			std::cout << "Looking for treasure\n";
			return Ongoing;
		}
		else if (state == Ongoing) {
			bool found = rand() % 2;
			if (found) {
				std::cout << "I found some treasure!\n";
				return Success;
			}
			std::cout << "No treasure in here..\n";
			return Failure;
		}
	return state;
		});

	BehaviourAction* lookForItems = new BehaviourAction("Look For Items", [&](float dt, BehaviourState state)->BehaviourState {
		if (state == Initialise) {
			std::cout << "Looking for items\n";
			return Ongoing;
		}
		else if (state == Ongoing) {
			bool found = rand() % 2;
			if (found) {
				std::cout << "I found some items!\n";
				return Success;
			}
			std::cout << "No treasure in here..\n";
			return Failure;
		}
	return state;
		});

	BehaviourSequence* sequence = new BehaviourSequence("Room Sequence");
	sequence->AddChild(findKey);
	sequence->AddChild(goToRoom);
	sequence->AddChild(openDoor);

	BehaviourSelector* selection = new BehaviourSelector("Loot Selector");
	selection->AddChild(lookForTreasure);
	selection->AddChild(lookForItems);

	BehaviourSequence* rootSequence = new BehaviourSequence("Root Sequence");
	rootSequence->AddChild(sequence);
	rootSequence->AddChild(selection);

	for (int i = 0; i < 5; i++) {
		rootSequence->Reset();
		behaviourTimer = 0.0f;
		distanceToTarget = rand() % 250;
		BehaviourState state = Ongoing;
		std::cout << "We're going on an Adventure!\n";
		while (state == Ongoing) {
			state = rootSequence->Execute(1.0f);
		}
		if (state == Success) {
			std::cout << "Sucessful Adventure!\n";
		}
		else if (state == Failure) {
			std::cout << "Unsucessful Adventure!\n";
		}
	}
	std::cout << "All done!\n";
}