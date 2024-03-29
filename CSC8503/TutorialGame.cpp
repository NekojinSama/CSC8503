#include "TutorialGame.h"
#include "GameWorld.h"
#include "PhysicsObject.h"
#include "RenderObject.h"
#include "TextureLoader.h"

#include "PositionConstraint.h"
#include "OrientationConstraint.h"
#include "StateGameObject.h"
#include "StateGameObjectB.h"
#include "AiPathFollow.h"

#include <fstream>
#include "Assets.h"

using namespace NCL;
using namespace CSC8503;

TutorialGame::TutorialGame()	{
	world		= new GameWorld();
#ifdef USEVULKAN
	renderer	= new GameTechVulkanRenderer(*world);
#else 
	renderer = new GameTechRenderer(*world);
#endif

	physics		= new PhysicsSystem(*world);

	forceMagnitude	= 10.0f;
	useGravity		= true;
	physics->UseGravity(useGravity);
	inSelectionMode = false;

	InitialiseAssets();
}

/*

Each of the little demo scenarios used in the game uses the same 2 meshes, 
and the same texture and shader. There's no need to ever load in anything else
for this module, even in the coursework, but you can add it if you like!

*/
void TutorialGame::InitialiseAssets() {
	cubeMesh	= renderer->LoadMesh("cube.msh");
	coneMesh	= renderer->LoadMesh("cone.msh");
	sphereMesh	= renderer->LoadMesh("sphere.msh");
	charMesh	= renderer->LoadMesh("goat.msh");
	enemyMesh	= renderer->LoadMesh("Keeper.msh");
	gooseMesh	= renderer->LoadMesh("goose.msh");
	bonusMesh	= renderer->LoadMesh("apple.msh");
	capsuleMesh = renderer->LoadMesh("capsule.msh");

	basicTex	= renderer->LoadTexture("checkerboard.png");
	basicShader = renderer->LoadShader("scene.vert", "scene.frag");

	InitCamera();
	InitWorld();
}

TutorialGame::~TutorialGame()	{
	delete cubeMesh;
	delete sphereMesh;
	delete charMesh;
	delete enemyMesh;
	delete bonusMesh;

	delete basicTex;
	delete basicShader;

	delete physics;
	delete renderer;
	delete world;
}

void TutorialGame::UpdateGame(float dt) {
	if (!inSelectionMode) {
		Debug::Print("Press M to change to camera POV!", Vector2(5, 15));
		
		if (POV) { PlayerCamera(dt); }
		else{ world->GetMainCamera()->UpdateCamera(dt); }

		/*if(GameObject* a->GetLayer() = 6){}*/
		/*CollisionDetection::CollisionInfo info;
		if (CollisionDetection::ObjectIntersection(player, apple, info)) {
			world->RemoveGameObject(apple);
		}*/
	}
	if (lockedObject != nullptr) {
		Vector3 objPos = lockedObject->GetTransform().GetPosition();
		Vector3 camPos = objPos + lockedOffset;

		Matrix4 temp = Matrix4::BuildViewMatrix(camPos, objPos, Vector3(0,1,0));

		Matrix4 modelMat = temp.Inverse();

		Quaternion q(modelMat);
		Vector3 angles = q.ToEuler(); //nearly there now!

		world->GetMainCamera()->SetPosition(camPos);
		world->GetMainCamera()->SetPitch(angles.x);
		world->GetMainCamera()->SetYaw(angles.y);
	}

	if (testStateObject) {
		testStateObject -> Update(dt);
	}

	if (testStateObjectB) {
		testStateObjectB->Update(dt);
	}

	if (pathObject) {
		pathObject->Update(dt);
	}

	UpdateKeys();
	//StateGameObject::TestBehaviourTree();

	if (useGravity) {
		
		Debug::Print("(G)ravity on", Vector2(5, 95), Debug::RED);
	}
	else {
		Debug::Print("(G)ravity off", Vector2(5, 95), Debug::RED);
	}

	RayCollision closestCollision;
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::K) && selectionObject) {
		Vector3 rayPos;
		Vector3 rayDir;

		rayDir = selectionObject->GetTransform().GetOrientation() * Vector3(0, 0, -1);

		rayPos = selectionObject->GetTransform().GetPosition();

		Ray r = Ray(rayPos, rayDir);

		if (world->Raycast(r, closestCollision, true, selectionObject)) {
			if (objClosest) {
				objClosest->GetRenderObject()->SetColour(Vector4(1, 1, 1, 1));
				Debug::DrawLine(rayPos, objClosest->GetTransform().GetPosition(), Vector4(1, 0, 0, 1), 10.0);
			}
			objClosest = (GameObject*)closestCollision.node;

			objClosest->GetRenderObject()->SetColour(Vector4(1, 0, 1, 1));
		}
	}

	Debug::DrawLine(Vector3(), Vector3(0, 100, 0), Vector4(1, 0, 0, 1));
	Debug::DrawLine(Vector3(50,0,0), Vector3(50, 100, 0), Vector4(1, 0, 0, 1));
	//Debug::DrawLine(Vector3(), Vector3(0, 100, 0), Vector4(1, 0, 0, 1));
	//Debug::DrawLine(Vector3(), Vector3(0, 100, 0), Vector4(1, 0, 0, 1));

	SelectObject();
	MoveSelectedObject();

	//world->GetObjectIterators();

	world->UpdateWorld(dt);
	renderer->Update(dt);
	physics->Update(dt);
	

	renderer->Render();
	Debug::UpdateRenderables(dt);
}

Vector3 TutorialGame::PlayerMovementPace(float dt, Vector3 dir) {
	float maxSpeed = 10.0f;
	float maxAcceleration = 10.0f;

	if (Window::GetKeyboard()->KeyDown(NCL::KeyboardKeys::SHIFT)) {
		maxSpeed = 30.0f;
	}

	Vector3 velocity = dir * maxSpeed;
	Vector3 dVelocity = dir * maxSpeed;

	float maxSpeedChange = maxAcceleration * dt;
	if (velocity.x < dVelocity.x) {
		velocity.x = min(velocity.x + maxSpeedChange, dVelocity.x);
	}
	else if (velocity.x > dVelocity.x) {
		velocity.x = max(velocity.x - maxSpeedChange, dVelocity.x);
	}
	if (velocity.z < dVelocity.z) {
		velocity.z = min(velocity.z + maxSpeedChange, dVelocity.z);
	}
	else if (velocity.z > dVelocity.z) {
		velocity.z = max(velocity.z - maxSpeedChange, dVelocity.z);
	}
	Vector3 displacement = velocity * dt;
	return displacement;
}

void TutorialGame::PlayerMovement(float dt) {
	bool keyPressed = Window::GetKeyboard()->KeyDown(NCL::KeyboardKeys::W) || Window::GetKeyboard()->KeyDown(NCL::KeyboardKeys::S) || Window::GetKeyboard()->KeyDown(NCL::KeyboardKeys::A) || Window::GetKeyboard()->KeyDown(NCL::KeyboardKeys::D);
	Ray isGround(player->GetTransform().GetPosition(), Vector3(0, -1, 0));
	RayCollision coll;
	if (world->Raycast(isGround, coll, true, player)) {
		onGround = (coll.rayDistance < 1.5f);
	}

	player->GetRenderObject()->SetColour(onGround ? Debug::BLUE : Debug::RED);

	if (Window::GetKeyboard()->KeyDown(NCL::KeyboardKeys::W)) {
		/*Vector3 newPos = player->GetTransform().GetPosition() + TutorialGame::PlayerMovementPace(dt, player->GetTransform().GetOrientation() * Vector3(0, 0, -1));
		player->GetTransform().SetPosition(newPos);*/

		player->GetPhysicsObject()->AddForce(TutorialGame::PlayerMovementPace(dt, player->GetTransform().GetOrientation() * Vector3(0, 0, -1)) * 20);	
	}
	
	if (Window::GetKeyboard()->KeyDown(NCL::KeyboardKeys::S)) {
		player->GetPhysicsObject()->AddForce(TutorialGame::PlayerMovementPace(dt, player->GetTransform().GetOrientation() * Vector3(0, 0, 1)) * 20);
	}
	if (Window::GetKeyboard()->KeyDown(NCL::KeyboardKeys::A)) {
		player->GetPhysicsObject()->AddForce(TutorialGame::PlayerMovementPace(dt, player->GetTransform().GetOrientation() * Vector3(-1, 0, 0)) * 20);
	}
	if (Window::GetKeyboard()->KeyDown(NCL::KeyboardKeys::D)) {
		player->GetPhysicsObject()->AddForce(TutorialGame::PlayerMovementPace(dt, player->GetTransform().GetOrientation() * Vector3(1, 0, 0)) * 20);
	}
	if (Window::GetKeyboard()->KeyDown(NCL::KeyboardKeys::SPACE) && onGround) {
		/*Vector3 newPos = player->GetTransform().GetPosition() + TutorialGame::PlayerMovementPace(dt, player->GetTransform().GetOrientation() * Vector3(0, 2, 0));
		player->GetTransform().SetPosition(newPos);*/
		player->GetPhysicsObject()->AddForce(Vector3(0, 1, 0) * 125.0f);
		onGround = false;
	}

	if (player->GetPhysicsObject()->GetLinearVelocity().Length() > 0.1f && onGround && !keyPressed) {
		Vector3 dampVel = Vector3::Lerp(player->GetPhysicsObject()->GetLinearVelocity(), Vector3(0, 0, 0), 10.0f * dt);
		player->GetPhysicsObject()->SetLinearVelocity(dampVel);
	}
}

void TutorialGame::PlayerCamera(float dt) {
	TutorialGame::PlayerMovement(dt);

	TutorialGame::pitch -= (Window::GetMouse()->GetRelativePosition().y);
	TutorialGame::yaw -= (Window::GetMouse()->GetRelativePosition().x);

	pitch = min(pitch, 90.0f);
	pitch = max(pitch, -90.0f);

	if (yaw < 0) {
		yaw += 360.0f;
	}
	if (yaw > 360.0f) {
		yaw -= 360.0f;
	}

	float radius = 10.0f;

	Matrix4 temp = Matrix4::Rotation(yaw, Vector3(0, 1, 0));
	Quaternion q(temp);
	player->GetTransform().SetOrientation(q);
	float x = player->GetTransform().GetPosition().x + radius * sin(yaw * 3.14f / 180);
	float z = player->GetTransform().GetPosition().z + radius * cos(yaw * 3.14f / 180);
	float y = player->GetTransform().GetPosition().y - radius * sin(pitch * 3.14f / 180);

	world->GetMainCamera()->SetPosition(Vector3(x,y,z));
	world->GetMainCamera()->SetPitch(pitch);
	world->GetMainCamera()->SetYaw(yaw);
}

void TutorialGame::UpdateKeys() {
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F1)) {
		InitWorld(); //We can reset the simulation at any time with F1
		selectionObject = nullptr;
	}

	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F2)) {
		InitCamera(); //F2 will reset the camera to a specific default place
	}

	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::G)) {
		useGravity = !useGravity; //Toggle gravity!
		physics->UseGravity(useGravity);
	}
	//Running certain physics updates in a consistent order might cause some
	//bias in the calculations - the same objects might keep 'winning' the constraint
	//allowing the other one to stretch too much etc. Shuffling the order so that it
	//is random every frame can help reduce such bias.
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F9)) {
		world->ShuffleConstraints(true);
	}
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F10)) {
		world->ShuffleConstraints(false);
	}

	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F7)) {
		world->ShuffleObjects(true);
	}
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F8)) {
		world->ShuffleObjects(false);
	}
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::M)) {
		TogglePOV();
	}

	if (lockedObject) {
		LockedObjectMovement();
	}
	else {
		DebugObjectMovement();
	}
}

void TutorialGame::TogglePOV() {
	POV = !POV;
}



void TutorialGame::LockedObjectMovement() {
	Matrix4 view		= world->GetMainCamera()->BuildViewMatrix();
	Matrix4 camWorld	= view.Inverse();

	Vector3 rightAxis = Vector3(camWorld.GetColumn(0)); //view is inverse of model!

	//forward is more tricky -  camera forward is 'into' the screen...
	//so we can take a guess, and use the cross of straight up, and
	//the right axis, to hopefully get a vector that's good enough!

	Vector3 fwdAxis = Vector3::Cross(Vector3(0, 1, 0), rightAxis);
	fwdAxis.y = 0.0f;
	fwdAxis.Normalise();


	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::UP)) {
		selectionObject->GetPhysicsObject()->AddForce(fwdAxis);
	}

	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::DOWN)) {
		selectionObject->GetPhysicsObject()->AddForce(-fwdAxis);
	}

	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::NEXT)) {
		selectionObject->GetPhysicsObject()->AddForce(Vector3(0,-10,0));
	}
}

void TutorialGame::DebugObjectMovement() {
//If we've selected an object, we can manipulate it with some key presses
	if (inSelectionMode && selectionObject) {
		//Twist the selected object!
		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::LEFT)) {
			selectionObject->GetPhysicsObject()->AddTorque(Vector3(-10, 0, 0));
		}

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::RIGHT)) {
			selectionObject->GetPhysicsObject()->AddTorque(Vector3(10, 0, 0));
		}

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::NUM7)) {
			selectionObject->GetPhysicsObject()->AddTorque(Vector3(0, 10, 0));
		}

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::NUM8)) {
			selectionObject->GetPhysicsObject()->AddTorque(Vector3(0, -10, 0));
		}

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::RIGHT)) {
			selectionObject->GetPhysicsObject()->AddTorque(Vector3(10, 0, 0));
		}

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::UP)) {
			selectionObject->GetPhysicsObject()->AddForce(Vector3(0, 0, -10));
		}

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::DOWN)) {
			selectionObject->GetPhysicsObject()->AddForce(Vector3(0, 0, 10));
		}

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::NUM5)) {
			selectionObject->GetPhysicsObject()->AddForce(Vector3(0, -10, 0));
		}
	}
}

void TutorialGame::InitCamera() {
	world->GetMainCamera()->SetNearPlane(0.1f);
	world->GetMainCamera()->SetFarPlane(500.0f);
	world->GetMainCamera()->SetPitch(-15.0f);
	world->GetMainCamera()->SetYaw(315.0f);
	world->GetMainCamera()->SetPosition(Vector3(-60, 40, 60));
	lockedObject = nullptr;
}

void TutorialGame::InitWorld() {
	world->ClearAndErase();
	physics->Clear();

	//InitMixedGridWorld(15, 15, 3.5f, 3.5f);

	InitGameExamples();
	InitDefaultFloor();
	BridgeConstraintTest();
	testStateObject = AddStateObjectToWorld(Vector3(50, 0, -140) + offset, 1.0f, 1);
	testStateObjectB = AddStateWorld(Vector3(-100, 0, -140) + offset, 1.0f, 1);
	pathObject = AddPathToWorld(Vector3(185, 0, 35) + offset, 3.0f, 1);
}

/*

A single function to add a large immoveable cube to the bottom of our world

*/
GameObject* TutorialGame::AddFloorToWorld(const Vector3& position) {
	GameObject* floor = new GameObject("Floor");
	floor->SetLayer(3);
	Vector3 floorSize = Vector3(200, 2, 200);
	AABBVolume* volume = new AABBVolume(floorSize);
	floor->SetBoundingVolume((CollisionVolume*)volume);
	floor->GetTransform()
		.SetScale(floorSize * 2)
		.SetPosition(position);

	floor->SetRenderObject(new RenderObject(&floor->GetTransform(), cubeMesh, basicTex, basicShader));
	floor->SetPhysicsObject(new PhysicsObject(&floor->GetTransform(), floor->GetBoundingVolume()));

	floor->GetPhysicsObject()->SetInverseMass(0);
	floor->GetPhysicsObject()->SetElasticity(0.1f);
	floor->GetPhysicsObject()->InitCubeInertia();

	world->AddGameObject(floor);

	return floor;
}

/*

Builds a game object that uses a sphere mesh for its graphics, and a bounding sphere for its
rigid body representation. This and the cube function will let you build a lot of 'simple' 
physics worlds. You'll probably need another function for the creation of OBB cubes too.

*/
GameObject* TutorialGame::AddSphereToWorld(const Vector3& position, float radius, float inverseMass) {
	GameObject* sphere = new GameObject("sphere");
	Vector3 sphereSize = Vector3(radius, radius, radius);
	SphereVolume* volume = new SphereVolume(radius);
	sphere->SetBoundingVolume((CollisionVolume*)volume);

	sphere->GetTransform()
		.SetScale(sphereSize)
		.SetPosition(position);

	sphere->SetLayer(3);

	sphere->SetRenderObject(new RenderObject(&sphere->GetTransform(), sphereMesh, basicTex, basicShader));
	sphere->SetPhysicsObject(new PhysicsObject(&sphere->GetTransform(), sphere->GetBoundingVolume()));

	sphere->GetPhysicsObject()->SetInverseMass(inverseMass);
	sphere->GetPhysicsObject()->SetElasticity(1.0f);
	sphere->GetPhysicsObject()->InitSphereInertia();

	world->AddGameObject(sphere);

	return sphere;
}

GameObject* TutorialGame::AddCubeToWorld(const Vector3& position, Vector3 dimensions, float inverseMass) {
	GameObject* cube = new GameObject("cube");

	AABBVolume* volume = new AABBVolume(dimensions);
	cube->SetBoundingVolume((CollisionVolume*)volume);

	cube->GetTransform()
		.SetPosition(position)
		.SetScale(Vector3(dimensions.x * 2,dimensions.y * 2, dimensions.z * 2));

	cube->SetLayer(3);

	cube->SetRenderObject(new RenderObject(&cube->GetTransform(), cubeMesh, basicTex, basicShader));
	cube->SetPhysicsObject(new PhysicsObject(&cube->GetTransform(), cube->GetBoundingVolume()));

	cube->GetPhysicsObject()->SetInverseMass(inverseMass);
	cube->GetPhysicsObject()->SetElasticity(0.4f);
	cube->GetPhysicsObject()->InitCubeInertia();

	world->AddGameObject(cube);

	return cube;
}

GameObject* TutorialGame::AddDoorPull(string objName, const Vector3& position, Vector3 dimensions, float inverseMass) {
	GameObject* cube = new GameObject(objName);

	AABBVolume* volume = new AABBVolume(dimensions);
	cube->SetBoundingVolume((CollisionVolume*)volume);

	cube->GetTransform()
		.SetPosition(position)
		.SetScale(Vector3(dimensions.x * 2,dimensions.y * 2, dimensions.z * 2));

	cube->SetLayer(8);

	cube->SetRenderObject(new RenderObject(&cube->GetTransform(), cubeMesh, basicTex, basicShader));
	cube->SetPhysicsObject(new PhysicsObject(&cube->GetTransform(), cube->GetBoundingVolume()));

	cube->GetPhysicsObject()->SetInverseMass(inverseMass);
	cube->GetPhysicsObject()->SetElasticity(0.4f);
	cube->GetPhysicsObject()->InitCubeInertia();

	world->AddGameObject(cube);

	return cube;
}

GameObject* TutorialGame::AddPlayerToWorld(const Vector3& position) {
	float meshSize		= 1.0f;
	float inverseMass	= 0.9f;

	player = new GameObject("Player");
	SphereVolume* volume  = new SphereVolume(1.0f);

	player->SetLayer(4);

	player->SetBoundingVolume((CollisionVolume*)volume);

	player->GetTransform()
		.SetScale(Vector3(meshSize, meshSize, meshSize))
		.SetPosition(position);

	player->SetRenderObject(new RenderObject(&player->GetTransform(), charMesh, nullptr, basicShader));
	player->SetPhysicsObject(new PhysicsObject(&player->GetTransform(), player->GetBoundingVolume()));

	player->GetPhysicsObject()->SetInverseMass(inverseMass);
	player->GetPhysicsObject()->InitSphereInertia();

	world->AddGameObject(player);

	return player;
}

GameObject* TutorialGame::AddEnemyToWorld(const Vector3& position) {
	float meshSize		= 3.0f;
	float inverseMass	= 0.5f;

	GameObject* character = new GameObject("Enemy");
	character->SetLayer(5);

	AABBVolume* volume = new AABBVolume(Vector3(0.3f, 0.9f, 0.3f) * meshSize);
	character->SetBoundingVolume((CollisionVolume*)volume);

	character->GetTransform()
		.SetScale(Vector3(meshSize, meshSize, meshSize))
		.SetPosition(position);

	character->SetRenderObject(new RenderObject(&character->GetTransform(), enemyMesh, nullptr, basicShader));
	character->SetPhysicsObject(new PhysicsObject(&character->GetTransform(), character->GetBoundingVolume()));

	character->GetPhysicsObject()->SetInverseMass(inverseMass);
	character->GetPhysicsObject()->InitSphereInertia();

	world->AddGameObject(character);

	return character;
}

BonusInteract* TutorialGame::AddBonusCircle(const Vector3& position) {
	bonusApple = new BonusInteract(world, this);
	bonusApple->SetLayer(6);
	SphereVolume* volume = new SphereVolume(2.0f);
	bonusApple->SetBoundingVolume((CollisionVolume*)volume);
	bonusApple->GetTransform()
		.SetScale(Vector3(2, 2, 2))
		.SetPosition(position);

	bonusApple->SetRenderObject(new RenderObject(&bonusApple->GetTransform(), sphereMesh, basicTex, basicShader));
	bonusApple->SetPhysicsObject(new PhysicsObject(&bonusApple->GetTransform(), bonusApple->GetBoundingVolume()));

	bonusApple->GetPhysicsObject()->SetInverseMass(1.0f);
	bonusApple->GetPhysicsObject()->InitSphereInertia();
	bonusApple->SetHealth(3);

	world->AddGameObject(bonusApple);

	return bonusApple;
}

BonusInteract* TutorialGame::AddBonusCube(const Vector3& position, Vector3 dimensions, float inverseMass) {
	bonusApple = new BonusInteract(world, this);
	bonusApple->SetLayer(6);
	AABBVolume* volume = new AABBVolume(dimensions);
	bonusApple->SetBoundingVolume((CollisionVolume*)volume);
	bonusApple->GetTransform()
		.SetPosition(position)
		.SetScale(Vector3(dimensions.x * 2, dimensions.y * 2, dimensions.z * 2));

	bonusApple->SetRenderObject(new RenderObject(&bonusApple->GetTransform(), cubeMesh, basicTex, basicShader));
	bonusApple->SetPhysicsObject(new PhysicsObject(&bonusApple->GetTransform(), bonusApple->GetBoundingVolume()));

	bonusApple->GetPhysicsObject()->SetInverseMass(1.0f);
	bonusApple->GetPhysicsObject()->InitCubeInertia();
	bonusApple->SetHealth(10);

	world->AddGameObject(bonusApple);

	return bonusApple;
}

BonusInteract* TutorialGame::AddRemovableWall(string objName, const Vector3& position, Vector3 dimensions, float inverseMass, bool status) {
	bonusApple = new BonusInteract(world, this);
	bonusApple->SetLayer(7);
	AABBVolume* volume = new AABBVolume(dimensions);
	bonusApple->SetBoundingVolume((CollisionVolume*)volume);
	/*this->IsActive = status;*/
	bonusApple->GetTransform()
		.SetPosition(position)
		.SetScale(Vector3(dimensions.x * 2, dimensions.y * 2, dimensions.z * 2));

	bonusApple->SetRenderObject(new RenderObject(&bonusApple->GetTransform(), cubeMesh, basicTex, basicShader));
	bonusApple->SetPhysicsObject(new PhysicsObject(&bonusApple->GetTransform(), bonusApple->GetBoundingVolume()));

	bonusApple->GetPhysicsObject()->SetInverseMass(inverseMass);
	bonusApple->GetPhysicsObject()->InitCubeInertia();
	bonusApple->SetHealth(2000);

	world->AddGameObject(bonusApple);

	return bonusApple;
}

BonusInteract* TutorialGame::AddSwitchButton(const Vector3& position, Vector3 dimensions, float inverseMass) {
	bonusApple = new BonusInteract(world, this);
	bonusApple->SetLayer(8);
	AABBVolume* volume = new AABBVolume(dimensions);
	bonusApple->SetBoundingVolume((CollisionVolume*)volume);
	bonusApple->GetTransform()
		.SetPosition(position)
		.SetScale(Vector3(dimensions.x * 2, dimensions.y * 2, dimensions.z * 2));

	bonusApple->SetRenderObject(new RenderObject(&bonusApple->GetTransform(), cubeMesh, basicTex, basicShader));
	bonusApple->GetRenderObject()->SetColour(Debug::RED);
	bonusApple->SetPhysicsObject(new PhysicsObject(&bonusApple->GetTransform(), bonusApple->GetBoundingVolume()));

	bonusApple->GetPhysicsObject()->SetInverseMass(inverseMass);
	bonusApple->GetPhysicsObject()->InitCubeInertia();
	bonusApple->SetHealth(2000);

	world->AddGameObject(bonusApple);

	return bonusApple;
}

BonusInteract* TutorialGame::AddBonusCapsule(const Vector3& position, Vector3 dimensions, float inverseMass) {
	bonusApple = new BonusInteract(world, this);
	bonusApple->SetLayer(6);
	AABBVolume* volume = new AABBVolume(dimensions);
	bonusApple->SetBoundingVolume((CollisionVolume*)volume);
	bonusApple->GetTransform()
		.SetPosition(position)
		.SetScale(Vector3(dimensions.x * 2, dimensions.y * 2, dimensions.z * 2));

	bonusApple->SetRenderObject(new RenderObject(&bonusApple->GetTransform(), capsuleMesh, basicTex, basicShader));
	bonusApple->SetPhysicsObject(new PhysicsObject(&bonusApple->GetTransform(), bonusApple->GetBoundingVolume()));

	bonusApple->GetPhysicsObject()->SetInverseMass(1.0f);
	bonusApple->GetPhysicsObject()->InitCubeInertia();

	world->AddGameObject(bonusApple);

	return bonusApple;
}

BonusInteract* TutorialGame::AddBonusCone(const Vector3& position) {
	bonusApple = new BonusInteract(world, this);
	bonusApple->SetLayer(6);
	SphereVolume* volume = new SphereVolume(2.5f);
	bonusApple->SetBoundingVolume((CollisionVolume*)volume);
	bonusApple->GetTransform()
		.SetScale(Vector3(2, 2, 2))
		.SetPosition(position);

	bonusApple->SetRenderObject(new RenderObject(&bonusApple->GetTransform(), coneMesh, basicTex, basicShader));
	bonusApple->SetPhysicsObject(new PhysicsObject(&bonusApple->GetTransform(), bonusApple->GetBoundingVolume()));

	bonusApple->GetPhysicsObject()->SetInverseMass(1.0f);
	bonusApple->GetPhysicsObject()->InitSphereInertia();

	world->AddGameObject(bonusApple);

	return bonusApple;
}

GameObject* TutorialGame::AddBonusToWorld(const Vector3& position) {
	TutorialGame::apple = new GameObject();
	apple->SetLayer(6);
	SphereVolume* volume = new SphereVolume(0.5f);
	apple->SetBoundingVolume((CollisionVolume*)volume);
	apple->GetTransform()
		.SetScale(Vector3(2, 2, 2))
		.SetPosition(position);

	apple->SetRenderObject(new RenderObject(&apple->GetTransform(), cubeMesh, basicTex, basicShader));
	apple->SetPhysicsObject(new PhysicsObject(&apple->GetTransform(), apple->GetBoundingVolume()));

	apple->GetPhysicsObject()->SetInverseMass(1.0f);
	apple->GetPhysicsObject()->InitSphereInertia();

	world->AddGameObject(apple);

	return apple;
}

StateGameObject* TutorialGame::AddStateObjectToWorld(const Vector3& position, float radius, float inverseMass) {
	StateGameObject* sphere = new StateGameObject();

	Vector3 sphereSize = Vector3(radius, radius, radius);
	SphereVolume* volume = new SphereVolume(radius);
	sphere->SetBoundingVolume((CollisionVolume*)volume);

	sphere->GetTransform()
		.SetScale(sphereSize)
		.SetPosition(position);

	sphere->SetLayer(5);

	sphere->SetRenderObject(new RenderObject(&sphere->GetTransform(), sphereMesh, basicTex, basicShader));
	sphere->SetPhysicsObject(new PhysicsObject(&sphere->GetTransform(), sphere->GetBoundingVolume()));

	sphere->GetPhysicsObject()->SetInverseMass(inverseMass);
	sphere->GetPhysicsObject()->SetElasticity(1.0f);
	sphere->GetPhysicsObject()->InitSphereInertia();
	sphere->SetWorld(world);
	world->AddGameObject(sphere);
	sphere->SetGameObject(player); 

	return sphere;
}

StateGameObjectB* TutorialGame::AddStateWorld(const Vector3& position, float radius, float inverseMass) {
	StateGameObjectB* sphere = new StateGameObjectB();

	Vector3 sphereSize = Vector3(radius, radius, radius);
	SphereVolume* volume = new SphereVolume(radius);
	sphere->SetBoundingVolume((CollisionVolume*)volume);

	sphere->GetTransform()
		.SetScale(sphereSize)
		.SetPosition(position);

	sphere->SetLayer(5);

	sphere->SetRenderObject(new RenderObject(&sphere->GetTransform(), sphereMesh, basicTex, basicShader));
	sphere->SetPhysicsObject(new PhysicsObject(&sphere->GetTransform(), sphere->GetBoundingVolume()));

	sphere->GetPhysicsObject()->SetInverseMass(inverseMass);
	sphere->GetPhysicsObject()->SetElasticity(1.0f);
	sphere->GetPhysicsObject()->InitSphereInertia();
	sphere->SetWorld(world);
	world->AddGameObject(sphere);
	sphere->SetGameObject(player);

	return sphere;
}

AiPathFollow* TutorialGame::AddPathToWorld(const Vector3& position, float radius, float inverseMass) {
	AiPathFollow* sphere = new AiPathFollow();

	Vector3 sphereSize = Vector3(radius, radius, radius);
	SphereVolume* volume = new SphereVolume(radius);
	sphere->SetBoundingVolume((CollisionVolume*)volume);

	sphere->GetTransform()
		.SetScale(sphereSize)
		.SetPosition(position);

	sphere->SetLayer(5);

	sphere->SetRenderObject(new RenderObject(&sphere->GetTransform(), gooseMesh, nullptr, basicShader));
	sphere->SetPhysicsObject(new PhysicsObject(&sphere->GetTransform(), sphere->GetBoundingVolume()));

	sphere->GetPhysicsObject()->SetInverseMass(inverseMass);
	sphere->GetPhysicsObject()->SetElasticity(1.0f);
	sphere->GetPhysicsObject()->InitSphereInertia();

	world->AddGameObject(sphere);

	return sphere;
}

void TutorialGame::InitDefaultFloor() {
	AddFloorToWorld(Vector3(0, -20+13, 0) + offset);
}

void TutorialGame::InitGameExamples() {
	AddPlayerToWorld(Vector3(70, 0, -70)+ offset);
	AddEnemyToWorld(Vector3(5, -5, 0) + Vector3(195, 0, 195) + offset);
	InitMazeGrid("TestGrid2.txt");
	AddBonusCircle(Vector3(40, 10, -40) + offset);
	AddBonusCube(Vector3(20, 5, -40) + offset, Vector3(2, 2, 2), 0.0f);
	AddBonusCube(Vector3(20, 5, -20) + offset, Vector3(2, 2, 2), 0.0f);
	AddBonusCube(Vector3(40, 10, -20) + offset, Vector3(2, 2, 2), 0.0f);
	//AddBonusCone(Vector3(40, 10, -20));
	//AddBonusCapsule(Vector3(20, 5, -20), Vector3(1, 2, 1), 0.0f);
	//AddAppleToWorld(Vector3(70, 0, 80));
}

void TutorialGame::InitSphereGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing, float radius) {
	for (int x = 0; x < numCols; ++x) {
		for (int z = 0; z < numRows; ++z) {
			Vector3 position = Vector3(x * colSpacing, 10.0f, z * rowSpacing);
			AddSphereToWorld(position, radius, 1.0f);
		}
	}
	AddFloorToWorld(Vector3(0, -2, 0));
}

void TutorialGame::InitMixedGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing) {
	float sphereRadius = 1.0f;
	Vector3 cubeDims = Vector3(1, 1, 1);

	for (int x = 0; x < numCols; ++x) {
		for (int z = 0; z < numRows; ++z) {
			Vector3 position = Vector3(x * colSpacing, 10.0f, z * rowSpacing);

			if (rand() % 2) {
				AddCubeToWorld(position, cubeDims);
			}
			else {
				AddSphereToWorld(position, sphereRadius);
			}
		}
	}
}

void TutorialGame::InitCubeGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing, const Vector3& cubeDims) {
	for (int x = 1; x < numCols+1; ++x) {
		for (int z = 1; z < numRows+1; ++z) {
			Vector3 position = Vector3(x * colSpacing, 10.0f, z * rowSpacing);
			AddCubeToWorld(position, cubeDims, 1.0f);
		}
	}
}

void TutorialGame::InitMazeGrid(const std::string& filename) {
	int nodeSize, gridWidth, gridHeight;
	std::ifstream infile(Assets::DATADIR  + filename);

	infile >> nodeSize;
	infile >> gridWidth;
	infile >> gridHeight;

	for (int y = 0; y < gridHeight; ++y) {
		for (int x = 0; x < gridWidth; ++x) {
			char type = 0;
			infile >> type;
			Vector3 position = Vector3((float)(x * nodeSize) , 0, (float)(y * nodeSize));
			if (type == 'x') {
				AddCubeToWorld(position, Vector3(nodeSize / 2, nodeSize / 2, nodeSize / 2), 0.0f);
			}
			if (type == 'm') {
				AddDoorPull("pull1", Vector3(position.x, 35, position.z), Vector3(nodeSize / 2, nodeSize / 2, nodeSize / 2), 0.0f);
				AddRemovableWall("wall1", Vector3(position.x, 5, position.z), Vector3(nodeSize / 2, nodeSize / 2, nodeSize / 2), 0.0001f, true);
			}
			if (type == 'l') {
				AddSwitchButton(Vector3(position.x,-5,position.z), Vector3(nodeSize / 2, nodeSize / 4, nodeSize / 2), 0.0f);
			}
		}
	}
}

void TutorialGame::BridgeConstraintTest() {
	Vector3 cubeSize = Vector3(2, 2, 2);

	float invCubeMass = 5;
	int numLinks = 10;
	float maxDistance = 10;
	float cubeDistance = 20;

	Vector3 startPos = Vector3(10, 30, 30);
	GameObject* start = AddCubeToWorld(startPos + Vector3(0, 0, 0), cubeSize, 0);
	GameObject* star = AddCubeToWorld(startPos + Vector3(0, -5, 0), cubeSize, 0.001);
	/*GameObject* end = AddCubeToWorld(startPos + Vector3((numLinks+2) * cubeDistance, 0, 0), cubeSize, 0);
	GameObject* previous = start;

	for (int i = 0; i < numLinks; ++i) {
		GameObject* block = AddCubeToWorld(startPos + Vector3((i + 1) * cubeDistance, 0, 0), cubeSize, invCubeMass);
		PositionConstraint* constraint = new PositionConstraint(previous, block, maxDistance);
		world->AddConstraint(constraint);
		previous = block;
	}*/
	PositionConstraint* constraint = new PositionConstraint(start, star, maxDistance);
	world->AddConstraint(constraint);
	/*for (GameObject* g : world->GetObjectIterators(g,f)) {
		f(g);
	}*/
}

/*
Every frame, this code will let you perform a raycast, to see if there's an object
underneath the cursor, and if so 'select it' into a pointer, so that it can be 
manipulated later. Pressing Q will let you toggle between this behaviour and instead
letting you move the camera around. 

*/
bool TutorialGame::SelectObject() {
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::Q)) {
		inSelectionMode = !inSelectionMode;
		if (inSelectionMode) {
			Window::GetWindow()->ShowOSPointer(true);
			Window::GetWindow()->LockMouseToWindow(false);
		}
		else {
			Window::GetWindow()->ShowOSPointer(false);
			Window::GetWindow()->LockMouseToWindow(true);
		}
	}
	if (inSelectionMode) {
		Debug::Print("Press Q to change to camera mode!", Vector2(5, 85));

		if (Window::GetMouse()->ButtonDown(NCL::MouseButtons::LEFT)) {
			if (selectionObject) {	//set colour to deselected;
				selectionObject->GetRenderObject()->SetColour(Vector4(1, 1, 1, 1));
				selectionObject = nullptr;
			}

			Ray ray = CollisionDetection::BuildRayFromMouse(*world->GetMainCamera());

			RayCollision closestCollision;
			if (world->Raycast(ray, closestCollision, true)) {
				selectionObject = (GameObject*)closestCollision.node;

				selectionObject->GetRenderObject()->SetColour(Vector4(0, 1, 0, 1));
				return true;
			}
			else {
				return false;
			}
		}
		if (Window::GetKeyboard()->KeyPressed(NCL::KeyboardKeys::L)) {
			if (selectionObject) {
				if (lockedObject == selectionObject) {
					lockedObject = nullptr;
				}
				else {
					lockedObject = selectionObject;
					world->GetMainCamera()->SetPosition(selectionObject->GetTransform().GetPosition() + Vector3(-10, 0, 0));
				}
			}
		}
	}
	else {
		Debug::Print("Press Q to change to select mode!", Vector2(5, 85));
	}
	return false;
}

/*
If an object has been clicked, it can be pushed with the right mouse button, by an amount
determined by the scroll wheel. In the first tutorial this won't do anything, as we haven't
added linear motion into our physics system. After the second tutorial, objects will move in a straight
line - after the third, they'll be able to twist under torque aswell.
*/

void TutorialGame::MoveSelectedObject() {
	Debug::Print("Click Force:" + std::to_string(forceMagnitude), Vector2(5, 90));
	forceMagnitude += Window::GetMouse()->GetWheelMovement() * 100.0f;

	Debug::Print("Damp Force:" + std::to_string(linearDamping), Vector2(5, 80));
	linearDamping += Window::GetKeyboard()->KeyPressed(NCL::KeyboardKeys::NUMPAD7) * 0.1f;
	linearDamping -= Window::GetKeyboard()->KeyPressed(NCL::KeyboardKeys::NUMPAD4) * 0.1f;
	physics->SetDampForce(linearDamping);

	if (!selectionObject) {
		return;//we haven't selected anything!
	}
	//Push the selected object!
	if (Window::GetMouse()->ButtonPressed(NCL::MouseButtons::RIGHT)) {
		Ray ray = CollisionDetection::BuildRayFromMouse(*world->GetMainCamera());

		RayCollision closestCollision;
		if (world->Raycast(ray, closestCollision, true)) {
			if (closestCollision.node == selectionObject) {
				selectionObject->GetPhysicsObject()->AddForceAtPosition(ray.GetDirection() * forceMagnitude, closestCollision.collidedAt);
			}
		}
	}

	if (Window::GetKeyboard()->KeyDown(NCL::KeyboardKeys::W)) {
		if (selectionObject) {
			selectionObject->GetTransform().SetPosition(selectionObject->GetTransform().GetPosition() + Vector3(0.4, 0, 0));
		}
	}
	if (Window::GetKeyboard()->KeyDown(NCL::KeyboardKeys::S)) {
		if (selectionObject) {
			selectionObject->GetTransform().SetPosition(selectionObject->GetTransform().GetPosition() + Vector3(-0.4, 0, 0));
		}
	}
	if (Window::GetKeyboard()->KeyDown(NCL::KeyboardKeys::A)) {
		if (selectionObject) {
			selectionObject->GetTransform().SetPosition(selectionObject->GetTransform().GetPosition() + Vector3(0, 0, -0.4));
		}
	}
	if (Window::GetKeyboard()->KeyDown(NCL::KeyboardKeys::D)) {
		if (selectionObject) {
			selectionObject->GetTransform().SetPosition(selectionObject->GetTransform().GetPosition() + Vector3(0, 0, 0.4));
		}
	}


}

