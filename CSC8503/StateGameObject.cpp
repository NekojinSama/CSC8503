#include "StateGameObject.h"
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

StateGameObject::StateGameObject() {
	counter = 0.0f;
	stateMachine = new StateMachine();
	GameObject* playerInfo = GetGameObject();

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


void StateGameObject::MovePatrol(GameObject* player) {
	GetPhysicsObject()->ClearForces();
	int i = 0;
	Vector3 moveDir = (posList.at(instance) + Vector3(195,0,195)) - this->GetTransform().GetPosition();
	this->GetPhysicsObject()->AddForce(Vector3(moveDir.Normalised().x * 2 , 0, moveDir.Normalised().z * 2));
	if ((moveDir*Vector3(1,0,1)).Length() < 1.0f) {
		(instance == posList.size() - 1 ? instance = 0 : instance++);
	}
	Vector2 Dis, Obj;
	Dis = Vector2(player->GetTransform().GetPosition().x, player->GetTransform().GetPosition().z);
	Obj = Vector2(this->GetTransform().GetPosition().x, this->GetTransform().GetPosition().z);
	float distance = (Dis - Obj).Length();
	if (distance < 20) {
		chase = true;
	}
}

void StateGameObject::goBeserk(GameObject* player) {
	GetPhysicsObject()->ClearForces();
	int i = 0;
	Vector3 moveDir = (posList.at(instance) + Vector3(195, 0, 195)) - this->GetTransform().GetPosition();
	this->GetPhysicsObject()->AddForce(Vector3(moveDir.Normalised().x * 2, 10, moveDir.Normalised().z * 2));
	if ((moveDir * Vector3(1, 0, 1)).Length() < 1.0f) {
		(instance == posList.size() - 1 ? instance = 0 : instance++);
	}
	Vector2 Dis, Obj;
	Dis = Vector2(player->GetTransform().GetPosition().x, player->GetTransform().GetPosition().z);
	Obj = Vector2(this->GetTransform().GetPosition().x, this->GetTransform().GetPosition().z);
	float distance = (Dis - Obj).Length();
	if (distance < 20) {
		chase = true;
	}
}

void StateGameObject::ChasePlayer(GameObject* player) {
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

void StateGameObject::TestBehaviourTree() {
	float behaviourTimer;
	float distanceToTarget;
	BehaviourAction* patrolArea = new BehaviourAction("Patrol Area", [&](float dt, BehaviourState state)->BehaviourState {
		if (state == Initialise) {
			StateGameObject::MovePatrol(GetGameObject());
			behaviourTimer = rand() % 100;
			state = Ongoing;
		}
		else if (state == Ongoing) {
			behaviourTimer -= dt;
			if (behaviourTimer <= 0.0f) {
				return Success;
			}
		}
	return state;
		});

	BehaviourAction* goBeserk = new BehaviourAction("Go Beserk", [&](float dt, BehaviourState state)->BehaviourState {
		if (state == Initialise) {
			StateGameObject::goBeserk(GetGameObject());
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

	BehaviourAction* chasePlayer = new BehaviourAction("Chase Player", [&](float dt, BehaviourState state)->BehaviourState {
		if (state == Initialise) {
			std::cout << "Chasing the Player\n";
			StateGameObject::ChasePlayer(GetGameObject());
			return Ongoing;
		}
		else if (state == Ongoing) {
			if (chase) {
				return Success;
			}
			return Failure;
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
	sequence->AddChild(patrolArea);
	sequence->AddChild(goBeserk);

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