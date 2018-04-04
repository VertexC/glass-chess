#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <pthread.h>
#include <iostream>
#include "global.h"
#include "config.h"

#include "scene.h"
#include "shader.h"
// #include "trace.h"
#include "object.h"
#include "tracer.h"
#include "util.h"
#include "bvh.h"
#include "bbox.h"
#include "mytimer.h"
int win_width = WIN_WIDTH;
int win_height = WIN_HEIGHT;

glm::vec3 frame[WIN_HEIGHT][WIN_WIDTH];

float image_width = IMAGE_WIDTH;
float image_height = (float(WIN_HEIGHT) / float(WIN_WIDTH)) * IMAGE_WIDTH;

// some colors
glm::vec3 background_clr;					   // background color
glm::vec3 null_clr = glm::vec3(0.0, 0.0, 0.0); // NULL color

glm::vec3 eye_pos = glm::vec3(0.0, 0.0, 0.0); // eye position
float image_plane = -1.5;					  // image plane position

// list of spheres in the scene
Scene *scene = NULL;
Bvh *bvh = NULL;
// maximum level of recursions; you can use to control whether reflection
// is implemented and for how many levels
int step_max = 1;

// You can put your flags here
int shadow_on = 0;	 // a flag to indicate whether you want to have shadows
int reflect_on = 0;	// a flag to indicate whether to have reflection
int chessboard_on = 0; // whether to set up chessboard
int refract_on = 0;	// whether have refraction effect
int difref_on = 0;	 // whether to have diffuse reflection
int antiAlias_on = 0;  // whether to have anti alias
int triangle_on = 0;   // whether show triangle mesh

int objectCount = 0; // number of objects

// OpenGL
const int NumPoints = 6;

// Thread
const int ThreadNum = 10;

//----------------------------------------------------------------------------
void *RayTrace(void *tracer)
{
	Tracer *mytracer = (Tracer *)tracer;
	mytracer->ray_trace();
	std::cout << "my tracer done" << std::endl;
	pthread_exit(NULL);
}

//----------------------------------------------------------------------------
void init()
{
	// Vertices of a square
	float ext = 1.0;
	float points[NumPoints * 4] = {
		-ext, -ext, 0, 1.0, //v1
		ext, -ext, 0, 1.0,  //v2
		-ext, ext, 0, 1.0,  //v3
		-ext, ext, 0, 1.0,  //v3
		ext, -ext, 0, 1.0,  //v2
		ext, ext, 0, 1.0	//v4
	};

	// Texture coordinates
	float tex_coords[NumPoints * 2] = {
		0.0, 0.0,
		1.0, 0.0,
		0.0, 1.0,
		0.0, 1.0,
		1.0, 0.0,
		1.0, 1.0};

	// Initialize texture Objects
	float newFrame[WIN_HEIGHT][WIN_WIDTH][3];

	for (int i = 0; i < WIN_HEIGHT; i++)
		for (int j = 0; j < WIN_WIDTH; j++)
		{
			newFrame[i][j][0] = frame[i][j].x;
			newFrame[i][j][1] = frame[i][j].y;
			newFrame[i][j][2] = frame[i][j].z;
		}

	GLuint texture;
	glGenTextures(1, &texture);

	glBindTexture(GL_TEXTURE_2D, texture);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, WIN_WIDTH, WIN_HEIGHT, 0,
				 GL_RGB, GL_FLOAT, newFrame);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glActiveTexture(GL_TEXTURE0);

	// Create and initialize a buffer Object
	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(points) + sizeof(tex_coords), NULL, GL_STATIC_DRAW);
	GLintptr offset = 0;
	glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(points), points);
	offset += sizeof(points);
	glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(tex_coords), tex_coords);

	// Load shaders and use the resulting shader program
	Shader myShader("vshader.glsl", "fshader.glsl");
	myShader.Use();

	// set up vertex arrays
	offset = 0;
	GLuint vPosition = glGetAttribLocation(myShader.Program, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0,
						  (GLvoid *)(offset));

	offset += sizeof(points);
	GLuint vTexCoord = glGetAttribLocation(myShader.Program, "vTexCoord");
	glEnableVertexAttribArray(vTexCoord);
	glVertexAttribPointer(vTexCoord, 2, GL_FLOAT, GL_FALSE, 0,
						  (GLvoid *)(offset));

	glUniform1i(glGetUniformLocation(myShader.Program, "texture"), 0);

	glClearColor(1.0, 1.0, 1.0, 1.0);
}

/*********************************************************
 * This is the OpenGL display function. It is called by
 * the event handler to draw the scene after you have
 * rendered the image using ray tracing. Remember that
 * the pointer to the image memory is stored in 'frame'.
 *
 * There is no need to change this.
 **********************************************************/

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_CULL_FACE);

	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	glutPostRedisplay();

	glutSwapBuffers();
}

/*********************************************************
 * This function handles keypresses
 *
 *   s - save image
 *   q - quit
 *
 * DO NOT CHANGE
 *********************************************************/

void keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'q':
	case 'Q':

		exit(0);
		break;
	case 's':
	case 'S':
		save_image();
		glutPostRedisplay();
		break;
	default:
		break;
	}
}

//----------------------------------------------------------------------------

int main(int argc, char **argv)
{
	// Parse the arguments
	if (argc < 2)
	{
		printf("Missing arguments ... use:\n");
		printf("./raycast step_max <options>\n");
		return -1;
	}

	step_max = atoi(argv[1]); // maximum level of recursions
	bool parallel_on = false;
	bool bvh_on = false;
	// Optional arguments
	for (int i = 2; i < argc; i++)
	{
		// if (strcmp(argv[i], "+s") == 0)
		// 	shadow_on = 1;
		// if (strcmp(argv[i], "+l") == 0)
		// 	reflect_on = 1;
		// if (strcmp(argv[i], "+c") == 0)
		// 	chessboard_on = 1;
		// if (strcmp(argv[i], "+r") == 0)
		// 	refract_on = 1;
		// if (strcmp(argv[i], "+f") == 0)
		// 	difref_on = 1;
		// if (strcmp(argv[i], "+p") == 0)
		// 	antiAlias_on = 1;
		// if (strcmp(argv[i], "+t") == 0)
		// 	triangle_on = 1;
		if (strcmp(argv[i], "+p") == 0)
			parallel_on = true;
		if (strcmp(argv[i], "+b") == 0)
			bvh_on = true;
	}

	scene = new Scene();
	scene->set_chess();
	scene->set_board();
	if (bvh_on)
	{
		bvh = new Bvh(&(scene->objectList));
	}

	printf("Rendering scene using my fantastic ray tracer...\n");

	// set Timer
	Timer timer;

	float x_grid_size = image_width / float(win_width);
	float y_grid_size = image_height / float(win_height);
	float x_start = -0.5 * image_width;
	float y_start = -0.5 * image_height;

	if (!parallel_on)
	{
		Tracer *tracer = new Tracer(scene, (glm::vec3 **)frame, x_start, y_start, win_height, win_width,
									x_grid_size, y_grid_size, image_plane, eye_pos, step_max, 0, 0, win_width, win_height,
									bvh);
		tracer->ray_trace();
		printf("Num of intersection: %d\n", tracer->getIntersectCount());
	}
	else
	{
		int total_count = 0;
		// create thread for parallel ray tracing
		int indexes[ThreadNum];
		pthread_t threads[ThreadNum];
		Tracer *tracers[ThreadNum];
		int rc;
		int widSize = win_width / ThreadNum;
		for (int i = 0; i < ThreadNum; i++)
		{
			int x_l = i * widSize;
			int x_r = i * widSize + widSize - 1;
			if (i == ThreadNum - 1)
			{
				x_r = win_width;
			}

			indexes[i] = i;
			std::cout << "Create Thread " << i << " for Ray-Tracing" << std::endl;
			tracers[i] = new Tracer(scene, (glm::vec3 **)frame, x_start, y_start, win_height, win_width,
									x_grid_size, y_grid_size, image_plane, eye_pos, step_max, x_l, 0, x_r, win_height,
									bvh);

			rc = pthread_create(&threads[i], NULL, RayTrace, (void *)&(*tracers[i]));
			if (rc)
			{
				std::cout << "Error: Cannot create thread, " << rc << std::endl;
				exit(-1);
			}
		}
		void *status;
		for (int i = 0; i < ThreadNum; i++)
		{
			pthread_join(threads[i], &status);
			total_count += tracers[i]->getIntersectCount();
		}

		printf("Num of intersection: %d\n", total_count);
	}
	double duration = timer.get_time();
	printf("Ray Trace done in %lf s\n", duration);
	// we want to make sure that intensity values are normalized
	histogram_normalization();

	// Show the result in glut via texture mapping
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutInitWindowSize(WIN_WIDTH, WIN_HEIGHT);
	glutCreateWindow("Ray tracing");
	glewInit();

	init();

	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutMainLoop();

	delete scene;
	delete bvh;

	return 0;
}
