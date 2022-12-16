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

void StateGameObject::drawRay(GameObject* player) {
	Ray isPlayer(this->GetTransform().GetPosition(), (this->GetTransform().GetPosition() - GetGameObject()->GetTransform().GetPosition()).Normalised());
	RayCollision coll;

	if (world->Raycast(isPlayer, coll, true, this)) {
		Debug::DrawLine(isPlayer.GetPosition(), GetGameObject()->GetTransform().GetPosition(), Debug::BLUE);
	}
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
	StateGameObject::drawRay(player);

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