# Goat Game | Game Engine
This project uses a physics engine built from scratch and some AI features. The project was developed as part of the university project which helped me learn how the game engine works.

This project aims to understand the aspects of a game engine and put it to use. However, it should be noted that the project is not a game but my understanding of how certain features in the game work.

## Features Implemented:

### Physics:
  * [Raycasting](CSC8503CoreClasses/Ray.h)
  * [Collision Detection](CSC8503CoreClasses/CollisionDetection.cpp)
  * [Layer Masking](CSC8503CoreClasses/GameObject.cpp)
  * [Application of force](CSC8503CoreClasses/PhysicsSystem.cpp)
  * [Constraints](CSC8503CoreClasses/PositionConstraint.cpp)
----------------
### AI
  * [State Machine](CSC8503/StateGameObject.cpp)
  * [Behaviour Trees](CSC8503CoreClasses/BehaviourSequence.cpp)
  * [Simple PathFinding](CSC8503/AiPathFollow.cpp)
----------------
> The game's level is designed under [Tutorial Game's file](CSC8503/TutorialGame.cpp).

## Gameplay:

Press M to start the game by switching to play mode, destroy objects and a door opens. Moving into that room you can find a sphere which is on patrol mode by default. If you move close to the sphere it’ll follow you around until you manage to move away from it. It’ll return to a normal state when you run away from the sphere. Across the other half of the map, you can find the pathfinding Goose which follows a path and then moves back along the same path.

## Basic Controls:

| Control | Description |
| :---: | :--- |
W    | Forward
S   | Backward
A    | Left
D    | Right
SHIFT         | to move faster
SPACE         | to jump
Q           | to switch between select mode 
M            | to switch between map mode and play mode (works only when not in switch mode)
G             | to toggle gravity
NUMPAD 7       | To increase the damp force
NUMPAD 4       | To decrease the damp force
Scroll wheel  | adds/decreases Force Magnitude

## Project Highlights:
The physics component incorporates the utilization of Newtonian physics, alongside collision detection and resolution systems, to replicate physically accurate interactions among objects and within the environment. In terms of game mechanics, different types of collision boxes were made using the concept of layers in Unity. Another feature implemented was adding constraints to simulate the opening of the gate.

AI was demonstrated in various forms, ranging from basic state machines responsible for managing moving platforms to AI adversaries employing behaviour trees. Behaviour trees enable the modelling of intricate behaviours and seamless transitions between them. Additionally, these AI entities made use of a straightforward pathfinding grid, featuring an implementation of the A* pathfinding algorithm.

## Problems faced during the project:
Problems I faced while making A* pathfinding was that the Goose wouldn't move to a certain point in the path and cause vector subscript out-of-range error. It was solved by increasing the radius which it considers a point and decreasing the speed at which it moves.

## Project Showcase:
> Click/tap on the image for a YouTube video showing the gameplay:

[![IMAGE ALT TEXT HERE](https://img.youtube.com/vi/JRBptyqvpQU/0.jpg)](https://www.youtube.com/watch?v=JRBptyqvpQU)
