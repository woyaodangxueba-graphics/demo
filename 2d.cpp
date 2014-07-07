// SimpleProgram.cpp
//
// A simple 2D OpenGL program

#define GL3_PROTOTYPES

// Include the vector and matrix utilities from the textbook, as well as some
// macro definitions.
#include "../include/Angel.h"

#ifdef __APPLE__
#  include <OpenGL/gl3.h>
#endif

#include <stdio.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
// A global constant for the number of points that will be in our object.
/*const int NumPoints = 5;*/

const int grid_x = 181;
const int grid_y = 121;
GLuint vao[2];
GLuint c_linepoints_num;


using namespace std;

//----------------------------------------------------------------------------
// the HSV color model will be as follows
// h : [0 - 360]
// s : [0 - 1]
// v : [0 - 1]
// If you want it differently (in a 2 * pi scale, 256 instead of 1, etc,
// you'll have to change it yourself.
// rgb is returned in 0-1 scale (ready for color3f)
void HSVtoRGB(float hsv[3], float rgb[3]) {
	float tmp1 = hsv[2] * (1-hsv[1]);
	float tmp2 = hsv[2] * (1-hsv[1] * (hsv[0] / 60.0f - (int) (hsv[0]/60.0f) ));
	float tmp3 = hsv[2] * (1-hsv[1] * (1 - (hsv[0] / 60.0f - (int) (hsv[0]/60.0f) )));
	switch((int)(hsv[0] / 60)) {
		case 0:
			rgb[0] = hsv[2] ;
			rgb[1] = tmp3 ;
			rgb[2] = tmp1 ;
			break;
		case 1:
			rgb[0] = tmp2 ;
			rgb[1] = hsv[2] ;
			rgb[2] = tmp1 ;
			break;
		case 2:
			rgb[0] = tmp1 ;
			rgb[1] = hsv[2] ;
			rgb[2] = tmp3 ;
			break;
		case 3:
			rgb[0] = tmp1 ;
			rgb[1] = tmp2 ;
			rgb[2] = hsv[2] ;
			break;
		case 4:
			rgb[0] = tmp3 ;
			rgb[1] = tmp1 ;
			rgb[2] = hsv[2] ;
			break;
		case 5:
			rgb[0] = hsv[2] ;
			rgb[1] = tmp1 ;
			rgb[2] = tmp2 ;
			break;
		default:
		 cout << "Inconceivable!\n";
		
	}
    
}

GLfloat find_large(GLfloat *arr, int arr_num){
	GLfloat temp = arr[0];
	for(int i=0;i<arr_num;i++){
		if (temp<arr[i])
		temp = arr[i];
	}
	return temp;
}

GLfloat find_small(GLfloat *arr, int arr_num){
	GLfloat temp = arr[0];
	for(int i=0;i<arr_num;i++){
		if (temp>arr[i]&&arr[i]!=0.0)
		temp = arr[i];
	}
	return temp;
}
//----------------------------------------------------------------------------
void init(int argc, char** argv)
{
	//read file name from commander arguments
	const char* filename = argv[1];
	//givn a 180*120 dataset, the vertices would be 181*121
	GLfloat data_color[grid_x*grid_y];
	//read file to our vertices grid
	fstream tempdata;
	char linebuffer[4096];
	tempdata.open(filename);

	int color_i=0;

	while(!tempdata.eof()){
		stringstream streamdata;
		tempdata.getline(linebuffer,4096);
		if(linebuffer[0]=='\0'||linebuffer[0]=='#')
			continue;
		streamdata.str(linebuffer);
		while(!streamdata.eof()){
			streamdata.getline(linebuffer,4096,' ');
			data_color[color_i] = atof(linebuffer);
			color_i++;
		}
	}
//  check read stuff
	// for(int i =180;i<500;i++){printf("%f ",data_color[i]);}
	// printf("\n");

	// fill grid with points positions
	vec2 imagegrid[grid_x*grid_y];
	for (int i = 0; i < grid_y; i++)
		for (int j = 0; j < grid_x; j++)
			imagegrid[i*grid_x+j] = vec2(-1.0*.95+j*(.9*2.0/180.0),-1.0*.95+i*(.9*2.0/120.0));

	//create index for triangles in a 2*2 prototype we create 2 triangles with 6 indices.
	//we now have 6 times indices to the vertiecs we have. and store it in a indices[] array.
	const int length = (grid_x-1)*(grid_y-1)*6;
	GLuint *indices = new GLuint[length];

	for(int i=0;i<length; i=i+6){
		int k;
		k = i/6;
		if(k%grid_x==grid_x-1)
			continue;
		else{
		indices[i] = k;
		indices[i+1] = k+1;
		indices[i+2] = k+grid_x;
		indices[i+3] = indices[i+1];
		indices[i+4] = indices[i+2];
		indices[i+5] = k+grid_x+1;
		}
	}

//	get buckets No. from command arguments
	GLint numberBuckets = atoi(argv[2]);
// two funtions that work to find smallest data from our raw dataset and the largest.
	GLfloat largestTempValue = find_large(data_color, grid_x*grid_y);
	GLfloat smallestTempValue = find_small(data_color, grid_x*grid_y);
	
//check returns good
	//cout <<"largest"<< largestTempValue << "\n";
	//cout <<"smallest"<< smallestTempValue << "\n";
	
	// now it is the step that discretized the raw dataset and assign them with a HSV color.
	vec3 dis_color[grid_x*grid_y];
	for(int i=0;i<grid_x*grid_y;i++){
		if (data_color[i]==0.0)
			dis_color[i] = vec3(0.0,0.0,1.0);//white, need a change?
			
		else dis_color[i]= vec3((240.0-(int(( (data_color[i]-smallestTempValue)/(largestTempValue-smallestTempValue) * (numberBuckets - 1)  + 0.5 ))*(240/numberBuckets))),1.0,1.0);
	}
	
	vec3 rgb_color[grid_x*grid_y];
// now we translate the HSV color from dis_color[] to RGB color and store them in rgb_color[]
	for(int i=0;i<grid_x*grid_y;i++){
		float temp1[3] = {dis_color[i].x,dis_color[i].y,dis_color[i].z};
		float temp2[3];
		HSVtoRGB(temp1,temp2);
		rgb_color[i].x = temp2[0];
		rgb_color[i].y = temp2[1];
		rgb_color[i].z = temp2[2];
	}
    
    // Create a vertex array object---OpenGL needs this to manage the Vertex
	// we did it in a global method that saves us time to pass it between functions which is much complex
    // Buffer Object
    // Generate the vertex array and then bind it to make make it active.
    glGenVertexArrays(2, vao);
    glBindVertexArray(vao[0]);

    // Create and initialize a buffer object---that's the memory buffer that
    // will be on the card!
 	GLuint buffer[5];

    // We only need one for this example.
    glGenBuffers(5, buffer);

    // Bind makes it the active VBO
    glBindBuffer(GL_ARRAY_BUFFER, buffer[0]);

    // Here we copy the vertex data into our buffer on the card.  The parameters
    // tell it the type of buffer object, the size of the data in bytes, the
    // pointer for the data itself, and a hint for how we intend to use it.
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec2)*grid_x*grid_y, imagegrid, GL_STATIC_DRAW);

	//indices buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, (grid_x-1)*(grid_y-1)*6*sizeof(GLuint), indices, GL_STATIC_DRAW);

    // Load the shaders.  Note that this function is not offered by OpenGL
    // directly, but provided as a convenience.
    GLuint program = InitShader("../SimpleProgram/src/vshader32.glsl", 
								"../SimpleProgram/src/fshader32.glsl");

    // Make that shader program active.
    glUseProgram(program);

    // Initialize the vertex position attribute from the vertex shader.  When
    // the shader and the program are linked, a table is created for the shader
    // variables.  Here, we get the index of the vPosition variable.
    GLuint loc = glGetAttribLocation(program, "vPosition");

    // We want to set this with an array!
    glEnableVertexAttribArray(loc);

    // We map it to this offset in our current buffer (VBO) So, our position
    // data is going into loc and contains 2 floats.  The parameters to this
    // function are the index for the shader variable, the number of components,
    // the type of the data, a boolean for whether or not this data is
    // normalized (0--1), stride (or byte offset between consective attributes),
    // and a pointer to the first component.  Note that BUFFER_OFFSET is a macro
    // defined in the Angel.h file.
	glVertexAttribPointer(loc, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
 
 // Now repeat lots of that stuff for the colors
    glBindBuffer(GL_ARRAY_BUFFER, buffer[2]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(rgb_color), rgb_color, GL_STATIC_DRAW);
    
    GLuint colorLoc = glGetAttribLocation(program, "vColor");
    glEnableVertexAttribArray(colorLoc);
    glVertexAttribPointer(colorLoc, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	
	//contour drawing
	// initial linepoints num and trianglespoints and num.
	// traverse dis_color[] to count the numbers in case we can allocate space to store them in arrays.
		c_linepoints_num = 0;

	 for(int i=0;i<grid_x*(grid_y-1); i++){
			// skip the last vertex in a line, it would cause a x-coor through lines or triangles
		 if(i%grid_x==grid_x-1){
			 //i++;
			continue;
		 }
		else{
				//determin lines
			if(dis_color[i].x!=dis_color[i+1].x)
				c_linepoints_num+=2;
			if(dis_color[i+1].x!=dis_color[i+grid_x+1].x)
				c_linepoints_num+=2;
			if(dis_color[i+grid_x+1].x!=dis_color[i+grid_x].x)
				c_linepoints_num+=2;
			if(dis_color[i+grid_x].x!=dis_color[i].x)
				c_linepoints_num+=2;
			//&&(dis_color[i+1].x!=0.0)&&(dis_color[i+grid_x+1].x!=0.0
			/*if(i%grid_x==0)
				c_linepoints_num+=2;*/
			}
	}

	// now we can create these vertex arrays
	vec2 *c_linepoints = new vec2[c_linepoints_num];
	
	//init two pointers in array as we will go through two large matrices
	 int c_linepoints_ptr = 0;

	 for(int i=0;i<grid_x*(grid_y-1); i++){

		 if(i%grid_x==grid_x-1){
				//i++;
			 continue;
		 }
		 else{
				//determin lines
				if(dis_color[i].x!=dis_color[i+1].x)
					{
						c_linepoints[c_linepoints_ptr] = (imagegrid[i] + imagegrid[i+1])*.5;
						c_linepoints[c_linepoints_ptr+1] = (imagegrid[i] + imagegrid[i+grid_x+1])*.5;
						c_linepoints_ptr+=2;
					}
				if(dis_color[i+1].x!=dis_color[i+grid_x+1].x)
					{
						c_linepoints[c_linepoints_ptr] = (imagegrid[i+1] + imagegrid[i+grid_x+1])*.5;
						c_linepoints[c_linepoints_ptr+1] = (imagegrid[i] + imagegrid[i+grid_x+1])*.5;
						c_linepoints_ptr+=2;
					}
				if(dis_color[i+grid_x+1].x!=dis_color[i+grid_x].x)
					{
						c_linepoints[c_linepoints_ptr] = (imagegrid[i+grid_x+1] + imagegrid[i+grid_x])*.5;
						c_linepoints[c_linepoints_ptr+1] = (imagegrid[i] + imagegrid[i+grid_x+1])*.5;
						c_linepoints_ptr+=2;
					}
				if(dis_color[i+grid_x].x!=dis_color[i].x)
					{
						c_linepoints[c_linepoints_ptr] = (imagegrid[i] + imagegrid[i+grid_x])*.5;
						c_linepoints[c_linepoints_ptr+1] = (imagegrid[i] + imagegrid[i+grid_x+1])*.5;
						c_linepoints_ptr+=2;
					}
				//&&(dis_color[i+1].x!=0.0)&&(dis_color[i+grid_x+1].x!=0.0
				/*if(i%grid_x==0)
					{
						c_linepoints[c_linepoints_ptr] = imagegrid[i];
						c_linepoints[c_linepoints_ptr+1] = imagegrid[i+grid_x];
						c_linepoints_ptr+=2;
					}*/
			}		
	}
	
	 vec3 *contourcol = new vec3[c_linepoints_num];
	 for(int i=0;i<c_linepoints_num;i++)
		 contourcol[i]=vec3(0.0,0.0,0.0);
	 
// second vao
	glBindVertexArray(vao[1]);

	
	glBindBuffer(GL_ARRAY_BUFFER, buffer[3]);
// go with a subbufferdata maner, sign c_linepoints and c_trianglepoints.

    glBufferData(GL_ARRAY_BUFFER, sizeof(vec2)*c_linepoints_num, c_linepoints, GL_STATIC_DRAW);
	
// location
	glUseProgram(program);
	GLuint contourloc = glGetAttribLocation(program, "vPosition");

	glEnableVertexAttribArray(contourloc);

	glVertexAttribPointer(contourloc, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
// color
	glBindBuffer(GL_ARRAY_BUFFER, buffer[4]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vec3)*c_linepoints_num, contourcol, GL_STATIC_DRAW);

	GLuint concolorLoc = glGetAttribLocation(program, "vColor");
    glEnableVertexAttribArray(concolorLoc);
    glVertexAttribPointer(concolorLoc, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

    
    

	 // Make the background white
    glClearColor(1.0, 1.0, 1.0, 1.0);


}

//----------------------------------------------------------------------------
void display(void)
{
    // clear the window
    glClear(GL_COLOR_BUFFER_BIT);

    // Draw the temperature map
	glBindVertexArray(vao[0]);
	glDrawElements(GL_TRIANGLES,(grid_x-1)*(grid_y-1)*6,GL_UNSIGNED_INT,0);
	// Draw contours
	glBindVertexArray(vao[1]);
	//our lines
	glDrawArrays(GL_LINES, 0, c_linepoints_num);

    glFlush();
    glutSwapBuffers();
}

//----------------------------------------------------------------------------
void
keyboard(unsigned char key, int x, int y)
{
    switch (key) {

    // Quit when ESC is pressed
    case 27:
        exit(EXIT_SUCCESS);
        break;
    }
}

//------------------------------------------------------------------------------
// This program draws a red rectangle on a white background, but it's still
// missing the machinery to move to 3D.
int main(int argc, char** argv)
{
    glutInit(&argc, argv);
#ifdef __APPLE__
    glutInitDisplayMode(GLUT_3_2_CORE_PROFILE | GLUT_RGBA | GLUT_DOUBLE);
#else
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
    glutInitContextVersion (3, 2);
    glutInitContextFlags (GLUT_FORWARD_COMPATIBLE);
#endif
    glutInitWindowSize(900, 600);
    glutInitWindowPosition(600, 50);
    glutCreateWindow("2d data visualization and contour");
    printf("%s\n%s\n", glGetString(GL_RENDERER), glGetString(GL_VERSION));

	glewExperimental = GL_TRUE;

#ifndef __APPLE__
	glewInit();
#endif

    init(argc, argv);

    //NOTE:  callbacks must go after window is created!!!
    glutKeyboardFunc(keyboard);
    glutDisplayFunc(display);
    glutMainLoop();

    return(0);
}
