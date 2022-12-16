#include "StateGameObjectB.h"
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

StateGameObjectB::StateGameObjectB() {
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

	TestBehaviourTree();
}

StateGameObjectB::~StateGameObjectB() {
	delete stateMachine;
}

void StateGameObjectB::Update(float dt) {
	//stateMachine->Update(dt)
	state = rootSequence->Execute(dt);
	if (state == Success) {
		rootSequence->Reset();
	}
}

void StateGameObjectB::MoveLeft(float dt) {
	GetPhysicsObject()->ClearForces();
	GetPhysicsObject()->AddForce({ -10, 0, 0 });
	counter += dt;
}

void StateGameObjectB::drawRay(GameObject* player) {
	Ray isPlayer(this->GetTransform().GetPosition(), (this->GetTransform().GetPosition() - GetGameObject()->GetTransform().GetPosition()).Normalised());
	RayCollision coll;

	if (world->Raycast(isPlayer, coll, true, this)) {
		Debug::DrawLine(isPlayer.GetPosition(), GetGameObject()->GetTransform().GetPosition(), Debug::BLUE);
	}
}


void StateGameObjectB::MovePatrol(GameObject* player) {
	GetPhysicsObject()->ClearForces();
	int i = 0;
	Vector3 moveDir = (posList2.at(instance) + Vector3(195,0,195)) - this->GetTransform().GetPosition();
	this->GetPhysicsObject()->AddForce(Vector3(moveDir.Normalised().x * 2 , 0, moveDir.Normalised().z * 2));
	if ((moveDir*Vector3(1,0,1)).Length() < 1.0f) {
		(instance == posList2.size() - 1 ? instance = 0 : instance++);
	}
	Vector2 Dis, Obj;
	Dis = Vector2(player->GetTransform().GetPosition().x, player->GetTransform().GetPosition().z);
	Obj = Vector2(this->GetTransform().GetPosition().x, this->GetTransform().GetPosition().z);
	float distance = (Dis - Obj).Length();
	if (distance < 20) {
		chase = true;
	}
}

void StateGameObjectB::goBeserk(GameObject* player) {
	GetPhysicsObject()->ClearForces();
	int i = 0;
	Vector3 moveDir = (posList2.at(instance) + Vector3(195, 0, 195)) - this->GetTransform().GetPosition();
	this->GetPhysicsObject()->AddForce(Vector3(moveDir.Normalised().x * 2, 10, moveDir.Normalised().z * 2));
	if ((moveDir * Vector3(1, 0, 1)).Length() < 1.0f) {
		(instance == posList2.size() - 1 ? instance = 0 : instance++);
	}
	Vector2 Dis, Obj;
	Dis = Vector2(player->GetTransform().GetPosition().x, player->GetTransform().GetPosition().z);
	Obj = Vector2(this->GetTransform().GetPosition().x, this->GetTransform().GetPosition().z);
	float distance = (Dis - Obj).Length();
	if (distance < 20) {
		chase = true;
	}
}

void StateGameObjectB::ChasePlayer(GameObject* player) {
	StateGameObjectB::drawRay(player);

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

void StateGameObjectB::TestBehaviourTree() {
	float behaviourTimer = 0.0f;
	BehaviourAction* patrolArea = new BehaviourAction("Patrol Area", [&](float dt, BehaviourState state)->BehaviourState {
		if (state == Initialise) {
			behaviourTimer = rand() % 100;
			state = Ongoing;
		}
		else if (state == Ongoing) {
			behaviourTimer -= dt;
			StateGameObjectB::MovePatrol(GetGameObject());
			if (chase) {
				return Success;
			}
			if (behaviourTimer <= 0.0f) {
				return Success;
			}
		}
	return state;
		});

	BehaviourAction* goBeserk = new BehaviourAction("Go Beserk", [&](float dt, BehaviourState state)->BehaviourState {
		if (state == Initialise) {
			behaviourTimer = rand() % 100;
			state = Ongoing;
		}
		else if (state == Ongoing) {
			behaviourTimer -= dt;
			StateGameObjectB::goBeserk(GetGameObject());
			if (chase) {
				return Success;
			}
			if (behaviourTimer <= 0.0f) {
				return Failure;
			}
		}
	return state;
		});

	BehaviourAction* chasePlayer = new BehaviourAction("Chase Player", [&](float dt, BehaviourState state)->BehaviourState {
		if (state == Initialise) {
			if (!chase) {
				return Failure;
			}
			return Ongoing;
		}
		else if (state == Ongoing) {
			if (!chase) {
				return Failure;
			}
			StateGameObjectB::ChasePlayer(GetGameObject());
		}
	return state;
		});

	BehaviourSequence* sequence = new BehaviourSequence("Room Sequence");
	sequence->AddChild(patrolArea);
	sequence->AddChild(goBeserk);
	sequence->AddChild(chasePlayer);

	StateGameObjectB::rootSequence = new BehaviourSequence("Root Sequence");
	rootSequence->AddChild(sequence);
}