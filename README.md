# 2D Rigid Body Physics Engine
I created this physics engine for the final project in my grade 12 physics class (SPH 4U).

Using Newtonian mechanics, the engine accurately models:
- Rigid body dynamics
- Kinematics (movement of objects)
- Friction
- Collisions
- Rotational movement
- Circles and arbitrary convex polygons
- Custom material density, friction/restitution coefficients

## Demo
https://github.com/user-attachments/assets/f4fb36ec-29e9-46d1-b7df-f56fe0014f1c

<br>
A simple use case of the dynamics system is to model a solar system with Newtonian gravity:

https://github.com/user-attachments/assets/770baeb7-582e-45df-8548-b94b5ecc08d6

## Accuracy
- Able to simulate basic object interactions to a reasonable degree of accuracy
- Inherently limited due to its nature as a fixed timestep engine (i.e. as opposed to continuous timesteps and iterative constraint solvers)

However, for simple to intermediate scenes the engine is remarkably accurate.
The following examples were taken from my presentation slideshow:

### Kinematics example
<img width="1820" height="1023" alt="image" src="https://github.com/user-attachments/assets/5034e5bf-cef3-4594-bb69-0dcc28afb400" />
<br>

https://github.com/user-attachments/assets/33f012fe-01d2-459a-97a6-4da7c62fcbe6

<br>

### Dynamics example
<img width="1820" height="1024" alt="image" src="https://github.com/user-attachments/assets/17740513-54b0-41a9-a79a-c4c5dd43f3cf" />
<br>

https://github.com/user-attachments/assets/906d53bf-d1bd-4eb5-bd9d-e4a55c39c16c

