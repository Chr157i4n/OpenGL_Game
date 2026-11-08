**Object Movement**
The Movement of all objects needs to be indepentant of the framerate. If the game adds the movement to the position every frame
without considering the framerate, it works perfectly fine, if the framerate is constant at 60 fps.
But if the framerate (without VSync) is much higher than 60, the movement is added to the position a lot more often.
And therefore the speed is higher at higher framerate.
To solve this the time between two frames (delta time) needs to be measured.
When the Engine then multiplies the movement with the current delta time, the object move further per frame, when the framerate is low.
Then the movement is the same with differente framerates.
When the Player wants to jump a constant impulse needs to be added and the gravity must also bei multiplied with the delta time before
beeing added to the movement.

**Model and Texture loading**
I Export the models made in Blender to the FBX format, but that is not important, since the used model loading Libary
Assimp can import many different formats. I programmed a second model converting tool, which obtains the needed information
from the model file and write them into a binary modeling file (.bmf).
The Model currently must contain a texture and a normal map per mesh.

**Configuration (loading and saving)**
For saving the configuration the engine uses a .ini file. It ignores comments, empty lines and sections.
When loading values the engine looks trough each line, split the line at the = and compares the first part with the value which should be loaded.
When writing it does the same and when the value is not found it will be added at the end of the file.
When a value is not found while loading the game uses it standard value for it and save that value in the config file when the user quit the game.

**Map file**
In the map file all used modelfiles, each object with its properties and all bots with navpoints are saved.
Because this file needs a more complex data structure (each object has a position), the engine uses XML files for the levels.


**Menu**
If the Player presses ESC while ingame the Engine opens the pause menu. Then the game itself is not beeing rendered anymore
and the fps are limited to 60 fps to reduce to gpu load.

**UI**
The UI is drawn after the postprocessing process at last directly onto the next frame in the doublebuffer.


**Camera**
In OpenGL there is no camera. the x and y axis are on the screen form -1 to +1. the -z axis goes in the depth of the screen.
the engine fakes a camera by moving all objects in the game world in the reverse direction when the camera is moved or turned.

"The engines don't move the ship at all. The ship stays where it is and the engines move the universe around it." - Futurama

So the models has its own coordinate system, the so called "model space". I set the origin in Blender for every object to the bottom center.
If the engine renders the object just like that, all objects would be directly in the center of the screen.
To move every object to its position and rotate it, the engine multiplies every model vertex in the shader with the model matrix.

To be able to move the camera around the scene the engine multiplies every model vertex afterwards with the view matrix which contatins the position and direction
the camera is looking from.

At last the vertices are multiplied with the projection matrix to get a perspective.

**3D Collision**
The engine currently is using first AABB (Axis Aligned Bounding Box) to determine whether a collision is possible, because this methods is really cheap.
If the objects are in range in the AABB-methods sais, that a collision is possible the engine use SAT (Seperate Axis Theorem) to precisely calculate whether
the objects are really colliding. for the SAT-method a simplified collision box is used. It can currently either be a cube or a prism (not working correctly).

AABB
This method is cheap, because it only consists of compares. But this method only work correctly, when the object are not rotated. So a bigger Bounding Box is used
to determine whether the object could collide. If the object is not in the Bounding Box of the other object, the objects cant collide.

SAT
The Seperate Axis Theorem is used to calculate whether two objects (polyhedrons), which must be convex are colliding.
"If there is one Axis, which seperates both objects, then the object dont collide."
The separation of both objects needs to be calculated for all normals of all edges of both objects. In case of the cube, a collection of every edge is also a collection
of all normals, so i use the edges directly. A second optimisation can be made for the cube, because there are 4 times the same normal and the cube has only 3 normals.
So in the end for two cubes the separation only needs to be calculated for 6 axis. If the object are separated on one of this 6 axis, then they are not colliding.
the separation can be calculated using the dot product of the axis and every edge point of the cube.
If the object collide the axis with the least collison is saved and the objects are moved a back on that axis the amount they collide.
Currently only the moving object is moved back. For correct physic both object should be moved back dependent on their mass. Then the player can move boxes by walking against them.

**Initialization procedure**
1. ConfigManager init
	-read the config.ini file
	-store all configuration in the ram inside the ConfigManager class
2. Game init
	2.1 Renderer init OpenGL	
		-creating window
		-creating opengl-context
		-setting all important flags in opengl
	2.2 load shaders
		-load and compile all shaders
	2.3 UI init
		-load font
		-create main ui elements
	2.4 load Map <- needs to be done after main menu and level choosen
		-load all models that are needed for a map
		-load textures of models
		-create all objects in the game world
	2.5 Renderer init
		-load skybox texture
		-init framebuffer
3. Gameloop
	-in this the menus or the game itself is rendered

**Game Loop**
1. Input processing
2. NPC tasks
3. Move objects
4. Check hits of bullets <- is currently doubled with the normal collision detection
5. Collision detection
6. Rendering
7. Frame limiting

**Rendering Pipeline**
0. Environment mapping (just once)
1. Shadow mapping
2. Rendering of opaque objects
3. Rendering of the skybox
4. Rendering of transparent objects
5. Postprocessing
6. UI rendering