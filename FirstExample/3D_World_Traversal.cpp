using namespace std;

#include "vgl.h"
#include "LoadShaders.h"
#include "glm\glm.hpp"
#include "glm\gtc\matrix_transform.hpp"
#include "glm\gtx\rotate_vector.hpp"
#include "..\SOIL\src\SOIL.h"
#include <iostream>
#include <vector>
#include "GameObject.h"

enum VAO_IDs { Triangles, NumVAOs };
enum Buffer_IDs { ArrayBuffer};
enum Attrib_IDs { vPosition = 0 };

const GLint NumBuffers = 2;
GLuint VAOs[NumVAOs];
GLuint Buffers[NumBuffers];
GLuint location;
GLuint cam_mat_location;
GLuint proj_mat_location;
GLuint texture[3];	//Array of pointers to textrure data in VRAM. We use two textures in this example.

int timeOfLastSpawn = 0;



const GLuint NumVertices = 60;

//Height of camera (player) from the level
float height = 0.8f;

//Player motion speed for movement and pitch/yaw
float travel_speed = 300.0f;		//Motion speed
float mouse_sensitivity = 0.01f;	//Pitch/Yaw speed

//Used for tracking mouse cursor position on screen
int x0 = 0;	
int y_0 = 0;
 
//Transformation matrices and camera vectors
glm::mat4 model_view;
glm::vec3 unit_z_vector = glm::vec3(0, 0, 1);	//Assigning a meaningful name to (0,0,1)
glm::vec3 cam_pos = glm::vec3(0.0f, 0.0f, height);
glm::vec3 forward_vector = glm::vec3(1, 1, 0);	//Forward vector is parallel to the level at all times (No pitch)

//The direction which the camera is looking, at any instance
glm::vec3 looking_dir_vector = glm::vec3(1, 1, 0);
glm::vec3 up_vector = unit_z_vector;
glm::vec3 side_vector = glm::cross(up_vector, forward_vector);


//Used to measure time between two frames
int oldTimeSinceStart = 0;
int deltaTime;

GLfloat alpha = 0;

//Creating and rendering bunch of objects on the scene to interact with
const int Num_Obstacles = 50;
float obstacle_data[Num_Obstacles][3];
int numEnemies = 0;

//win condition variables
int numKilled = 0;

vector<GameObject> gameObjects;


//Helper function to generate a random float number within a range
float randomFloat(float a, float b)
{
	float random = ((float)rand()) / (float)RAND_MAX;
	float diff = b - a;
	float r = random * diff;
	return a + r;
}



// inititializing buffers, coordinates, setting up pipeline, etc.
void init(void)
{
	glEnable(GL_DEPTH_TEST);

	//Normalizing all vectors
	up_vector = glm::normalize(up_vector);
	forward_vector = glm::normalize(forward_vector);
	looking_dir_vector = glm::normalize(looking_dir_vector);
	side_vector = glm::normalize(side_vector);

	//Randomizing the position and scale of obstacles - this doesn't do anything right now
	for (int i = 0; i < Num_Obstacles; i++)
	{
		obstacle_data[i][0] = randomFloat(-50, 50); //X
		obstacle_data[i][1] = randomFloat(-50, 50); //Y
		obstacle_data[i][2] = randomFloat(0.1, 10.0); //Scale
	}

	ShaderInfo shaders[] = {
		{ GL_VERTEX_SHADER, "triangles.vert" },
		{ GL_FRAGMENT_SHADER, "triangles.frag" },
		{ GL_NONE, NULL }
	};

	GLuint program = LoadShaders(shaders);
	glUseProgram(program);	//My Pipeline is set up


	
	GLfloat vertices[NumVertices][3] = {
		
		{ -100.0, -100.0, 0.0 }, //Plane to walk on and a sky
		{ 100.0, -100.0, 0.0 },
		{ 100.0, 100.0, 0.0 },
		{ -100.0, 100.0, 0.0 },

		//cube
		{ -0.45, -0.45 ,0.01 }, // bottom face
		{ 0.45, -0.45 ,0.01 },
		{ 0.45, 0.45 ,0.01 },
		{ -0.45, 0.45 ,0.01 },

		{ -0.45, -0.45 ,0.9 }, //top face
		{ 0.45, -0.45 ,0.9 },
		{ 0.45, 0.45 ,0.9 },
		{ -0.45, 0.45 ,0.9 },

		{ 0.45, -0.45 , 0.01 }, //left face
		{ 0.45, 0.45 , 0.01 },
		{ 0.45, 0.45 ,0.9 },
		{ 0.45, -0.45 ,0.9 },

		{ -0.45, -0.45, 0.01 }, //right face
		{ -0.45, 0.45 , 0.01 },
		{ -0.45, 0.45 ,0.9 },
		{ -0.45, -0.45 ,0.9 },

		{ -0.45, 0.45 , 0.01 }, //front face
		{ 0.45, 0.45 , 0.01 },
		{ 0.45, 0.45 ,0.9 },
		{ -0.45, 0.45 ,0.9 },
	
		{ -0.45, -0.45 , 0.01 }, //back face 
		{ 0.45, -0.45 , 0.01 },
		{ 0.45, -0.45 ,0.9 },
		{ -0.45, -0.45 ,0.9 }, //28

		




		//hexagon

		
		{1.0f,   -0.25f, -0.5f},    // left top
		{1.0f,   -0.25f, 0.5f},    // right top		
		{0.0f,   -0.25f, 1.0f},    // right
		{-1.0f,   -0.25f, 0.5f},    // right bottom		
		{-1.0f,  -0.25f, -0.5f},    // left bottom
		{0.0f,   -0.25f, -1.0f},     // left
		

		//2nd hexagon
		{1.0f,   0.25f, -0.5f},    // left top
		{1.0f,   0.25f, 0.5f},    // right top		
		{0.0f,   0.25f, 1.0f},    // right
		{-1.0f,   0.25f, 0.5f},    // right bottom		
		{-1.0f,  0.25f, -0.5f},    // left bottom
		{0.0f,   0.25f, -1.0f},     // left
		

		//quads connecting hexagons
		{ 1.0f,   -0.25f, -0.5f },    // left top
		{ 1.0f,   0.25f, -0.5f },    // left top
		{ 1.0f,   0.25f, 0.5f },    // right top
		{ 1.0f,   -0.25f, 0.5f },    // right top

		{ 0.0f,   -0.25f, 1.0f },    // right
		{ 0.0f,   0.25f, 1.0f },    // right
		{ -1.0f,   0.25f, 0.5f },    // right bottom
		{ -1.0f,   -0.25f, 0.5f },    // right bottom

		{ 0.0f,   -0.25f, -1.0f },     // left
		{ 0.0f,   0.25f, -1.0f },     // left
		{ -1.0f,  0.25f, -0.5f },    // left bottom
		{ -1.0f,  -0.25f, -0.5f },    // left bottom
		
		{ 1.0f,   -0.25f, -0.5f },    // left top
		{ 1.0f,   0.25f, -0.5f },    // left top
		{ 0.0f,   0.25f, -1.0f },     // left
		{ 0.0f,   -0.25f, -1.0f },     // left

		{ -1.0f,   0.25f, 0.5f },    // right bottom
		{ -1.0f,   -0.25f, 0.5f },    // right bottom
		{ -1.0f,  -0.25f, -0.5f },   // left bottom
		{ -1.0f,  0.25f, -0.5f },    // left bottom
		    




	};

	//These are the texture coordinates for the second texture
	GLfloat textureCoordinates[60][2] = {
		0.0f, 0.0f,
		200.0f, 0.0f,
		200.0f, 200.0f,
		0.0f, 200.0f,
		
		0.0f, 1.0f,
		1.0f, 1.0f,
		1.0f, 0.0f,
		0.0f, 0.0f,
		
		0.0f, 1.0f,
		1.0f, 1.0f,
		1.0f, 0.0f,
		0.0f, 0.0f,

		0.0f, 1.0f,
		1.0f, 1.0f,
		1.0f, 0.0f,
		0.0f, 0.0f,

		0.0f, 1.0f,
		1.0f, 1.0f,
		1.0f, 0.0f,
		0.0f, 0.0f,

		0.0f, 1.0f,
		1.0f, 1.0f,
		1.0f, 0.0f,
		0.0f, 0.0f,

		0.0f, 1.0f,
		1.0f, 1.0f,
		1.0f, 0.0f,
		0.0f, 0.0f,

		0.0f, 1.0f,
		1.0f, 1.0f,
		1.0f, 0.0f,
		0.0f, 0.0f,

		0.0f, 1.0f,
		1.0f, 1.0f,
		1.0f, 0.0f,
		0.0f, 0.0f,

		0.0f, 1.0f,
		1.0f, 1.0f,
		1.0f, 0.0f,
		0.0f, 0.0f,

		0.0f, 1.0f,
		1.0f, 1.0f,
		1.0f, 0.0f,
		0.0f, 0.0f,

		0.0f, 1.0f,
		1.0f, 1.0f,
		1.0f, 0.0f,
		0.0f, 0.0f,

		0.0f, 1.0f,
		1.0f, 1.0f,
		1.0f, 0.0f,
		0.0f, 0.0f,

		0.0f, 1.0f,
		1.0f, 1.0f,
		1.0f, 0.0f,
		0.0f, 0.0f,

		0.0f, 1.0f,
		1.0f, 1.0f,
		1.0f, 0.0f,
		0.0f, 0.0f,
	};


	//Creating our texture:
	//This texture is loaded from file. To do this, we use the SOIL (Simple OpenGL Imaging Library) library.
	//When using the SOIL_load_image() function, make sure the you are using correct patrameters, or else, your image will NOT be loaded properly, or will not be loaded at all.
	GLint width1, height1;
	unsigned char* textureData1 = SOIL_load_image("grass.png", &width1, &height1, 0, SOIL_LOAD_RGB);

	GLint width2, height2;
	unsigned char* textureData2 = SOIL_load_image("wheel.jpg", &width2, &height2, 0, SOIL_LOAD_RGB);

	GLint width3, height3;
	unsigned char* textureData3 = SOIL_load_image("ammo.png", &width3, &height3, 0, SOIL_LOAD_RGB);

	glGenBuffers(2, Buffers);
	glBindBuffer(GL_ARRAY_BUFFER, Buffers[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glBindAttribLocation(program, 0, "vPosition");
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, Buffers[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(textureCoordinates), textureCoordinates, GL_STATIC_DRAW);
	glBindAttribLocation(program, 1, "vTexCoord");
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(1);

	location = glGetUniformLocation(program, "model_matrix");
	cam_mat_location = glGetUniformLocation(program, "camera_matrix");
	proj_mat_location = glGetUniformLocation(program, "projection_matrix");

	///////////////////////TEXTURE SET UP////////////////////////
	
	//Allocating three buffers in VRAM
	glGenTextures(3, texture);

	//First Texture: 

	//Set the type of the allocated buffer as "TEXTURE_2D"
	glBindTexture(GL_TEXTURE_2D, texture[0]);

	//Loading the second texture into the second allocated buffer:
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width1, height1, 0, GL_RGB, GL_UNSIGNED_BYTE, textureData1);

	//Setting up parameters for the texture that recently pushed into VRAM
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);


	//And now, second texture: 

	//Set the type of the allocated buffer as "TEXTURE_2D"
	glBindTexture(GL_TEXTURE_2D, texture[1]);

	//Loading the second texture into the second allocated buffer:
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width2, height2, 0, GL_RGB, GL_UNSIGNED_BYTE, textureData2);

	//Setting up parameters for the texture that recently pushed into VRAM
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	//And now, second texture: 

	//Set the type of the allocated buffer as "TEXTURE_2D"
	glBindTexture(GL_TEXTURE_2D, texture[2]);

	//Loading the second texture into the second allocated buffer:
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width3, height3, 0, GL_RGB, GL_UNSIGNED_BYTE, textureData3);

	//Setting up parameters for the texture that recently pushed into VRAM
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	//////////////////////////////////////////////////////////////
}

//Helper function to draw a cube
void drawCube(float scale)
{
	model_view = glm::scale(model_view, glm::vec3(scale, scale, scale));
	glUniformMatrix4fv(location, 1, GL_FALSE, &model_view[0][0]);

	//Select the second texture (ammo.png) when drawing the second geometry (cube)
	glBindTexture(GL_TEXTURE_2D, texture[2]);
	glDrawArrays(GL_QUADS, 4, 24);
}

void drawHexagon(float scale) {
	model_view = glm::scale(model_view, glm::vec3(scale, scale, scale));
	glUniformMatrix4fv(location, 1, GL_FALSE, &model_view[0][0]);

	glBindTexture(GL_TEXTURE_2D, texture[1]); //wheel texture
	glDrawArrays(GL_POLYGON, 28, 6);
	glDrawArrays(GL_POLYGON, 34, 6);
	glDrawArrays(GL_QUADS, 40, 12);
	glDrawArrays(GL_QUADS, 42, 4);
	glDrawArrays(GL_QUADS, 56, 4);
	glDrawArrays(GL_QUADS, 52, 4);

}

void drawEnemyOrBullet(float scale, glm::vec3 direction) //repurposed drawCube for draw "enemy"
{
	model_view = glm::scale(model_view, glm::vec3(scale, scale, scale));
	

	//Select the 3rd texture (ammo.png) when drawing the second geometry (cube)
	glBindTexture(GL_TEXTURE_2D, texture[2]);
	model_view = glm::translate(model_view, glm::vec3(0.0f, 0.0f, 0.45f)); //lifted off the ground so that the wheels aren't stuck in the ground
	model_view = glm::rotate(model_view, atan(direction.y / direction.x), unit_z_vector);
	glUniformMatrix4fv(location, 1, GL_FALSE, &model_view[0][0]);
	glDrawArrays(GL_QUADS, 4, 24);

	glm::mat4 temp = model_view;

	model_view = glm::translate(model_view, glm::vec3(scale / 2.0, scale / 2.0, -0.1f));	
	model_view = glm::rotate(model_view, alpha, glm::vec3(0, 1, 0));
	drawHexagon(scale / 3.0);
	model_view = temp;

	model_view = glm::translate(model_view, glm::vec3((scale*-1.0) / 2.0, scale / 2.0, -0.1f));
	model_view = glm::rotate(model_view, alpha, glm::vec3(0, 1, 0));
	drawHexagon(scale / 3.0);
	model_view = temp;

	model_view = glm::translate(model_view, glm::vec3(scale / 2.0, (scale*-1.0) / 2.0, -0.1f));
	model_view = glm::rotate(model_view, alpha, glm::vec3(0, 1, 0));
	drawHexagon(scale / 3.0);
	model_view = temp;

	model_view = glm::translate(model_view, glm::vec3((scale*-1.0) / 2.0, (scale*-1.0) / 2.0, -0.1f));
	model_view = glm::rotate(model_view, alpha, glm::vec3(0, 1, 0));
	drawHexagon(scale / 3.0);
	model_view = temp;



	//glDrawArrays(GL_QUADS, 50, 4);
}



void draw_objects() {
	int timeSinceStart = glutGet(GLUT_ELAPSED_TIME);

	if (timeSinceStart - timeOfLastSpawn > 2000) { 
		gameObjects.push_back(GameObject('e', glm::vec3(randomFloat(-30, 30), randomFloat(-30, 30), 0.0f), looking_dir_vector, glutGet(GLUT_ELAPSED_TIME)));
		timeOfLastSpawn = glutGet(GLUT_ELAPSED_TIME);
		
		
	}

	
	
	for (int i = 0; i < gameObjects.size(); i++) {		
		if (gameObjects[i].isAlive()) {
			model_view = glm::translate(model_view, gameObjects[i].getLocation());
			
			glUniformMatrix4fv(location, 1, GL_FALSE, &model_view[0][0]);
			if (gameObjects[i].getType() == 'b') {	//if type is bullet, draw at scale of 0.05
				drawCube(0.05);
			}
			else if (gameObjects[i].getType() == 'e') { //if type is enemy, draw at scale of 1
				drawEnemyOrBullet(1, gameObjects[i].getDirection());
			}
			model_view = glm::mat4(1.0);
		}
		
	}
	
	
	for (int i = 0; i < gameObjects.size(); i++) {
		if (!gameObjects[i].isAlive()) {
			gameObjects.erase(gameObjects.begin() + i);
			i--;
		}
	}
}

void updateLocations() {
	vector<int> enemies;
	for (int i = 0; i < gameObjects.size(); i++) {
		if (gameObjects[i].getType() == 'b') {
			gameObjects[i].updateLocation();
			if ((glutGet(GLUT_ELAPSED_TIME) - gameObjects[i].getTimeCreated()) > 2000) {				//let bullet fly for 2 seconds then set it up to be deleted				
				gameObjects[i].setDead();
			}
		}
		if (gameObjects[i].getType() == 'e') {	//seperating behavior for enemy gameobject types from bullet
			gameObjects[i].updateLocation(glm::vec3(0,0,0));	//calculates direction towards center, moves and if it comes too close destroys the object
			enemies.push_back(i); //store indexes of enemies to slightly reduce number of iterations needed
		}
	}

	for (int i = 0; i < enemies.size(); i++) {
		if (gameObjects[enemies[i]].checkIfTooClose(cam_pos)) {
			cout << "You got hit by a tank! You lose!\n";
			glutDestroyWindow(glutGetWindow());
		}
		for (int j = 0; j < gameObjects.size(); j++) {
			if (enemies[i] != j) {
				if (gameObjects[enemies[i]].checkIfTooClose(gameObjects[j].getLocation())) {
					if (gameObjects[j].getType() == 'e') { //if the type is enemy, kill both and don't add to score
						gameObjects[j].setDead();
						gameObjects[enemies[i]].setDead();

						cout << "2 enemies were too close and have destroyed eachother\n";
					}
					if (gameObjects[j].getType() == 'b') { //if the type is bullet, kill both and add to score
						gameObjects[j].setDead();
						gameObjects[enemies[i]].setDead();
						numKilled++;
						
						cout << "You've killed " << numKilled << " vehicles! Kill 10 to win\n";
						if (numKilled >= 10) {
							cout << "You win! Game closing now\n";
							glutDestroyWindow(glutGetWindow());
						}
					}
				}
			}
		}
	}
}

//Renders level
void draw_level()
{
	
	model_view = glm::mat4(1.0);
	
	//Rendering obstacles obstacles
	

	//Select the first texture (grass.png) when drawing the first geometry (floor)
	glBindTexture(GL_TEXTURE_2D, texture[0]);
	glDrawArrays(GL_QUADS, 0, 4);
}



//---------------------------------------------------------------------
//
// display
//
void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	model_view = glm::mat4(1.0);
	glUniformMatrix4fv(location, 1, GL_FALSE, &model_view[0][0]);

	//The 3D point in space that the camera is looking
	glm::vec3 look_at = cam_pos + looking_dir_vector;

	glm::mat4 camera_matrix = glm::lookAt(cam_pos, look_at, up_vector);
	glUniformMatrix4fv(cam_mat_location, 1, GL_FALSE, &camera_matrix[0][0]);

	glm::mat4 proj_matrix = glm::frustum(-0.01f, +0.01f, -0.01f, +0.01f, 0.01f, 100.0f);
	glUniformMatrix4fv(proj_mat_location, 1, GL_FALSE, &proj_matrix[0][0]);

	draw_level();
	draw_objects();
	updateLocations();
	

	glFlush();
}


void keyboard(unsigned char key, int x, int y)
{
	if (key == 'a')
	{
		//Moving camera along opposit direction of side vector
		cam_pos += side_vector * travel_speed * ((float)deltaTime) / 1000.0f;
	}
	if (key == 'd')
	{
		//Moving camera along side vector
		cam_pos -= side_vector * travel_speed * ((float)deltaTime) / 1000.0f;
	}
	if (key == 'w')
	{
		//Moving camera along forward vector. To be more realistic, we use X=V.T equation in physics
		cam_pos += forward_vector * travel_speed * ((float)deltaTime) / 1000.0f;
	}
	if (key == 's')
	{
		//Moving camera along backward (negative forward) vector. To be more realistic, we use X=V.T equation in physics
		cam_pos -= forward_vector * travel_speed * ((float)deltaTime) / 1000.0f;
	}
	if (key == 'f') {
		//creates "bullet" when you push f
		gameObjects.push_back(GameObject('b', cam_pos, looking_dir_vector, glutGet(GLUT_ELAPSED_TIME)));
	}

}

//Controlling Pitch with vertical mouse movement
void mouse(int x, int y)
{
	//Controlling Yaw with horizontal mouse movement
	int delta_x = x - x0;
	
	//The following vectors must get updated during a yaw movement
	forward_vector = glm::rotate(forward_vector, -delta_x * mouse_sensitivity, unit_z_vector);
	looking_dir_vector = glm::rotate(looking_dir_vector, -delta_x * mouse_sensitivity, unit_z_vector);
	side_vector = glm::rotate(side_vector, -delta_x * mouse_sensitivity, unit_z_vector);
	up_vector = glm::rotate(up_vector, -delta_x * mouse_sensitivity, unit_z_vector);
	x0 = x;

	//The following vectors must get updated during a pitch movement
	int delta_y = y - y_0; 
	glm::vec3 tmp_up_vec = glm::rotate(up_vector, delta_y * mouse_sensitivity, side_vector);
	glm::vec3 tmp_looking_dir = glm::rotate(looking_dir_vector, delta_y * mouse_sensitivity, side_vector);

	//The dot product is used to prevent the user from over-pitch (pitching 360 degrees)
	//The dot product is equal to cos(theta), where theta is the angle between looking_dir and forward vector
	GLfloat dot_product = glm::dot(tmp_looking_dir, forward_vector);

	//If the angle between looking_dir and forward vector is between (-90 and 90) degress 
	if (dot_product > 0)
	{
		up_vector = glm::rotate(up_vector, delta_y * mouse_sensitivity, side_vector);
		looking_dir_vector = glm::rotate(looking_dir_vector, delta_y * mouse_sensitivity, side_vector);
	}
	y_0 = y;
}

void idle()
{
	float omega = 0.001;
	//Calculating the delta time between two frames
	//We will use this delta time when moving forward (in keyboard function)
	int timeSinceStart = glutGet(GLUT_ELAPSED_TIME);
	deltaTime = timeSinceStart - oldTimeSinceStart;
	oldTimeSinceStart = timeSinceStart;

	alpha = omega * timeSinceStart;
	//cout << timeSinceStart << " " << oldTimeSinceStart << " " << deltaTime << endl;
	glutPostRedisplay();
}

//---------------------------------------------------------------------
//
// main
//
int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA);
	glutInitWindowSize(1024, 1024);
	glutCreateWindow("Tanks Game");

	glewInit();	//Initializes the glew and prepares the drawing pipeline.

	init();

	glutDisplayFunc(display);

	glutKeyboardFunc(keyboard);

	glutIdleFunc(idle);

	glutPassiveMotionFunc(mouse);

	glutMainLoop();
	
	

}
