#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glew.h>
#include <GL/glut.h>
#endif
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <vector>
#include <map>
#include "transformacao.h"
#include "tinyxml2.h"
using namespace tinyxml2;



/**
*		Global variables used to store program data, rotations, translations, view
*	mode, mouse coords, colors, some variables used throughout the code, and
* finnaly the triangles used to render every object.
*		We will get into more details when we get to the functions that alter this
* values.
*/
float alfa = 0.0f, beta = 0.5f, radius = 5000.0f;
float camX, camY, camZ;
float rotation = 0.0f;
float vert_rot = 0.0f, hori_rot = 0.0f;
float vert_trans = 0.0f, hori_trans = 0.0f;
char view_mode = 'l';
int x_pos = 400, y_pos = 400;
bool click = false;
float x,y,z;
int N = 1, timebase = 0, frame = 0;
GLuint buffers[2];


/**
*		This time we decided to use a vector to store every information
*	about the XML transformations, whether is angle and coordinates,
* rotations, colors and names of the models.
*		This structure also has a pointer to the next transformation that it
* has to render.
*/
std::vector<Transformacao*> transformacoes;




/**
* 	Function's declatarion allowing them to be put after main, or without
*	any specific order.
*		The specification for each of the functions will be written just before
*	their full implementation.
*/
void color(XMLElement* element2);
void translate(XMLElement* element2);
void rotate(XMLElement* element2);
void scale(XMLElement* element2);
void model(XMLElement* element2);
void light(XMLElement* element2);
void popMatrix();
void pushMatrix();
int parserXML(XMLElement* pListElement);
int processXML(char* file);
void renderScene(void);
void renderFigures(void);
void changeSize(int w, int h);
void special_key_handler(int key, int x, int y);
void normal_key_handler(unsigned char c, int x, int y);
void movement_handler(int x, int y);
void mouse_handler(int button, int state, int x, int y);
void spherical2Cartesian();
void showFPS();
std::vector<float> processPointTranslate(XMLElement *element2);
//void loadTexture(char *imagem);
//void init(char *imagem);





/**
* 	This function is the main function of the program, every other function is
*	fully deppendent of this onde, both directly or indirectly. This is
*	responsible for initializing variables and setting up OpenGL. It also
* serves as a first step to user input validation.
*		The main function of the program is responsible for setting up not only
* the OpenGL elements but making sure that every variable is correctly set
* up for the rest of the program to continue, it's code is in an unchangeble
* order, and by altering it we might not obtain the desired results.
*
*	@param argc - Integer representing the number of arguments on which
*								the program was invoked
* @param argv - Array of Strings with every word which the program was
*								invoked
* @return int - Integer that serves as last resource to indicate if an error
*								as occured
*/
int main(int argc, char **argv) {
	float white[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

	if(argc < 2) {
		printf("Invalid Input!\n");
		return -1;
	}

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH|GLUT_DOUBLE|GLUT_RGBA);
	glutInitWindowPosition(100,100);
	glutInitWindowSize(800,800);
	glutCreateWindow("Rendering scene");

	glutDisplayFunc(renderScene);
	glutReshapeFunc(changeSize);

	glutSpecialFunc(special_key_handler);
	glutKeyboardFunc(normal_key_handler);
	glutMouseFunc(mouse_handler);
	glutMotionFunc(movement_handler);

	#ifndef __APPLE___
		glewInit();
	#endif

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	spherical2Cartesian();
	
	glGenBuffers(3, buffers);

	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, white);

	//glBindTexture(GL_TEXTURE_2D, texture);
	glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
	glBindBuffer(GL_ARRAY_BUFFER, buffers[1]);
	glBindBuffer(GL_ARRAY_BUFFER, buffers[2]);
	
	int exitCode = processXML(argv[1]);
	if(exitCode < 0) return -1;
	
	//init("terra.jpg");

	glutMainLoop();

	return 1;
}



/**
* 	This function is used everytime we want to open a 3d model file and save
* it to a global variable (t).
*		It will read a triplet of floats for each line of document that it reads.
*	We defined that as the composition of our .3d model files, so this function
* reads them procedurally and saves it to a vector.
*		We can assume that the .3d file as an ammount of lines %3 equal to 0.
*
*	@param tok - Character representing the name of the file to open.
* @return vector - Vector containing the coordinates os every object.
*/
void open3dModel(const char* tok, std::vector<float>& v, std::vector<float>& n){
 	FILE *f_3d;
 	printf("Open\n");
 	f_3d = fopen(tok, "r+");
 	if (f_3d < 0) return;
 	printf("Open1\n");

	char *s1, *s2, *s3, *s4, *s5, *s6;
 	s1 = (char*) malloc(sizeof(char) * 64);
 	s2 = (char*) malloc(sizeof(char) * 64);
 	s3 = (char*) malloc(sizeof(char) * 64);
 	s4 = (char*) malloc(sizeof(char) * 64);
 	s5 = (char*) malloc(sizeof(char) * 64);
 	s6 = (char*) malloc(sizeof(char) * 64);
 	printf("Open2\n");
 
 	while(!feof(f_3d)){ 
 		printf("Open6\n");
 		fscanf(f_3d, "%s %s %s", s1, s2, s3);
 		printf("Open7\n");
		v.push_back(atof(s1));
		printf("Open8\n");
		v.push_back(atof(s2));
		printf("Open9\n");
		v.push_back(atof(s3));
		printf("Open3\n");
	
		fscanf(f_3d, "%s %s %s", s4, s5, s6);
		n.push_back(atof(s4));
		n.push_back(atof(s5));
		n.push_back(atof(s6));	
		printf("Open4\n");
	}
	printf("Open5\n");
 	fclose(f_3d);
 	printf("Close\n");
 	return;
}




/**
*
*/
void color(XMLElement* element2) {
 int r=0, g=0, b=0;
 if(!(r = element2->IntAttribute("R"))) r = r%256;
 if(!(g = element2->IntAttribute("G"))) g = g%256;
 if(!(b = element2->IntAttribute("B"))) b = b%256;
 Transformacao* tf = new Color(r,g,b);
 transformacoes.push_back(tf);
}




/**
*
*/
std::vector<float> processPointTranslate(XMLElement *element2) {
	std::vector<float> v;
	element2 = element2->FirstChildElement("point");
	v.push_back(element2->FloatAttribute("X"));
	v.push_back(element2->FloatAttribute("Y"));
	v.push_back(element2->FloatAttribute("Z"));
	element2 = element2->NextSiblingElement("point");
	while(element2) {
		v.push_back(element2->FloatAttribute("X"));
		v.push_back(element2->FloatAttribute("Y"));
		v.push_back(element2->FloatAttribute("Z"));
		element2 = element2->NextSiblingElement("point");
	}
	return v;
}




/**
*
*/
void translate(XMLElement* element2) {
	float time;
	Transformacao* tf;
	if((time = element2->FloatAttribute("time"))) {
		//Existe tempo, falta processar point
		std::vector<float> pontos;
		pontos = processPointTranslate(element2);
		tf = new Translate(time, pontos);
	}
	else {
		if(!(x = element2->FloatAttribute("X"))) x = 0;
		if(!(y = element2->FloatAttribute("Y"))) y = 0;
		if(!(z = element2->FloatAttribute("Z"))) z = 0;
		tf = new Translate(x,y,z);
	}
 	transformacoes.push_back(tf);
}




/**
*
*/
void rotate(XMLElement* element2) {
 float ang, time;
 Transformacao* tf;
 if(!(x = element2->FloatAttribute("X"))) x=0;
 if(!(y = element2->FloatAttribute("Y"))) y=0;
 if(!(z = element2->FloatAttribute("Z"))) z=0;
 if(!(ang = element2->FloatAttribute("angle"))) ang = 0;
 if(!(time = element2->FloatAttribute("time"))) time = 0;
 if(ang != 0) tf = new Rotate(ang, x, y, z);
 else tf = new Rotate(time, x, y, z, true);
 transformacoes.push_back(tf);
}




/**
*
*/
void scale(XMLElement* element2) {
 if(!(x = element2->FloatAttribute("X"))) x=1;
 if(!(y = element2->FloatAttribute("Y"))) y=1;
 if(!(z = element2->FloatAttribute("Z"))) z=1;
 Transformacao* tf = new Scale(x,y,z);
 transformacoes.push_back(tf);
}




/**
*
*/
void model(XMLElement* element2) {
	XMLElement* tftemp = element2;
	float r=0,g=0,b=0;
	char* nome, *texture;
	std::vector<float> vc, norm;
	while(tftemp != NULL) {
		nome = strdup((char*) tftemp->Attribute("file"));
	 	open3dModel(nome, vc, norm);
		if(vc.size() == 0) {
			printf("Error opening %s!\n", nome);
			return;
		}
		printf("Opened %s successfully.\n", nome);
		if(!(tftemp->Attribute("texture"))){
			r = tftemp->FloatAttribute("diffR");
			g = tftemp->FloatAttribute("diffG");
			b = tftemp->FloatAttribute("diffB");
			Transformacao* tf = new Model(vc,norm,r,g,b);
			transformacoes.push_back(tf);
		}
		else{
			texture = strdup((char*) tftemp->Attribute("texture"));
			Transformacao* tf = new Model(vc, norm,texture);
			transformacoes.push_back(tf);
		}
		tftemp = tftemp->NextSiblingElement("model");
	}
}




/**
*
*/
void popMatrix() {
 Transformacao* tf = new PopMatrix();
 transformacoes.push_back(tf);
}




/**
*
*/
void pushMatrix() {
 Transformacao* tf = new PushMatrix();
 transformacoes.push_back(tf);
}



void light(XMLElement* element2){
	char* type;
	if(!(x = element2->FloatAttribute("posX"))) x=0;
 	if(!(y = element2->FloatAttribute("posY"))) y=0;
 	if(!(z = element2->FloatAttribute("posZ"))) z=0;
 	type = strdup((char*) element2->Attribute("type"));
 	Transformacao* tf = new Light(x,y,z, type);
 	transformacoes.push_back(tf);
}


int parserXMLLight(XMLElement* pListElement) {
 XMLElement* element2;
 XMLElement* tempEl;

 if(pListElement != NULL) {
	 element2 = pListElement->FirstChildElement("light");
	 while(element2!=NULL){	
	 	light(element2);
	 	element2 = element2 -> NextSiblingElement("light");
	 }
 }
 return 1;
}


/**
*
*/
int parserXMLGroup(XMLElement* pListElement) {
 XMLElement* element2;
 XMLElement* tempEl;

 if(pListElement != NULL) {
	 element2 = pListElement->FirstChildElement("color");
	 if(element2!=NULL) color(element2);
	 element2 = pListElement->FirstChildElement("translate");
	 if(element2!=NULL) translate(element2);
	 element2 = pListElement->FirstChildElement("rotate");
	 if(element2!=NULL) rotate(element2);
	 element2 = pListElement->FirstChildElement("scale");
	 if(element2!=NULL) scale(element2);
	 element2 = pListElement->FirstChildElement("model");
	 if(element2!=NULL) model(element2);
	 tempEl = pListElement->FirstChildElement("group");
	 if(tempEl != NULL) parserXMLGroup(tempEl);

	 tempEl = pListElement->NextSiblingElement("group");
	 if(tempEl != NULL) parserXMLGroup(tempEl);
 }
 return 1;
}




/**
* 	This function implements the tinyxml2 API, and it's used at program
*	startup when we are begining to process the XML file. This function is
* responsible for invoking the open3dModel function, since this will extract
*	the filenames where to find the 3d files.
*		It checks for the rootElement, in our case the scene, then it iterates
*	for the first group, running the parserXML which will parse the XML file
* indicated by "file", checking for error codes while doing so.
*
*	@param file - Character array with the path to the XML file
* @return int - Integer associated to an errorCode
*/
int processXML(char* file) {
 XMLDocument xmlDoc;
 XMLNode *root;
 XMLElement *pListElement;
 if(xmlDoc.LoadFile(file) != XML_SUCCESS) {
	 printf("Error loading %s.\n", file);
	 return -1;
 }
 root = xmlDoc.FirstChildElement("scene");
 if((pListElement = root -> FirstChildElement("lights")))
 	parserXMLLight(pListElement);

 if((pListElement = root -> FirstChildElement("group")))
 	parserXMLGroup(pListElement);

 return XML_SUCCESS;
}




/**
* 	This function is only invoked either by the glutMainLoop or by a user
*	key press / mouse interaction. What this function does is render the scene
*	specified in the program input XML file.
*		This function sets up the camera position, view point. It also renders
* rotations and translations that the user might have done to the scenery.
*	It then checks how the user wants to render the images, either by filling
* them, seeing only the lines or the points. Finally it calls the renderFigures
* function.
*
*	@param void
* @return void
*/
void renderScene(void) {
 glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

 glLoadIdentity();
 gluLookAt(camX, camY, camZ,
					 0.0f, 0.0f, 0.0f,
					 0.0f, 1.0f, 0.0f);

 glTranslatef(vert_trans, 0, hori_trans);
 glRotatef(vert_rot, 1, 0, 0);
 glRotatef(hori_rot, 0, 1, 0);

 glEnable(GL_CULL_FACE);
 glCullFace(GL_BACK);
 glFrontFace(GL_CCW);
 glPolygonMode(GL_FRONT, view_mode == 'f' ? GL_FILL :
													 view_mode == 'l' ? GL_LINE :
														 GL_POINT);
 renderFigures();
 showFPS();

 glutSwapBuffers();
}




/**
* 	This function is only invoked by render scene, and it's sole purpose
*	is to render every triangle that has been previously saved to memory.
*	It's important to denote that this render is made using tri-vertex
* aggregation to triangles.
*		It acesses the global vectors that we defined to store every triangle
* and transformation, making sure to draw the correct coordinates.
*		It doesn't draw directly on screen but calls a function that does so.
*
* @return void
*/
void renderFigures() {
 int color;
 std::vector<float> vc;
 Transformacao* tftemp;
 glLineWidth(1.0f);
 for(int i = 0; i < transformacoes.size(); i++) {
	 tftemp = transformacoes.at(i);
	 tftemp->transformar();
 }
}




/**
* 	This function is invoked everytime the viewing window is reshaped.
*		It checks if the window is too short, preventing the user to make
*	a small window. It then sets the correct parameters for the renderization
* to take place.
*
* @param w - Integer representing the new window width
* @param h - Integer representing the new window height
* @return void
*/
void changeSize(int w, int h) {
 // Prevent a divide by zero, when window is too short
 // (you cant make a window with zero width).
 if(h == 0) h = 1;
 // compute window's aspect ratio
 float ratio = w * 1.0 / h;
 // Set the projection matrix as current
 glMatrixMode(GL_PROJECTION);
 // Load Identity Matrix
 glLoadIdentity();
 // Set the viewport to be the entire window
 glViewport(0, 0, w, h);
 // Set perspective
 gluPerspective(45.0f ,ratio, 1.0f ,50000.0f);
 // return to the model view matrix mode
 glMatrixMode(GL_MODELVIEW);
}




/**
* 	This function is invoked only when we need to convert the camera
* coordinates to a cartesian format. It does so by setting new values
* to the global variables representing the camera position.
*
* @return void
*/
void spherical2Cartesian() {
 camX = radius * cos(beta) * sin(alfa);
 camY = radius * sin(beta);
 camZ = radius * cos(beta) * cos(alfa);
}




/**
* 	This function is invoked only when mouse moves on screen. It receives
*	two valus representing the current mouse position.
*		The movement_handler function makes sure that we apply the correct
*	rotation to the scene when we drag the mouse arrow, it updates the final
* mouse position once it's done ajusting the rotation for the next render
* invocation.
*
* @param x - Integer representing the mouse x position
* @param y - Integer representing the mouse y position
* @return void
*/
void movement_handler(int x, int y) {
 if (click) {
	 hori_rot -= (x_pos - x) / 2;
	 vert_rot -= (y_pos - y) / 2;
	 x_pos = x;
	 y_pos = y;

	 //Re-render
	 glutPostRedisplay();
 }
}




/**
* 	This function is invoked only when mouse buttons are pressed. It receives
*	an indicator of which key, and the state of it (Down meaning it's being pressed
*	and Down meaning it's not).
*		This function arranges with three variables, it sets the (click) variable
*	either to true or false meaning that the mouse is or is not pressed. It also
*	updates the coordenates of the mouse, so that we can track it's movement.
*
*	@param button - Integer representing the mouse button that is being pressed
* @param state - Integer representation of Press value (GLUT_UP | GLUT_DOWN)
* @param x - Integer representing the mouse x position
* @param y - Integer representing the mouse y position
* @return void
*/
void mouse_handler(int button, int state, int x, int y) {
 if (button == GLUT_LEFT_BUTTON) {
	 if (state == GLUT_DOWN) {
		 click = true;
		 x_pos = x;
		 y_pos = y;
	 }
	 if (state == GLUT_UP) click = false;
 }
}




/**
* 	This function handles a special key press, changing the right variables
*	to obtain the desired results.
*		We use a global variable (radius) to assure thar the user can zoom in
* or zoom out as they wish to. We impose a limit of 250, so that it won't be
* possible to get closer to the earth than this.
*
*	@param key - Character representing the special key pressed
* @param x - Integer representing the mouse x position
* @param y - Integer representing the mouse y position
* @return void
*/
void special_key_handler(int key, int x, int y) {
 switch(key) {
	 case GLUT_KEY_PAGE_DOWN:
		 radius -= 20.0f;
		 break;
	 case GLUT_KEY_PAGE_UP:
		 radius += 20.0f;
		 break;
	 case GLUT_KEY_UP:
		 hori_trans += 20.0f;
		 break;
	 case GLUT_KEY_DOWN:
		 hori_trans -= 20.0f;
		 break;
	 case GLUT_KEY_RIGHT:
		 vert_trans -= 20.0f;
		 break;
	 case GLUT_KEY_LEFT:
		 vert_trans += 20.0f;
		 break;
 }
 spherical2Cartesian();
 glutPostRedisplay();
}




/**
* 	This function handles a normal key press, changing the right variables
*	to obtain the desired results.
*		We use a global variable (view_mode) to indicate the key that the user
*	pressed, and then we can render the desired scenes accordingly. This
* variable stores an 'f' if the user wants to see the object filled, an 'l'
* if we want to see only lines and finnaly a 'p' if we only want to render
* points.
*
*	@param c - Character representing the key pressed (non special keys)
* @param x - Integer representing the mouse x position
* @param y - Integer representing the mouse y position
* @return void
*/
void normal_key_handler(unsigned char c, int x, int y) {
 switch (c) {
	 case 'f':
	 case 'F':
		 view_mode = 'f';
		 break;
	 case 'l':
	 case 'L':
		 view_mode = 'l';
		 break;
	 case 'p':
	 case 'P':
		 view_mode = 'p';
		 break;
 }

 //Re-render
 glutPostRedisplay();
}



void showFPS() {
  float fps;
	int time;
	char s[64];
  frame++;
	time = glutGet(GLUT_ELAPSED_TIME);
	if (time - timebase > 1000) {
		fps = frame*1000.0/(time-timebase);
		timebase = time;
		frame = 0;
		sprintf(s, "Rendering scene at %.2f FPS", fps);
		glutSetWindowTitle(s);
	}
}

/*
void loadTexture(char *) {

	unsigned int t, tw, th;
	unsigned char *texData;
	ilGenImages(1, &t);
	ilBindImage(t);
	ilLoadImage(imagem);
	tw = ilGetInteger(IL_IMAGE_WIDTH);
	th = ilGetInteger(IL_IMAGE_HEIGHT);
	ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);
	texData = ilGetData();

	glGenTextures(1, &texture);

	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tw, th, 0, GL_RGBA, GL_UNSIGNED_BYTE, texData);
	glGenerateMipmap(GL_TEXTURE_2D);
}

void init(char *imagem) {

	alpha = 0.0;
	beta = 0.0;

	unsigned int ima[1];

	ilInit();
	ilGenImages(1, ima);
	ilBindImage(ima[0]);
	ilLoadImage(imagem);
	ilConvertImage(IL_LUMINANCE, IL_UNSIGNED_BYTE);

	imageWidth = ilGetInteger(IL_IMAGE_HEIGHT);
	imageData = ilGetData();
	printf("%d\n", imageWidth);

	prepareTerrain();

	loadTexture(imagem);
	
	glEnable(GL_TEXTURE_2D);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

}
*/