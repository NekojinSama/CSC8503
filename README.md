# Goat Game | Game Engine

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
NUMPAD 7       | to increase damp force
NUMPAD 4       | to decrease damp force
Scroll wheel  | adds/decreases Force Magnitude

## Gameplay:

Press M to start game by switch to play mode, destroy objects and a door opens. Moving in that room you can find a sphere which is on patrol mode by default. If you move close to the sphere it’ll follow you around until you manage to move away from it. It’ll return to normal state when you runaway from the sphere. Across the other half of the map you can find the pathfinding Goose which follows a path and then moves back along the same path.


## Features Implemented:

### Physics:
  * Raycasting
  * Collion Detection
  * Layer Masking
  * Application of force
  * constraints
----------------
### AI
  * State Machine
  * Behaviour Trees
  * Simple PathFinding
