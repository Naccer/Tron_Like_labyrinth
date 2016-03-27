#include <glimac/SDLWindowManager.hpp>
#include <GL/glew.h>
#include <iostream>
#include <glimac/Sphere.hpp>
#include <glimac/Program.hpp>
#include <glm/gtc/random.hpp>
#include <ctime>
#include <cmath>
#include <glimac/Image.hpp>
#include "libPPM.h"
#include <glimac/TrackballCamera.hpp>
#include <glimac/FreeflyCamera.hpp>
#include <glimac/tiny_obj_loader.h>
#include <glimac/Geometry.hpp>
#include "Monster.h"
#include "Character.h"
#include <algorithm>
#include <glimac/Sphere.hpp>
#include <vector>
#include <string>
#include <sstream>
//#include "objectLoader.c"

using namespace glimac;
using namespace glm;
using namespace tinyobj;
using namespace std;

bool loadOBJ(
	const char * path,
	std::vector<glm::vec3> & out_vertices,
	std::vector<glm::vec2> & out_uvs,
	std::vector<glm::vec3> & out_normals
){
	printf("Loading OBJ file %s...\n", path);

	std::vector<unsigned int> vertexIndices, uvIndices, normalIndices;
	std::vector<glm::vec3> temp_vertices;
	std::vector<glm::vec2> temp_uvs;
	std::vector<glm::vec3> temp_normals;


	FILE * file = fopen(path, "r");
	if( file == NULL ){
		printf("Impossible to open the file ! Are you in the right path ? \n");
		getchar();
		return false;
	}

	while( 1 ){

		char lineHeader[128];
		// read the first word of the line
		int res = fscanf(file, "%s", lineHeader);
		if (res == EOF)
			break; // EOF = End Of File. Quit the loop.

		// else : parse lineHeader

		if ( strcmp( lineHeader, "v" ) == 0 ){
			glm::vec3 vertex;
			fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z );
			temp_vertices.push_back(vertex);
		}else if ( strcmp( lineHeader, "vt" ) == 0 ){
			glm::vec2 uv;
			fscanf(file, "%f %f\n", &uv.x, &uv.y );
			uv.y = -uv.y; // Invert V coordinate since we will only use DDS texture, which are inverted. Remove if you want to use TGA or BMP loaders.
			temp_uvs.push_back(uv);
		}else if ( strcmp( lineHeader, "vn" ) == 0 ){
			glm::vec3 normal;
			fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z );
			temp_normals.push_back(normal);
		}else if ( strcmp( lineHeader, "f" ) == 0 ){
			std::string vertex1, vertex2, vertex3;
			unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
			int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2] );
			if (matches != 9){
				printf("File can't be read by our simple parser :-( Try exporting with other options\n");
				return false;
			}
			vertexIndices.push_back(vertexIndex[0]);
			vertexIndices.push_back(vertexIndex[1]);
			vertexIndices.push_back(vertexIndex[2]);
			uvIndices    .push_back(uvIndex[0]);
			uvIndices    .push_back(uvIndex[1]);
			uvIndices    .push_back(uvIndex[2]);
			normalIndices.push_back(normalIndex[0]);
			normalIndices.push_back(normalIndex[1]);
			normalIndices.push_back(normalIndex[2]);
		}else{
			// Probably a comment, eat up the rest of the line
			char stupidBuffer[1000];
			fgets(stupidBuffer, 1000, file);
		}

	}

	// For each vertex of each triangle
	for( unsigned int i=0; i<vertexIndices.size(); i++ ){

		// Get the indices of its attributes
		unsigned int vertexIndex = vertexIndices[i];
		unsigned int uvIndex = uvIndices[i];
		unsigned int normalIndex = normalIndices[i];

		// Get the attributes thanks to the index
		glm::vec3 vertex = temp_vertices[ vertexIndex-1 ];
		glm::vec2 uv = temp_uvs[ uvIndex-1 ];
		glm::vec3 normal = temp_normals[ normalIndex-1 ];

		// Put the attributes in buffers
		out_vertices.push_back(vertex);
		out_uvs     .push_back(uv);
		out_normals .push_back(normal);

	}

	return true;
}


glm::mat4 scaling(float sx, float sy, float sz) 
{
	mat4 S =  glm::mat4(vec4(sx, 0, 0, 0), vec4(0, sy, 0, 0), vec4(0, 0, sz, 0), vec4(0, 0, 0, 1));
	return S;
}
struct Vertex {
        glm::vec3 m_Position;
        glm::vec3 m_Normal;
        glm::vec2 m_TexCoords;
    };
 
////////////LOAD OBJECTS////////////////////////////   
void load_obj(const char* filename, vector<glm::vec4> &vertices, vector<glm::vec3> &normals, vector<GLushort> &elements)
{
    ifstream in(filename, ios::in);
    if (!in)
    {
        cerr << "Cannot open " << filename << endl; exit(1);
    }

    string line;
    while (getline(in, line))
    {
        if (line.substr(0,2) == "v ")
        {
            istringstream s(line.substr(2));
            glm::vec4 v; s >> v.x; s >> v.y; s >> v.z; v.w = 1.0f;
            vertices.push_back(v);
        }
        else if (line.substr(0,2) == "f ")
        {
            istringstream s(line.substr(2));
            GLushort a,b,c;
            s >> a; s >> b; s >> c;
            a--; b--; c--;
           elements.push_back(a); elements.push_back(b); elements.push_back(c);
        }
        else if (line[0] == '#')
        {
            /* ignoring this line */
        }
        else
        {
            /* ignoring this line */
        }
    }

    normals.resize(vertices.size(), glm::vec3(0.0, 0.0, 0.0));
    for (int i = 0; i < elements.size(); i+=3)
    {
        GLushort ia = elements[i];
        GLushort ib = elements[i+1];
        GLushort ic = elements[i+2];
        glm::vec3 normal = glm::normalize(glm::cross(
        glm::vec3(vertices[ib]) - glm::vec3(vertices[ia]),
        glm::vec3(vertices[ic]) - glm::vec3(vertices[ia])));
        normals[ia] = normals[ib] = normals[ic] = normal;
    }
}

void randomPosition(int *map, int lines, int columns, int numberOfPositions, int *indicesPositions) {

	std::vector<int> a;
	a.resize(lines*columns);
	
	for (int i = 0; i < lines*columns; i++)
		a[i] = i;
	
	std::random_shuffle ( a.begin(), a.end());

	int j = 0;
	
	for (int i = 0; (i < lines*columns) && (j < numberOfPositions); i++) {
		//std::cout << a[i] << " " << map[a[i]] << "\t";
		if (map[a[i]] == 1) 
			indicesPositions[j++] = a[i];
		//std::cout << j << " " << indicesPositions[j-1] << " " << a[i] << std::endl;
	}
}

glm::vec2 ind2sub(int i, int lines, int columns) {

	std::cout << i << " " << lines << " " << columns << " " << i%lines << " " << i/lines << std::endl;

	return vec2(i%lines, i/lines);
}


glm::mat4 rotateAlongX(float b)
{
	float a = radians(b);
	glm::mat4 R =  glm::mat4(vec4(1, 0, 0, 0), vec4(0, cos(a), -sin(a), 0), vec4(0, sin(a), cos(a), 0), vec4(0, 0, 0, 1));
	return R;
}

glm::mat4 rotateAlongY(float b)
{
	float a = radians(b);
	glm::mat4 R =  glm::mat4(vec4(cos(a), 0, sin(a), 0), vec4(0, 1, 0, 0), vec4(-sin(a), 0, cos(a), 0), vec4(0, 0, 0, 1));
	return R;
}


int main(int argc, char** argv) 
{

	cout << "Would you like the see the player movements to be performed automatically? [y/n]: ";
	bool execute_automat = true;
	char answer;
	cin >> answer;
	cout << answer << endl;
	
	if (answer == 'n' || answer == 'N')
	{	
		cout << "false" << endl;
		execute_automat = false;
	}

	///////////////////////LIGHT////////////////////////////
	int instanceCount = 2500;
	int pointLightCount = 0;
	int directionalLightCount = 1;
	int spotLightCount = 1;
	float speed = 1.0;
	////////////////////////////////////////////////////////

    	// Initialize SDL and open a window
    	SDLWindowManager windowManager(800, 600, "GLImac");

    	// Initialize glew for OpenGL3+ support
    	GLenum glewInitError = glewInit();
    	if(GLEW_OK != glewInitError) {
        	std::cerr << glewGetErrorString(glewInitError) << std::endl;
        	return EXIT_FAILURE;
    	}

    	std::cout << "OpenGL Version : " << glGetString(GL_VERSION) << std::endl;
    	std::cout << "GLEW Version : " << glewGetString(GLEW_VERSION) << std::endl;
    
    	FilePath applicationPath(argv[0]);
		Program program = loadProgram(applicationPath.dirPath() + "shaders/tron.vert",
    	applicationPath.dirPath() + "shaders/tron.frag");
		program.use();

	Program programSky = loadProgram(applicationPath.dirPath() + "shaders/sky.vert",
    	applicationPath.dirPath() + "shaders/sky.frag");
		//programSky.use();
    	/*********************************
     	* HERE SHOULD COME THE INITIALIZATION CODE
     	*********************************/
      
    	//Load the scenario from a PPM file and fill the array labyrinth with the information about the maze

    	PPMImage *image =  readPPM("Tron_Labirynth/Project/images/maze_ex.ppm");
    	int lines = image -> lines;
    	int columns = image -> columns;
    	int labyrinth[lines*columns];
		
		std::cout << "lines " << lines << "\tcolumns " << columns << std::endl;
		
    	int initialLine= 0;
    	int initialColumn = 0;

    	float posX= 0.0;
    	float posY = 0.0;
    	char direction = 'n';

    	int finalLine= 0;
    	int finalColumn = 0;

		//Fill the array 'labyrinth' and find its initial and final positions
    	for (int i = 0; i < lines; i++)
    	{
			for (int j = 0; j < columns; j++)
			{
				labyrinth[i*columns+j] = (image -> matrix[i*columns+j]);
				//The value '2' represents the initial position of the here
				if (labyrinth[i*columns+j] == 2)
        		{
					initialLine = i;
        	       	initialColumn = j;
					posX = i;
        	        posY = j;
				}
				//The value '3' represents the exit of the labyrinth
				else if (labyrinth[i*columns+j] == 3)
        		{
					finalLine = i;
        	        finalColumn = j;
				}
			}	
    	}
		
    	//Create the hero and define its attributes
     	Character hero(vec2(posX, posY), direction, "Hero", 10, 1, "null");

	//Create random balls
	int numberOfBalls = 10;
	vec3 listBalls[numberOfBalls];
	for (int i = 0; i < numberOfBalls; i++)
	{
	 	float posX = 12;
		float posY = 14.0;
		float posZ = 0.0;
		//cout << "posX and posY : " << posX << " " << posY << endl;
		const vec3 pos = vec3(posX, posY, posZ);
		listBalls[i] = pos;
	}
		
	//Create the array of monsters
	int numberOfMonsters = 5;
	Monster listOfMonsters[numberOfMonsters];
	//Monster monster0(0, vec2(12, 6), "monster1", 0, 3, 1, "null");
	Monster monster0(0, vec2(11, 4), "monster1", 0, 3, 1, "null");
	Monster monster1(0, vec2(0, 0), "monster2", 1, 3, 1, "null");
	Monster monster2(0, vec2(10, 4), "monster3", 2, 3, 1, "null");
	Monster monster3(0, vec2(7, 6), "monster3", 2, 3, 1, "null");
	Monster monster4(0, vec2(13, 15), "monster3", 2, 3, 1, "null");
	listOfMonsters[0] = monster0;
	listOfMonsters[1] = monster1;
	listOfMonsters[2] = monster2;
	listOfMonsters[3] = monster1;
	listOfMonsters[4] = monster2;
		
	//Define the position of the doors
	int numberOfDoors = 0;
	//int doorPositions[] = {12, 6};
	int doorPositions[numberOfDoors];
	
	randomPosition(labyrinth, lines, columns, numberOfDoors, doorPositions);



		for (int i = 0; i < numberOfDoors; i++)
		{
			//std::cout << labyrinth[doorPositions[i]] << " -> ";
			//labyrinth[doorPositions[i]*columns+doorPositions[i+1]] = 5;

			labyrinth[doorPositions[i]] = 5;
			//std::cout << doorPositions[i] << " -> " << ind2sub(doorPositions[i], lines, columns) << std::endl;
			//std::cout << labyrinth[doorPositions[i]] << std::endl;
		}
		//return EXIT_SUCCESS;
		
		//Define the cube attributes
    	int cube_triangleCount = 12;
    	int cube_triangleList[] = {0, 1, 2, 2, 1, 3, 4, 5, 6, 6, 5, 7, 8, 9, 10, 10, 9, 11, 12, 13, 14, 14, 13, 15, 16, 17, 18, 19, 17, 20, 21, 	22, 23, 24, 25, 26, };
    	float cube_uvs[] = {0.f, 0.f, 0.f, 1.f, 1.f, 0.f, 1.f, 1.f, 0.f, 0.f, 0.f, 1.f, 1.f, 0.f, 1.f, 1.f, 0.f, 0.f, 0.f, 1.f, 1.f, 0.f, 1.f, 		1.f, 0.f, 0.f, 0.f, 1.f, 1.f, 0.f, 1.f, 1.f, 0.f, 0.f, 0.f, 1.f, 1.f, 0.f,  1.f, 0.f,  1.f, 1.f,  0.f, 1.f,  1.f, 1.f,  0.f, 0.f, 0.f, 		0.f, 1.f, 1.f,  1.f, 0.f,  };
    	float cube_vertices[] = {-0.5, -0.5, 0.5, 0.5, -0.5, 0.5, -0.5, 0.5, 0.5, 0.5, 0.5, 0.5, -0.5, 0.5, 0.5, 0.5, 0.5, 0.5, -0.5, 0.5, 		-0.5, 0.5, 0.5, -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, -0.5, -0.5, 0.5, -0.5, -0.5, -0.5, -0.5, -0.5, 0.5, -0.5, -0.5, -0.5, 		-0.5, 0.5, 0.5, -0.5, 0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, -0.5, -0.5, -0.5, -0.5, -0.5, -0.5, 	0.5, -0.5, 0.5, -0.5, -0.5, 0.5, -0.5, -0.5, -0.5, 0.5, -0.5, 0.5, 0.5 };
    	float cube_normals[] = {0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0, 0, -1, 	0, -1, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0, 0, 		-1, 0, 0, };

		//Monsters and shot
		Sphere sphere(0.5, 32, 16);

	    ShapeVertex const *vertices = sphere.getDataPointer();
	    
	    GLuint vbo_monster;
	    glGenBuffers(1, &vbo_monster);
	    glBindBuffer(GL_ARRAY_BUFFER, vbo_monster);
	    glBufferData(GL_ARRAY_BUFFER, sphere.getVertexCount() * sizeof(ShapeVertex), vertices, GL_STATIC_DRAW);
	    glBindBuffer(GL_ARRAY_BUFFER, 0);

	    GLuint vao_monster;
	    glGenVertexArrays(1, &vao_monster);
	    glBindBuffer(GL_ARRAY_BUFFER, vbo_monster);
	    glBindVertexArray(vao_monster);
	    const GLuint VERTEX_ATTRIB_POSITION = 0;
	    const GLuint VERTEX_ATTRIB_NORMAL = 1;
	    const GLuint VERTEX_ATTRIB_TEXTURE = 2;
	    glEnableVertexAttribArray(VERTEX_ATTRIB_POSITION);
	    glVertexAttribPointer(VERTEX_ATTRIB_POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(ShapeVertex),
	        (const GLuint *)(offsetof(ShapeVertex, position)));
	    glEnableVertexAttribArray(VERTEX_ATTRIB_NORMAL);
	    glVertexAttribPointer(VERTEX_ATTRIB_NORMAL, 3, GL_FLOAT, GL_FALSE, sizeof(ShapeVertex), 
	        (const GLuint *)(offsetof(ShapeVertex, normal)));
	    glEnableVertexAttribArray(VERTEX_ATTRIB_TEXTURE);
	    glVertexAttribPointer(VERTEX_ATTRIB_TEXTURE, 2, GL_FLOAT, GL_FALSE, sizeof(ShapeVertex),
	        (const GLuint *)(offsetof(ShapeVertex, texCoords)));
	    glBindVertexArray(0);
	    glBindBuffer(GL_ARRAY_BUFFER, 0);

		//Vao cube
		GLuint vao_cube[1];
		glGenBuffers(1, vao_cube);
		GLuint vbo_cube[4];
		glGenBuffers(4, vbo_cube);

		glBindVertexArray(vao_cube[0]);
		// Bind indices and upload data
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_cube[0]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_triangleList), cube_triangleList, GL_STATIC_DRAW);
		// Bind vertices and upload data
		glBindBuffer(GL_ARRAY_BUFFER, vbo_cube[1]);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT)*3, (void*)0);
		glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);
		// Bind normals and upload data
		glBindBuffer(GL_ARRAY_BUFFER, vbo_cube[2]);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT)*3, (void*)0);
		glBufferData(GL_ARRAY_BUFFER, sizeof(cube_normals), cube_normals, GL_STATIC_DRAW);
		// Bind uv coords and upload data
		glBindBuffer(GL_ARRAY_BUFFER, vbo_cube[3]);
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT)*2, (void*)0);
		glBufferData(GL_ARRAY_BUFFER, sizeof(cube_uvs), cube_uvs, GL_STATIC_DRAW);


		//Floor of the scenario
		GLuint vao_floor[1];
		glGenBuffers(1, vao_floor);
		GLuint vbo_floor[4];
		glGenBuffers(4, vbo_floor);

		int plane_triangleCount = 2;
		int plane_triangleList[] = {0, 1, 2, 2, 1, 3}; 
		float plane_uvs[] = {0.f, 0.f, 0.f, 50.f, 50.f, 0.f, 50.f, 50.f};
		float plane_vertices[] = {-500.0, -0.5, 500.0, 500.0, -0.5, 500.0, -500.0, -0.5, -500.0, 500.0, -0.5, -500.0};
		float plane_vertices2[] = {-50.0, 1.0, 50.0, 50.0, 1.0, 50.0, -50.0, 1.0, -50.0, 50.0, 1.0, -50.0};
		float plane_normals[] = {0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0};


		glBindVertexArray(vao_floor[0]);
		// Bind indices and upload data
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_floor[0]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(plane_triangleList), plane_triangleList, GL_STATIC_DRAW);
		// Bind vertices and upload data
		glBindBuffer(GL_ARRAY_BUFFER, vbo_floor[1]);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT)*3, (void*)0);
		glBufferData(GL_ARRAY_BUFFER, sizeof(plane_vertices), plane_vertices, GL_STATIC_DRAW);
		// Bind normals and upload data
		glBindBuffer(GL_ARRAY_BUFFER, vbo_floor[2]);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT)*3, (void*)0);
		glBufferData(GL_ARRAY_BUFFER, sizeof(plane_normals), plane_normals, GL_STATIC_DRAW);
		// Bind uv coords and upload data
		glBindBuffer(GL_ARRAY_BUFFER, vbo_floor[3]);
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT)*2, (void*)0);
		glBufferData(GL_ARRAY_BUFFER, sizeof(plane_uvs), plane_uvs, GL_STATIC_DRAW);
   	
		//Load the object
		// Read our .obj file
		std::vector<glm::vec3> verticesObj;
		std::vector<glm::vec2> uvsObj;
		std::vector<glm::vec3> normalsObj; // Won't be used at the moment.
		bool res = loadOBJ("Tron_Labirynth/Project/ferrari.obj", verticesObj, uvsObj, normalsObj);

		// Load it into a VBO

		GLuint vertexbufferObj;
		glGenBuffers(1, &vertexbufferObj);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbufferObj);
		glBufferData(GL_ARRAY_BUFFER, verticesObj.size() * sizeof(glm::vec3), &verticesObj[0], GL_STATIC_DRAW);

		GLuint uvbufferObj;
		glGenBuffers(1, &uvbufferObj);
		glBindBuffer(GL_ARRAY_BUFFER, uvbufferObj);
		glBufferData(GL_ARRAY_BUFFER, uvsObj.size() * sizeof(glm::vec2), &uvsObj[0], GL_STATIC_DRAW);

		/*//Load object

		Geometry geo;
		geo.loadOBJ("../GLImac-Template/TP_project/alexia.obj", "../GLImac-Template/TP_project/alexia.mtl", true);
		const glimac::Geometry::Vertex* cubeData = geo.getVertexBuffer();

		GLuint vao_obj;
		glGenBuffers(1, &vao_obj);
		GLuint vbo_obj;
		glGenBuffers(1,&vbo_obj);

			glBindBuffer(GL_ARRAY_BUFFER, vbo_obj);
		glBufferData(GL_ARRAY_BUFFER, geo.getVertexCount() * sizeof(glimac::Geometry::Vertex), cubeData, GL_STATIC_DRAW);

		glBindVertexArray(vao_obj);
		const GLuint VERTEX_ATTRIB_POSITION = 0;
		const GLuint VERTEX_ATTRIB_NORMAL = 1;
		const GLuint VERTEX_ATTRIB_TEXTURE = 2;
		glEnableVertexAttribArray(VERTEX_ATTRIB_POSITION);
		glVertexAttribPointer(VERTEX_ATTRIB_POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(glimac::Geometry::Vertex),
		(const GLuint *)(offsetof(glimac::Geometry::Vertex, m_Position)));
		glEnableVertexAttribArray(VERTEX_ATTRIB_NORMAL);
		glVertexAttribPointer(VERTEX_ATTRIB_NORMAL, 3, GL_FLOAT, GL_FALSE, sizeof(glimac::Geometry::Vertex), 
		(const GLuint *)(offsetof(glimac::Geometry::Vertex, m_Normal)));
		glEnableVertexAttribArray(VERTEX_ATTRIB_TEXTURE);
		glVertexAttribPointer(VERTEX_ATTRIB_TEXTURE, 2, GL_FLOAT, GL_FALSE, sizeof(glimac::Geometry::Vertex),
		(const GLuint *)(offsetof(glimac::Geometry::Vertex, m_TexCoords)));
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);  */

		//Monster
		/*GLuint vao_monster[1];
		glGenBuffers(1, vao_monster);
		GLuint vbo_monster[4];
		glGenBuffers(4, vbo_monster);

		glBindVertexArray(vao_monster[0]);
		// Bind indices and upload data
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_monster[0]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_triangleList), cube_triangleList, GL_STATIC_DRAW);
		// Bind vertices and upload data
		glBindBuffer(GL_ARRAY_BUFFER, vbo_monster[1]);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT)*3, (void*)0);
		glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);
		// Bind normals and upload data
		glBindBuffer(GL_ARRAY_BUFFER, vbo_monster[2]);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT)*3, (void*)0);
		glBufferData(GL_ARRAY_BUFFER, sizeof(cube_normals), cube_normals, GL_STATIC_DRAW);
		// Bind uv coords and upload data
		glBindBuffer(GL_ARRAY_BUFFER, vbo_monster[3]);
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT)*2, (void*)0);
		glBufferData(GL_ARRAY_BUFFER, sizeof(cube_uvs), cube_uvs, GL_STATIC_DRAW);*/

		//Shot
		GLuint vao_shot[1];
		glGenBuffers(1, vao_shot);
		GLuint vbo_shot[4];
		glGenBuffers(4, vbo_shot);

		glBindVertexArray(vao_shot[0]);
		// Bind indices and upload data
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_shot[0]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_triangleList), cube_triangleList, GL_STATIC_DRAW);
		// Bind vertices and upload data
		glBindBuffer(GL_ARRAY_BUFFER, vbo_shot[1]);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT)*3, (void*)0);
		glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);
		// Bind normals and upload data
		glBindBuffer(GL_ARRAY_BUFFER, vbo_shot[2]);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT)*3, (void*)0);
		glBufferData(GL_ARRAY_BUFFER, sizeof(cube_normals), cube_normals, GL_STATIC_DRAW);
		// Bind uv coords and upload data
		glBindBuffer(GL_ARRAY_BUFFER, vbo_shot[3]);
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT)*2, (void*)0);
		glBufferData(GL_ARRAY_BUFFER, sizeof(cube_uvs), cube_uvs, GL_STATIC_DRAW);
		
		//Load object
		/*const char* filename = "Tron_Labirynth/Project/ferrari.obj";
		vector<glm::vec4> v;
		vector<glm::vec3> n;
		vector<GLushort> e;
		load_obj(filename, v, n, e);
		
		int object_triangleCount = n.size()/2;
		int object_triangleList[e.size()];
		for (int i = 0; i < e.size(); i++)
		{
			object_triangleList[i] = int(e.at(i));
		}
		
		float object_uvs[v.size()*2];
		float object_vertices[v.size()*3];
		for (int i=0; i < v.size(); i++)
		{
			glm::vec4 vertice = v.at(i);
			object_vertices[i*3] = vertice.x;
			object_vertices[i*3+1] = vertice.y;
			object_vertices[i*3+2] = vertice.z;
			
			//float u = 0.5 + arctang()
		}*/
		/*int plane_triangleCount = 2;
		int plane_triangleList[] = {0, 1, 2, 2, 1, 3}; 
		float plane_uvs[] = {0.f, 0.f, 0.f, 50.f, 50.f, 0.f, 50.f, 50.f};
		float plane_vertices[] = {-500.0, -0.5, 500.0, 500.0, -0.5, 500.0, -500.0, -0.5, -500.0, 500.0, -0.5, -500.0};
		float plane_vertices2[] = {-50.0, 1.0, 50.0, 50.0, 1.0, 50.0, -50.0, 1.0, -50.0, 50.0, 1.0, -50.0};
		float plane_normals[] = {0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0};*/

	    //Load walls texture 
	    std::unique_ptr<Image> pImageWall = loadImage("Tron_Labirynth/Project/images/wall_tron.png");
	    if (pImageWall == NULL) {
		std::cout << "ERROR: loading texture failed." << std::endl;
		return EXIT_FAILURE;
	    } else
		std::cout << "Texture loaded " << pImageWall->getWidth() << " x " << pImageWall->getHeight() 
		    << " at location " << pImageWall->getPixels() << std::endl;

	    GLuint textureWall;
	    glGenTextures(1, &textureWall);
	    glBindTexture(GL_TEXTURE_2D, textureWall);
	    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, pImageWall->getWidth(), pImageWall->getHeight(), 0, 
	    GL_RGBA, GL_FLOAT, (const GLvoid *)(pImageWall->getPixels()));
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	    glBindTexture(GL_TEXTURE_2D, 0);

	    //Load floor texture 
	    std::unique_ptr<Image> pImageFloor = loadImage("Tron_Labirynth/Project/images/floor_tron.png");
	    if (pImageFloor == NULL) {
		std::cout << "ERROR: loading texture failed." << std::endl;
		return EXIT_FAILURE;
	    } else
		std::cout << "Texture loaded " << pImageFloor->getWidth() << " x " << pImageFloor->getHeight() 
		    << " at location " << pImageFloor->getPixels() << std::endl;

	    GLuint textureFloor;
	    glGenTextures(1, &textureFloor);
	    glBindTexture(GL_TEXTURE_2D, textureFloor);
	    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, pImageFloor->getWidth(), pImageFloor->getHeight(), 0, 
	    GL_RGBA, GL_FLOAT, (const GLvoid *)(pImageFloor->getPixels()));
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	    glBindTexture(GL_TEXTURE_2D, 0);

            //Load the sky texture
            	    std::unique_ptr<Image> pImageSky = loadImage("Tron_Labirynth/Project/images/blue.png");
	    if (pImageSky == NULL) {
		std::cout << "ERROR: loading texture failed." << std::endl;
		return EXIT_FAILURE;
	    } else
		std::cout << "Texture loaded " << pImageSky->getWidth() << " x " << pImageSky->getHeight() 
		    << " at location " << pImageSky->getPixels() << std::endl;

	    GLuint textureSky;
	    glGenTextures(1, &textureSky);
	    glBindTexture(GL_TEXTURE_2D, textureSky);
	    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, pImageSky->getWidth(), pImageSky->getHeight(), 0, 
	    GL_RGBA, GL_FLOAT, (const GLvoid *)(pImageSky->getPixels()));
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	    glBindTexture(GL_TEXTURE_2D, 0);

            //Load the night sky texture
            	    std::unique_ptr<Image> pImageNight = loadImage("Tron_Labirynth/Project/images/night.png");
	    if (pImageNight == NULL) {
		std::cout << "ERROR: loading texture failed." << std::endl;
		return EXIT_FAILURE;
	    } else
		std::cout << "Texture loaded " << pImageNight->getWidth() << " x " << pImageNight->getHeight() 
		    << " at location " << pImageNight->getPixels() << std::endl;

	    GLuint textureNight;
	    glGenTextures(1, &textureNight);
	    glBindTexture(GL_TEXTURE_2D, textureNight);
	    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, pImageNight->getWidth(), pImageNight->getHeight(), 0, 
	    GL_RGBA, GL_FLOAT, (const GLvoid *)(pImageNight->getPixels()));
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	    glBindTexture(GL_TEXTURE_2D, 0);

	    //Load monster texture 
	    std::unique_ptr<Image> pImageMonster = loadImage("Tron_Labirynth/Project/images/spnza_bricks_a_diff.tga");
	    if (pImageMonster == NULL) {
		std::cout << "ERROR: loading texture failed." << std::endl;
		return EXIT_FAILURE;
	    } else
		std::cout << "Texture loaded " << pImageMonster->getWidth() << " x " << pImageMonster->getHeight() 
		    << " at location " << pImageMonster->getPixels() << std::endl;

	    GLuint textureMonster;
	    glGenTextures(1, &textureMonster);
	    glBindTexture(GL_TEXTURE_2D, textureMonster);
	    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, pImageMonster->getWidth(), pImageMonster->getHeight(), 0, 
	    GL_RGBA, GL_FLOAT, (const GLvoid *)(pImageMonster->getPixels()));
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	    glBindTexture(GL_TEXTURE_2D, 0);


	    //Load door texture 
	    std::unique_ptr<Image> pImageDoor = loadImage("Tron_Labirynth/Project/images/door3.jpg");
	    if (pImageDoor == NULL) {
		std::cout << "ERROR: loading texture failed." << std::endl;
		return EXIT_FAILURE;
	    } else
		std::cout << "Texture loaded " << pImageDoor->getWidth() << " x " << pImageDoor->getHeight() 
		    << " at location " << pImageDoor->getPixels() << std::endl;

	    GLuint textureDoor;
	    glGenTextures(1, &textureDoor);
	    glBindTexture(GL_TEXTURE_2D, textureDoor);
	    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, pImageDoor->getWidth(), pImageDoor->getHeight(), 0, 
	    GL_RGBA, GL_FLOAT, (const GLvoid *)(pImageDoor->getPixels()));
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	    glBindTexture(GL_TEXTURE_2D, 0);

	    //Load exit texture 
	    std::unique_ptr<Image> pImageExit = loadImage("Tron_Labirynth/Project/images/CloudMap.jpg");
	    if (pImageDoor == NULL) {
		std::cout << "ERROR: loading texture failed." << std::endl;
		return EXIT_FAILURE;
	    } else
		std::cout << "Texture loaded " << pImageExit->getWidth() << " x " << pImageExit->getHeight() 
		    << " at location " << pImageExit->getPixels() << std::endl;

	    GLuint textureExit;
	    glGenTextures(1, &textureExit);
	    glBindTexture(GL_TEXTURE_2D, textureExit);
	    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, pImageExit->getWidth(), pImageExit->getHeight(), 0, 
	    GL_RGBA, GL_FLOAT, (const GLvoid *)(pImageExit->getPixels()));
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	    glBindTexture(GL_TEXTURE_2D, 0);
	    

	    //Get the location of the uniform variables
	    GLint uTexture = glGetUniformLocation(program.getGLId(), "uTexture");
	    GLint location = glGetUniformLocation(program.getGLId(),"uModelMatrix");
	    GLint isSky = glGetUniformLocation(program.getGLId(),"isSky");
	    //cout << "isSky " << isSky << endl;
	    GLint uTime1 = glGetUniformLocation(programSky.getGLId(),"uTime");
	    //GLint uCameraPosition = glGetUniformLocation(programSky.getGLId(),"uCameraPosition");
	    //GLint uCameraNormal = glGetUniformLocation(programSky.getGLId(),"uCameraNormal");
	    //GLint uLightPosition = glGetUniformLocation(programSky.getGLId(),"uLightPosition");
	    glUniform1i(uTexture, 0);

	    GLint uTime2 = glGetUniformLocation(programSky.getGLId(),"uTime");
	    GLint uTextureSky1 = glGetUniformLocation(programSky.getGLId(), "uTexture1");
	    GLint uTextureSky2 = glGetUniformLocation(programSky.getGLId(), "uTexture2");
	    glUniform1i(uTextureSky1, 0);
	    glUniform1i(uTextureSky2, 0);

	    /////////////////LIGHT////////////////////////////////////////
	    program.use();
    	    // Generate Shader Storage Objects
    	    GLuint ssbo[3];
            glGenBuffers(3, ssbo);
            // Upload uniforms
	    GLuint mvpLocation = glGetUniformLocation(program.getGLId(), "MVP");
	    GLuint mvLocation = glGetUniformLocation(program.getGLId(), "MV");
	    GLuint timeLocation = glGetUniformLocation(program.getGLId(), "Time");
	    GLuint diffuseLocation = glGetUniformLocation(program.getGLId(), "Diffuse");
	    GLuint specLocation = glGetUniformLocation(program.getGLId(), "Specular");
	    GLuint lightLocation = glGetUniformLocation(program.getGLId(), "Light");
	    GLuint specularPowerLocation = glGetUniformLocation(program.getGLId(), "SpecularPower");
	    GLuint instanceCountLocation = glGetUniformLocation(program.getGLId(), "InstanceCount");
	    glProgramUniform1i(program.getGLId(), diffuseLocation, 0);
	    glProgramUniform1i(program.getGLId(), specLocation, 1);
	    //////////////////////////////////////////////////////////////

	    //Define the projection properties and the camera
	    mat4 ProjMatrix = perspective(radians(45.f), (float)(600.0)/800.0f, 0.1f, 50.f);
	    TrackballCamera *tCamera = new TrackballCamera();
	    vec2 lastMousePosition;
	    FreeflyCamera camera;
	    camera.rotateLeft(-90); //Rotate -90 in order to correct the labyrinth (IT WILL BE GOOD TO FIND A WAY TO CHANGE IT)
	    float translationMonster = 0; //Define the translation of on monster that is in the same line/column of the hero
	    float translationShot = 0; //Define the translation of each shot
	    float translationDoor = 0; //Define the horizontal translation of one door when it is opened
	    bool shot = false; //Used to verify if the hero fired a shot
	    bool openDoor = false; //Used to verify if the door next to the hero shold be opened
	    float t = 0.0;
	    /*********************************
	     * HERE SHOULD COME THE RENDERING CODE
	    *********************************/
	    bool done = false;
	    int tries = 0; //number of atempts to open some door  
	    glEnable(GL_DEPTH_TEST);
  	    glDepthFunc(GL_LESS);

		// enable client states

	    int current_step = 0;
	    int num_step = 0;
	    
	    int list_steps[] = {0,1,3,0,2,0,1,3,1,2,
				1,3,2,1,3,0,1,3,4,0,
				5, 0, 1, 2, 1, 3,1,2, 0, 1};
	    if (execute_automat == false)
	    {
		cout << "here" << endl;
		for (int i = 0; i < 30; i++)
			list_steps[i] = 0;
	    }
	    int list_number_steps[] = {50, 60, 90,100, 180,10,40, 90, 40,90,
				       40,90,180,60, 90,200, 200, 90, 36, 100, 
                                       36, 200, 50, 45, 30, 45, 70, 90, 100, 60};
	    int count_balls = 0;
	    float posBallX = posX - 4;
	    float posBallY = posY;
	    float posBallZ = float(rand()%1);	
	 
	    //Spot light
	    bool turned_on = true;
	    float radius = 5.f;
	    float intensityLight = 15.f;
	    while(!done) 
	    {
		//cout << "posX / posY : " << ceil(posX) << " " << ceil(posY) << endl;
		if (count_balls == 100)
		{
			posBallX = posX-rand()%10;
			posBallY = posY;
			posBallZ = float(rand()%1);
		}
		count_balls += 1;
		//cout << posX << " " << posY << " " << endl;
		//Handle with the game steps
		if (num_step == list_number_steps[current_step])
		{
			current_step += 1;
			num_step = 0;
		}
		if (list_steps[current_step] == 0)
		{
			num_step += 1;
		}

		if (list_steps[current_step] == 1)
		{
			if (hero.getDirection() == 'n' )
			{
				posX-=0.05;
			}
			else if (hero.getDirection() == 's')
			{
				posX+=0.05;
			}
			else if (hero.getDirection() == 'w')
			{
				posY-=0.05;
			}
			else if (hero.getDirection() == 'e')
			{
				posY+=0.05;
			}
			camera.moveFront(0.05);
			num_step += 1;
		}
		if (list_steps[current_step] == 2)
		{
			camera.rotateLeft(-1);
			num_step += 1;

			if (hero.getDirection() == 'n' and (num_step == 90 ||num_step == 180))
			{
			//camera.moveFront(0.5);
			hero.setDirection('e');

			}

			else if (hero.getDirection() == 'e' and (num_step == 90 ||num_step == 180))
			{
			//camera.moveLeft(-0.5);
			hero.setDirection('s');
			}
			else if (hero.getDirection() == 's' and (num_step == 90 ||num_step == 180))
			{
			//camera.moveFront(-0.5);
			hero.setDirection('w');
			}
			else if (hero.getDirection() == 'w' and (num_step == 90 ||num_step == 180))
			{
			//camera.moveLeft(0.5);
			hero.setDirection('n');
			}
			
		}
		if (list_steps[current_step] == 3)
		{
			camera.rotateLeft(1);
			num_step += 1;
			//camera.displayPosition();
			if (hero.getDirection() == 'n' and (num_step == 90 ||num_step == 180))
			{
				//camera.moveFront(0.5);
				//camera.moveLeft(-0.5);
				hero.setDirection('w');
			}
			else if (hero.getDirection() == 'w' and (num_step == 90 ||num_step == 180))
			{
				//camera.moveLeft(-0.5);
				//camera.moveFront(0.5);
				hero.setDirection('s');	
				
			}
			else if (hero.getDirection() == 's' and (num_step == 90 ||num_step == 180))
			{
				//camera.moveFront(0.5);
				//camera.moveLeft(0.5);
				hero.setDirection('e');
			}
			else if (hero.getDirection() == 'e' and (num_step == 90 ||num_step == 180))
			{
				//camera.moveLeft(-0.5);		
				//camera.moveFront(-0.5);
				hero.setDirection('n');
			}

		}
		if (list_steps[current_step] == 4)
		{
			camera.rotateUp(0.5);
			num_step += 1;
		}

		if (list_steps[current_step] == 5)
		{
			camera.rotateUp(-0.5);
			num_step += 1;
		}
			
		t += 0.0005;
		glUniform1f(uTime1, t); 
		//glUniform1fv(uCameraPosition, 3, glm::value_ptr(camera.getPosition()));
		//glUniform1fv(uCameraNormal, 3, glm::value_ptr(-camera.getNormal()));
		//vec4 lightPosition = vec4(1.f,1.f,1.f,1.f)*camera.getViewMatrix();
		//glUniform1fv(uLightPosition, 3, glm::value_ptr(lightPosition));

		//////////LIGHT////////////////////////////////////////////////

		// Get camera matrices
        	glm::mat4 projection = perspective(radians(45.f), (float)(600.0)/800.0f, 0.1f, 50.f);
        	glm::mat4 worldToView = camera.getViewMatrix();
        	glm::mat4 objectToWorld;
        	glm::mat4 mv = worldToView * objectToWorld;
        	glm::mat4 mvp = projection * mv;
		
		glProgramUniformMatrix4fv(program.getGLId(), mvpLocation, 1, 0, glm::value_ptr(mvp));
        	glProgramUniformMatrix4fv(program.getGLId(), mvLocation, 1, 0, glm::value_ptr(mv));
        	glProgramUniform1i(program.getGLId(), instanceCountLocation, (int) instanceCount);
		glProgramUniform1f(program.getGLId(), specularPowerLocation, 30.f);
        	glProgramUniform1f(program.getGLId(), timeLocation, t);

		struct PointLight
		{
		    glm::vec3 position;
		    int padding;
		    glm::vec3 color;
		    float intensity;
		};

		struct DirectionalLight
		{
		    glm::vec3 position;
		    int padding;
		    glm::vec3 color;
		    float intensity;
		};

		struct SpotLight
		{
		    glm::vec3 position;
		    float angle;
		    glm::vec3 direction;
		    float penumbraAngle;
		    glm::vec3 color;
		    float intensity;
		};


		int pointLightBufferSize = sizeof(PointLight) * pointLightCount + sizeof(int) * 4;
		int directionalLightBufferSize = sizeof(DirectionalLight) * directionalLightCount + sizeof(int) * 4;
		int spotLightBufferSize = sizeof(SpotLight) * spotLightCount + sizeof(int) * 4;

		glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo[0]);
		glBufferData(GL_SHADER_STORAGE_BUFFER, pointLightBufferSize, 0, GL_DYNAMIC_COPY);
		void * lightBuffer = glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_WRITE_ONLY);
		((int*) lightBuffer)[0] = pointLightCount;
		for (int i = 0; i < pointLightCount; ++i) {
		    vec3 pos;
		    vec3 color;
		    float intensity;
		    if (i == 0)
		    {
		    	//pos = glm::vec3( camera.getViewMatrix() * glm::vec4(1.0f, 0.1f, 4.0f, 1.0));
			pos =glm::vec3( camera.getViewMatrix() * glm::vec4(5.0f, 21.1f, 2.0f, 1.0));
			//pos = glm::vec3(0.0f, 0.0f, 0.0f);
			color = vec3(0.f, 0.f, 1.f);
			intensity = rand()%30;
		    }
		    else
	            {
			pos = vec3( camera.getViewMatrix() * glm::vec4((pointLightCount*cosf(t))  * sinf(t*i), 1.0, fabsf(pointLightCount*sinf(t)) * cosf(t*i), 1.0));
			color = glm::vec3(fabsf(cos(t+i*2.f)), 1.-fabsf(sinf(t+i)) , 0.5f + 0.5f-fabsf(cosf(t+i)) );
			intensity = 0.5f + 10*fabsf(cosf(t+i));
	            }
		    PointLight p = { pos, 0,  
		                     color,  
		                     intensity};
		    ((PointLight*) ((int*) lightBuffer + 4))[i] = p;
		}
		glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);


		glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo[1]);
		glBufferData(GL_SHADER_STORAGE_BUFFER, directionalLightBufferSize, 0, GL_DYNAMIC_COPY);
		lightBuffer = glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_WRITE_ONLY);
		((int*) lightBuffer)[0] = directionalLightCount;
		for (int i = 0; i < directionalLightCount; ++i) {
		    //float intensity =  5.f + fabsf(cosf(i)) * 0.1f;
	            float intensity =  5.f;
		    //simulate the sun
		    DirectionalLight directionalLight = { glm::vec3( camera.getViewMatrix() * glm::vec4(cos(360*(t/100)), sin(360*(t/100)), cos(360*(t/100)), 0.0)), 10,  
		                                          glm::vec3(1.0, 1.0, 1.0),  
		                                         intensity};
		    ((DirectionalLight*) ((int*) lightBuffer + 4))[i] = directionalLight;
		}
		glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);



		glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo[2]);
		glBufferData(GL_SHADER_STORAGE_BUFFER, spotLightBufferSize, 0, GL_DYNAMIC_COPY);
		lightBuffer = glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_WRITE_ONLY);
		((int*) lightBuffer)[0] = spotLightCount;
		for (int i = 0; i < spotLightCount; ++i)  {

		    glm::vec3 position;
		    float angle;
		    glm::vec3 direction;
		    float penumbraAngle;
		    glm::vec3 color;
		    float intensity;
		    
		    if (i == 0 || i != 0)
		    {
			    // cout << "aqui0" << endl;
			    //position = glm::vec3( camera.getViewMatrix() * glm::vec4(1.0f, 1.0, 4.0f, 1.0));
			    if (spotLightCount > 1)
			    	position = glm::vec3(cos(360.f*i/spotLightCount)/2, sin(360.f*i/spotLightCount)/2 , 0.0f);
			    else
				position = glm::vec3(0.f, 0.f, 0.0f);
			    //cout << "position = " << position << endl;
			    angle = radius;
			    direction = glm::vec3(0.0, 0.0, -1.0);
			    penumbraAngle =  radius;
			    color = glm::vec3(0.f, 1.f, 0.f) ;
			    intensity = intensityLight;
		    }

		    /*else if (i == 1)
		    {
			    cout << "aqui1" << endl;
			    //position = glm::vec3( camera.getViewMatrix() * glm::vec4(1.0f, 1.0, 4.0f, 1.0));
			    position = glm::vec3( 0.1f, 0.0f, 0.0f);
			    cout << "position = " << position << endl;
			    angle = 3.f;
			    direction = glm::vec3(0.0, 0.0, -1.0);
			    penumbraAngle =  3.f;
			    color = glm::vec3(1.f, 1.f, 1.f) ;
			    intensity = 25.f;
		    }*/

		    else {
			    position = glm::vec3( camera.getViewMatrix() * glm::vec4((spotLightCount*sinf(t))  * cosf(t*i), 1.f + sinf(t * i), fabsf(spotLightCount*cosf(t)) * sinf(t*i), 1.0));
			    angle = 45.f + 20.f * cos(t + i);
			    direction = glm::vec3( camera.getViewMatrix() * glm::vec4(sinf(t*10.0+i), -1.0, 0.0, 0.0));
			    penumbraAngle =  60.f + 20.f * cos(t + i);
			    color = glm::vec3(fabsf(cos(t+i*2.f)), 1.-fabsf(sinf(t+i)), 0.5f + 0.5f-fabsf(cosf(t+i))) ;
			    intensity = 1.0;
		}
		    SpotLight spotLight = { position, angle , direction
		                            ,penumbraAngle,  
		                            color, intensity};
		    ((SpotLight*) ((int*) lightBuffer + 4))[i] = spotLight;
		}
		
		glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

		glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 0, ssbo[0], 0, pointLightBufferSize);
		glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 1, ssbo[1], 0, directionalLightBufferSize);
		glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 2, ssbo[2], 0, spotLightBufferSize);
		//////////////////////////////////////////////////////////////////

		// Event loop:
		SDL_Event e;

		    while(windowManager.pollEvent(e)) 
		    {
		    	if (e.type == SDL_MOUSEMOTION) 
		    	{
		        	//std::cout << "MOTION";
		        	if (e.motion.state == SDL_BUTTON_RMASK) 
		        	{
		            	//std::cout << " + BUTTON";
		            	//std::cout << "DX = " << e.motion.x - lastMousePosition.x << "\tDY = " << e.motion.y - lastMousePosition.y << std::endl;
		            	camera.rotateUp(-(e.motion.y - lastMousePosition.y));
		            	camera.rotateLeft(-(e.motion.x - lastMousePosition.x));
						////camera.displayPosition();
		        	}
		        	//std::cout << std::endl;
		        	lastMousePosition = vec2(e.motion.x, e.motion.y);
		    	}
			if (windowManager.isKeyPressed(SDLK_l)) 
			{
				if (turned_on == true)				
					turned_on = false;
				else
					turned_on = false;
			}

			if (windowManager.isKeyPressed(SDLK_z)) 
			{
				radius += 3.f;
			}

			if (windowManager.isKeyPressed(SDLK_x)) 
			{
				radius -= 3.f;
			}

			if (windowManager.isKeyPressed(SDLK_c)) 
			{
				intensityLight += 5.f;
			}

			if (windowManager.isKeyPressed(SDLK_v)) 
			{
				intensityLight -= 5.f;
			}

			if (windowManager.isKeyPressed(SDLK_b)) 
			{
				spotLightCount += 1;
			}
			if (windowManager.isKeyPressed(SDLK_n)) 
			{
				spotLightCount -= 1;
			}

		    	if (windowManager.isKeyPressed(SDLK_LEFT)) 
				{
					//camera.displayPosition();
					if (hero.getDirection() == 'n')
					{
						//camera.moveFront(0.5);
						//camera.moveLeft(-0.5);
						hero.setDirection('w');
					}
					else if (hero.getDirection() == 'w')
					{
						//camera.moveLeft(-0.5);
						//camera.moveFront(0.5);
						hero.setDirection('s');	
						
					}
					else if (hero.getDirection() == 's')
					{
						//camera.moveFront(0.5);
						//camera.moveLeft(0.5);
						hero.setDirection('e');
					}
					else if (hero.getDirection() == 'e')
					{
						//camera.moveLeft(-0.5);		
						//camera.moveFront(-0.5);
						hero.setDirection('n');
					}
				
					camera.rotateLeft(5);
					
					//std::cout << "posX: " << posX << " posY: " << posY << " direction:" << hero.getDirection() << std::endl;
		    	}
				if (windowManager.isKeyPressed(SDLK_RIGHT)) 
				{
					//camera.displayPosition();
					if (hero.getDirection() == 'n')
					{
						//camera.moveFront(0.5);
						hero.setDirection('e');
						
					}

					else if (hero.getDirection() == 'e')
					{
						//camera.moveLeft(-0.5);
						hero.setDirection('s');
					}
					else if (hero.getDirection() == 's')
					{
						//camera.moveFront(-0.5);
						hero.setDirection('w');
					}
					else if (hero.getDirection() == 'w')
					{
						//camera.moveLeft(0.5);
						hero.setDirection('n');
					}
					camera.rotateLeft(-5);
					
					//std::cout << "posX: " << posX << " posY: " << posY << " direction:" << hero.getDirection() << std::endl;
				}
					if (windowManager.isKeyPressed(SDLK_UP)) 
				{
					//camera.displayPosition();
					if (hero.getDirection() == 'n' )//and labyrinth[(posX-1)*columns+posY] != 0 and labyrinth[(posX-1)*columns+posY] != 5)
					{
						camera.moveFront(0.1);
						posX--;
					}
					else if (hero.getDirection() == 's')// and labyrinth[(posX+1)*columns+posY] != 0 and labyrinth[(posX+1)*columns+posY] != 5)
					{
						camera.moveFront(0.1);
						posX++;
					}
					else if (hero.getDirection() == 'w')// and labyrinth[(posX)*columns+(posY-1)] != 0 and labyrinth[(posX)*columns+(posY-1)] != 5)
					{
						camera.moveFront(0.1);
						posY--;
					}
					else if (hero.getDirection() == 'e') //and labyrinth[(posX)*columns+(posY+1)] != 0 and labyrinth[(posX)*columns+(posY+1)] != 5)
					{
						camera.moveFront(0.1);
						posY++;
					}
					//std::cout << "posX: " << posX << " posY: " << posY << " direction:" << direction << std::endl;

					//Print the labyrinth to verify if the movement was correct
					for (int i=0; i< lines; i++)
					{
						for (int j = 0; j < columns; j++)
						{
							if (i == posX and j == posY)
								std::cout << "* ";
								else if (labyrinth[i*columns+j] == 0)
										std::cout << "0 ";
								else
										std::cout << labyrinth[i*columns+j] << " ";
						}
						std::cout << std::endl;
					}
		    	}
		    	if (windowManager.isKeyPressed(SDLK_DOWN)) 
				{
					////camera.displayPosition();
					if (hero.getDirection() == 'n')// and labyrinth[(posX+1)*columns+posY] != 0)
					{
						camera.moveFront(-0.1);
						posX++;
					}
					else if (hero.getDirection() == 's')// and labyrinth[(posX-1)*columns+posY] != 0)
					{
						camera.moveFront(-0.1);
						posX--;
					}
					else if (hero.getDirection() == 'w')// and labyrinth[(posX)*columns+(posY+1)] != 0)
					{
						camera.moveFront(-0.1);
						posY++;
					}
					else if (hero.getDirection() == 'e')// and labyrinth[(posX)*columns+(posY-1)] != 0)
					{
						camera.moveFront(-0.1);
						posY--;
					}
					//Print the labyrinth and current hero position
					//std::cout << "posX: " << posX << " posY: " << posY << " direction:" << direction << std::endl;
					for (int i=0; i< lines; i++)
						{
						for (int j = 0; j < columns; j++)
						{
							if (i == posX and j == posY)
								std::cout << "* ";
								else if (labyrinth[i*columns+j] == 0)
										std::cout << "0 ";
								else
										std::cout << "1 ";
						}
						std::cout << std::endl;
					}

				}
				if (windowManager.isKeyPressed(SDLK_a) || windowManager.isKeyPressed(SDLK_q)) 
				{
					camera.moveLeft(0.1);
				}

				if (windowManager.isKeyPressed(SDLK_d) ) 
				{
					camera.moveLeft(-0.1);
				}
				 if (windowManager.isKeyPressed(SDLK_g))
				{
					shot = true;
				}
				if (windowManager.isKeyPressed(SDLK_o))
				{
					openDoor = true;
				}
					  
							
				if(e.type == SDL_QUIT) {
					done = true; // Leave the loop after this iteration
				}
		    
		   
			}
		
			//Draw the Ceiling
			glClearColor(0.2,0.0,0.2,0.0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			glBindVertexArray(vao_floor[0]);
			glBindTexture(GL_TEXTURE_2D, textureSky);
			glUniform1i(uTexture, 0);
			mat4 MVMatrix = translate(mat4(1), vec3(0, 0, -5));
			camera.computeDirectionVectors();

			mat4 globalMVMatrix = camera.getViewMatrix() * translate(mat4(1.f), vec3(0, 0, -5));
			mat4 finalMatrix = ProjMatrix*translate(globalMVMatrix,vec3(-1, 0, 0));//*scaling(100, 100, 100);

				
			glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(finalMatrix));

			glDrawElements(GL_TRIANGLES, plane_triangleCount*3, GL_UNSIGNED_INT, (void*)0);
			glBindTexture(GL_TEXTURE_2D, 0);	
			glBindVertexArray(0);

			//If the hero fired a shot (key 'g'), then it is draw as a tranlation from the hero to the next two positions
			if (shot)
			{
					std::cout << "SHOT" << std::endl;
					glBindVertexArray(vao_monster); //The same sphere is used for monsters and fire shot
					glBindTexture(GL_TEXTURE_2D, textureFloor);
					glUniform1i(uTexture, 0);
					camera.computeDirectionVectors();
					mat4 globalMVMatrix;
					
					//The shot translation depends on the direction of the hero's field of view
					if (hero.getDirection() == 'n')
						globalMVMatrix = camera.getViewMatrix() * translate(mat4(1.f), vec3((lines-posX)+translationShot, 0.0, posY));
					else if (hero.getDirection() == 's')
						globalMVMatrix = camera.getViewMatrix() * translate(mat4(1.f), vec3((lines-posX+1)-translationShot, 0.0, posY));
					else if (hero.getDirection() == 'w')
						globalMVMatrix = camera.getViewMatrix() * translate(mat4(1.f), vec3((lines-posX+0.5), 0.0, posY-translationShot));
					else if (hero.getDirection() == 'e')
						globalMVMatrix = camera.getViewMatrix() * translate(mat4(1.f), vec3((lines-posX+0.5), 0.0, posY+translationShot));
					mat4 finalMatrix = ProjMatrix*translate(globalMVMatrix,vec3(-(lines-initialLine), 0, -initialColumn))*scaling(0.01, 0.01, 0.01);
					glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(finalMatrix));
					glUniform1i(uTexture, 0);
					glDrawArrays(GL_TRIANGLES, 0, sphere.getVertexCount() * sizeof(ShapeVertex));
		    			glBindVertexArray(0);
					translationShot += 0.01; //Define the shot speed
					//After translating two positions, the shot 'desapears'
					if (translationShot > 2.0)
					{
						shot = false;
						translationShot = 0;
					}
			}
			

			//Draw the labyrinth

			for (int i=0; i< lines; i++)
			{
				for (int j = 0; j < columns; j++)
				{

				//Verifies if the current position, defined by the line i and column j, is an wall
				if (labyrinth[i*columns+j] == 0)
				{
					glBindVertexArray(vao_cube[0]);
					glBindTexture(GL_TEXTURE_2D, textureWall);
					glUniform1i(uTexture, 0);
					mat4 MVMatrix = translate(mat4(1), vec3(0, 0, -5));
					camera.computeDirectionVectors();
					mat4 globalMVMatrix = camera.getViewMatrix() * translate(mat4(1.f), vec3((lines-i), 0, j)); //Define the translation needed by each position of wall on the labyrinth
					mat4 finalMatrix = ProjMatrix*translate(globalMVMatrix,vec3(-(lines-initialLine-1)-1.0, 0, -initialColumn)); //Translate the whole labyrinth to be visible by the camera
					glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(finalMatrix));
					glUniform1i(uTexture, 0);

					/////////////////LIGHT////////////////////////////////////////////////
					worldToView = camera.getViewMatrix();
					mv = globalMVMatrix;
					mvp = projection * mv;

					glProgramUniformMatrix4fv(program.getGLId(), mvpLocation, 1, 0, glm::value_ptr(mvp));
					glProgramUniformMatrix4fv(program.getGLId(), mvLocation, 1, 0, glm::value_ptr(mv));
					/////////////////////////////////////////////////////////////////////

					glDrawElements(GL_TRIANGLES, cube_triangleCount*3, GL_UNSIGNED_INT, (void*)0);
					
				}
				if (labyrinth[i*columns+j] == 3)
				{
					if (posX*columns+posY == 3)
					{
						std::cout << "YOU WON!";
						done = true;
					}
					
					glBindVertexArray(vao_cube[0]);
					glUniform1i(isSky, 1);
					glBindTexture(GL_TEXTURE_2D, textureNight);
					glUniform1i(uTexture, 0);
					mat4 MVMatrix = translate(mat4(1), vec3(0, 0, -5));
					camera.computeDirectionVectors();
					mat4 globalMVMatrix = camera.getViewMatrix() * translate(mat4(1.f), vec3((lines-i), 0, j)); //Define the translation needed by each position of wall on the labyrinth
					mat4 finalMatrix = ProjMatrix*translate(globalMVMatrix,vec3(-(lines-initialLine-1)-1.0, 0, -initialColumn)); //Translate the whole labyrinth to be visible by the camera
					glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(finalMatrix));
					glUniform1i(uTexture, 0);

					/////////////////LIGHT////////////////////////////////////////////////
					worldToView = camera.getViewMatrix();
					mv = globalMVMatrix;
					mvp = projection * mv;

					glProgramUniformMatrix4fv(program.getGLId(), mvpLocation, 1, 0, glm::value_ptr(mvp));
					glProgramUniformMatrix4fv(program.getGLId(), mvLocation, 1, 0, glm::value_ptr(mv));
					/////////////////////////////////////////////////////////////////////
					glDrawElements(GL_TRIANGLES, cube_triangleCount*3, GL_UNSIGNED_INT, (void*)0);
					glUniform1i(isSky, 0);
				}
				//Draw ceiling and floor
				else
				{
					//programSky.use();
					glUniform1i(isSky, 1);
					//glUniform1f(uTime2, t);
					glBindVertexArray(vao_cube[0]);
					//glBindTexture(GL_TEXTURE_2D, textureSky);
					//glUniform1i(uTextureSky1, textureSky);
					glBindTexture(GL_TEXTURE_2D, textureNight);
					glUniform1i(uTexture, textureNight);
					camera.computeDirectionVectors();
					
					//Draw the ceiling translating one cube one position in the vertical axis
					mat4 globalMVMatrix = camera.getViewMatrix() * translate(mat4(1.f), vec3((lines-i), 1.0, j));
					mat4 finalMatrix = ProjMatrix*translate(globalMVMatrix,vec3(-(lines-initialLine-1)-1.0, 0, -initialColumn));
					glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(finalMatrix));

					/////////////////LIGHT////////////////////////////////////////////////
					worldToView = camera.getViewMatrix();
					mv = globalMVMatrix;
					mvp = projection * mv;

					glProgramUniformMatrix4fv(program.getGLId(), mvpLocation, 1, 0, glm::value_ptr(mvp));
					glProgramUniformMatrix4fv(program.getGLId(), mvLocation, 1, 0, glm::value_ptr(mv));
					/////////////////////////////////////////////////////////////////////

					glDrawElements(GL_TRIANGLES, cube_triangleCount*3, GL_UNSIGNED_INT, (void*)0);
					
					// Bind our texture in Texture Unit 0
					/*glBindTexture(GL_TEXTURE_2D, textureFloor);
					globalMVMatrix = camera.getViewMatrix() * translate(mat4(1.f), vec3((lines-i), -1.0, j));
					finalMatrix = ProjMatrix*translate(globalMVMatrix,vec3(-(lines-initialLine-1)-1.0, 0, -initialColumn));
					glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(finalMatrix));
					glUniform1i(uTexture, 0);

					// 1rst attribute buffer : vertices
					glEnableVertexAttribArray(0);
					glBindBuffer(GL_ARRAY_BUFFER, vertexbufferObj);
					glVertexAttribPointer(
						0,                  // attribute
						3,                  // size
						GL_FLOAT,           // type
						GL_FALSE,           // normalized?
						0,                  // stride
						(void*)0            // array buffer offset
					);

					// 2nd attribute buffer : UVs
					glEnableVertexAttribArray(1);
					glBindBuffer(GL_ARRAY_BUFFER, uvbufferObj);
					glVertexAttribPointer(
						1,                                // attribute
						2,                                // size
						GL_FLOAT,                         // type
						GL_FALSE,                         // normalized?
						0,                                // stride
						(void*)0                          // array buffer offset
					);

					// Draw the triangle !
					/////////////////LIGHT////////////////////////////////////////////////
					worldToView = camera.getViewMatrix();
					mv = globalMVMatrix;
					mvp = projection * mv;

					glProgramUniformMatrix4fv(program.getGLId(), mvpLocation, 1, 0, glm::value_ptr(mvp));
					glProgramUniformMatrix4fv(program.getGLId(), mvLocation, 1, 0, glm::value_ptr(mv));
					/////////////////////////////////////////////////////////////////////

					glDrawArrays(GL_TRIANGLES, 0, verticesObj.size() );

					glDisableVertexAttribArray(0);
					glDisableVertexAttribArray(1);*/

					program.use();
					glUniform1i(isSky, 0);
					//The same as the ceiling, except for the translation signal
					glBindTexture(GL_TEXTURE_2D, textureFloor);
					globalMVMatrix = camera.getViewMatrix() * translate(mat4(1.f), vec3((lines-i), -1.0, j));
					finalMatrix = ProjMatrix*translate(globalMVMatrix,vec3(-(lines-initialLine-1)-1.0, 0, -initialColumn));
					glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(finalMatrix));
					glUniform1i(uTexture, 0);
					/////////////////LIGHT////////////////////////////////////////////////
					worldToView = camera.getViewMatrix();
					mv = globalMVMatrix;
					mvp = projection * mv;

					glProgramUniformMatrix4fv(program.getGLId(), mvpLocation, 1, 0, glm::value_ptr(mvp));
					glProgramUniformMatrix4fv(program.getGLId(), mvLocation, 1, 0, glm::value_ptr(mv));
					/////////////////////////////////////////////////////////////////////

					glDrawElements(GL_TRIANGLES, cube_triangleCount*3, GL_UNSIGNED_INT, (void*)0);
				}

				//The value '5' represents a door at the position
				if (labyrinth[i*columns+j] == 5)
				{
					glBindVertexArray(vao_cube[0]);
					glBindTexture(GL_TEXTURE_2D, textureDoor);
					glUniform1i(uTexture, 0);
					mat4 MVMatrix = translate(mat4(1), vec3(0, 0, -5));

					//Left side of the door
					camera.computeDirectionVectors();
					mat4 globalMVMatrix = camera.getViewMatrix() * translate(mat4(1.f), vec3((lines-i)-1.0, 0, j-0.5));
					mat4 finalMatrix = ProjMatrix*translate(globalMVMatrix,vec3(-(lines-initialLine-1)-1.0, 0, -initialColumn-translationDoor))*scaling(0.1, 1, 1);;
					glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(finalMatrix));
					glUniform1i(uTexture, 0);


					//Right side of the door
					glDrawElements(GL_TRIANGLES, cube_triangleCount*3, GL_UNSIGNED_INT, (void*)0);
					globalMVMatrix = camera.getViewMatrix() * translate(mat4(1.f), vec3((lines-i)-1.0, 0, j+0.5));
					finalMatrix = ProjMatrix*translate(globalMVMatrix,vec3(-(lines-initialLine-1)-1.0, 0, -initialColumn+ translationDoor))*scaling(0.1, 1, 1);;
					glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(finalMatrix));
					glUniform1i(uTexture, 0);

					/////////////////LIGHT////////////////////////////////////////////////
					worldToView = camera.getViewMatrix();
					mv = globalMVMatrix;
					mvp = projection * mv;

					glProgramUniformMatrix4fv(program.getGLId(), mvpLocation, 1, 0, glm::value_ptr(mvp));
					glProgramUniformMatrix4fv(program.getGLId(), mvLocation, 1, 0, glm::value_ptr(mv));
					/////////////////////////////////////////////////////////////////////

					glDrawElements(GL_TRIANGLES, cube_triangleCount*3, GL_UNSIGNED_INT, (void*)0);

					//Verify if the door next to the hero should be open (The variable is set 'true' by pressing the key 'o')
					/*bool isNextToDoor = false;
					if ((hero.getDirection() == 'n' and hero.getPositionX() == i-1) or
					    (hero.getDirection() == 's' and hero.getPositionX() == i-1) or
					    (hero.getDirection() == 'w' and hero.getPositionY() == j+1) or
					    (hero.getDirection() == 'e' and hero.getPositionY() == j-1))
					    isNextToDoor = true;*/
					//std::cout << i << " " << j << " " << hero.getPositionX() << " " << hero.getPositionY() << std::endl;
					bool isNextToDoor = false;
					if ((hero.getDirection() == 'n' and posX == i+1 and posY == j) or
					    (hero.getDirection() == 's' and posX == i-1 and posY == j) or
					    (hero.getDirection() == 'w' and posX == i and posY == j+1) or
					    (hero.getDirection() == 'e' and posX == i and posY == j-1))
					    isNextToDoor = true;
										
					if (openDoor)
					{
						std::cout << "Door " << i << " " << j << " " << posX << " " << posY << " " << isNextToDoor << " " << openDoor << std::endl;
						if (isNextToDoor)
						{
							std::cout << "aqui" << std::endl;
							//If both sides of the door are totaly open, they 'desapear' from the labyrinth
							if (translationDoor > 0.5)
							{
								
								labyrinth[i*columns+j] = 1;
								openDoor = false;
								translationDoor = 0;
							}
							//Each side of the door is translated in the same speed, but oposite directions
							else	
								translationDoor += 0.05;
						}
						else if (tries > numberOfDoors*10)
						{
							openDoor = false;
							tries = 0;
						}
						else
							tries++;
					}
					
				}
			}
		}

		//Draw the random balls
		if (ceil(posX) == 12 and ceil(posY) == 11)
		{
			for (int i = 0; i < numberOfBalls; i++)
			{
				glm::mat4 rotateBall;
				glBindVertexArray(vao_monster);
			    	glBindTexture(GL_TEXTURE_2D, textureMonster);
				glUniform1i(uTexture, 0);

				camera.computeDirectionVectors();
				listBalls[i][1] -= 0.1f;
				rotateBall = rotateAlongY(-5);

				mat4 globalMVMatrix = camera.getViewMatrix() * translate(mat4(1.f), vec3(lines-(listBalls[i][0]), 0.0, (listBalls[i][1])))*rotateBall*scaling(0.01, 0.01, 0.01);
			

				/////////////////LIGHT////////////////////////////////////////////////
				worldToView = camera.getViewMatrix();
				mv = globalMVMatrix;
				mvp = projection * mv;

				glProgramUniformMatrix4fv(program.getGLId(), mvpLocation, 1, 0, glm::value_ptr(mvp));
				glProgramUniformMatrix4fv(program.getGLId(), mvLocation, 1, 0, glm::value_ptr(mv));
				/////////////////////////////////////////////////////////////////////

			    	glDrawArrays(GL_TRIANGLES, 0, sphere.getVertexCount() * sizeof(ShapeVertex));
			    	glBindVertexArray(0);
			}
		}
	
		//Check if the list of monsters on of them is aligned to the hero. In this case, it is draw at his position and then translated
		//towards the hero (FOR WHILE, IT DOESN'T ATACK)
		//for (int i = 0; i < numberOfMonsters; i++)
		for (int i = 0; i < 0; i++)
		{
			
			glm::mat4 rotateMonster;
			//Verify if the hero is in the same line of the monster and, then, translate it along the line 'posX'
			if (ceil(posX) == ceil(listOfMonsters[i].getPositionX()))
			{
				glBindVertexArray(vao_monster);
			    	glBindTexture(GL_TEXTURE_2D, textureMonster);
				glUniform1i(uTexture, 0);

				camera.computeDirectionVectors();
				//The monster always moves towards the hero, decreasing the distance between them
				if (ceil(posY) >= ceil(listOfMonsters[i].getPositionY())) {
					listOfMonsters[i].setPosition(vec2(listOfMonsters[i].getPositionX(), listOfMonsters[i].getPositionY()+0.001f));
					rotateMonster = rotateAlongY(-180);
				}
				if (ceil(posY) <= ceil(listOfMonsters[i].getPositionY())) {
					listOfMonsters[i].setPosition(vec2(listOfMonsters[i].getPositionX(), listOfMonsters[i].getPositionY()-0.001f));
					rotateMonster = rotateAlongY(180);
				}

				mat4 globalMVMatrix = camera.getViewMatrix() * translate(mat4(1.f), vec3(lines-(listOfMonsters[i].getPositionX()), 0.0, listOfMonsters[i].getPositionY()));
				
				mat4 finalMatrix = ProjMatrix*translate(globalMVMatrix,vec3(-(lines-initialLine-1)-1.0, 0, -initialColumn))*rotateMonster*(tCamera -> scale(0.5, 0.5, 0.5));
				glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(finalMatrix));
		    	glUniform1i(uTexture, 0);

			/////////////////LIGHT////////////////////////////////////////////////
			worldToView = camera.getViewMatrix();
			mv = globalMVMatrix;
			mvp = projection * mv;

			glProgramUniformMatrix4fv(program.getGLId(), mvpLocation, 1, 0, glm::value_ptr(mvp));
			glProgramUniformMatrix4fv(program.getGLId(), mvLocation, 1, 0, glm::value_ptr(mv));
			/////////////////////////////////////////////////////////////////////

		    	glDrawArrays(GL_TRIANGLES, 0, sphere.getVertexCount() * sizeof(ShapeVertex));
		    	glBindVertexArray(0);
		   	 }
			//Verify if the hero is in the same line of the monster and, then, translate it along the line 'posX'
			if (ceil(posY) == ceil(listOfMonsters[i].getPositionY()))
			{
				glBindVertexArray(vao_monster);
			    	glBindTexture(GL_TEXTURE_2D, textureMonster);
				glUniform1i(uTexture, 0);
				camera.computeDirectionVectors();
				//The monster always moves towards the hero
				if (ceil(posX) >= ceil(listOfMonsters[i].getPositionX()))
				{
					listOfMonsters[i].setPosition(vec2(listOfMonsters[i].getPositionX()+0.001f, listOfMonsters[i].getPositionY()));
					rotateMonster = rotateAlongY(-90);
				}
				if (ceil(posX) <= ceil(listOfMonsters[i].getPositionX()))
				{
					listOfMonsters[i].setPosition(vec2(listOfMonsters[i].getPositionX()-0.001f, listOfMonsters[i].getPositionY()));
					rotateMonster = rotateAlongY(90);
				}

				mat4 globalMVMatrix = camera.getViewMatrix() * translate(mat4(1.f), vec3(lines-(listOfMonsters[i].getPositionX()), 0.0, listOfMonsters[i].getPositionY()));
				
				mat4 finalMatrix = ProjMatrix*translate(globalMVMatrix,vec3(-(lines-initialLine-1)-1.0, 0, -initialColumn))*rotateMonster*(tCamera -> scale(0.5, 0.5, 0.5));
				glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(finalMatrix));
		    	glUniform1i(uTexture, 0);

			/////////////////LIGHT////////////////////////////////////////////////
			worldToView = camera.getViewMatrix();
			mv = globalMVMatrix;
			mvp = projection * mv;

			glProgramUniformMatrix4fv(program.getGLId(), mvpLocation, 1, 0, glm::value_ptr(mvp));
			glProgramUniformMatrix4fv(program.getGLId(), mvLocation, 1, 0, glm::value_ptr(mv));
			/////////////////////////////////////////////////////////////////////
		    	
			glDrawArrays(GL_TRIANGLES, 0, sphere.getVertexCount() * sizeof(ShapeVertex));
		    	glBindVertexArray(0);	
		   	 }
			//Check, in case one shot was fired, if it colides with the monster at some time.
			int positionOfShotX = (lines-posX)+translationShot;
			int positionOfShotY = posY;
			int positionOfMonsterX = lines-listOfMonsters[i].getPositionX();
			int positionOfMonsterY = listOfMonsters[i].getPositionY();
			//std::cout << "Shot : ( " << positionOfShotX << ", " << positionOfShotY << ")" << std::endl;
			//std::cout << "Monster: ( " << positionOfMonsterX << ", " << positionOfMonsterY << ")" << std::endl;
			//In the afirmative case, the monster 'desapears'
			if ( shot == true and positionOfShotX == positionOfMonsterX and positionOfShotY == positionOfMonsterY)
			{
				listOfMonsters[i].setPosition(vec2(-1.f, -1.f));
				shot = false;
			}

		}
		    
		/*//Draw the object
		glBindVertexArray(vao_obj);
		glBindTexture(GL_TEXTURE_2D, textureMonster);
		glUniform1i(uTexture, 0);

		mat4 globalMVMatrix = camera.getViewMatrix() * translate(mat4(1.f), vec3((lines-1), 0.0, 6));
				
		mat4 finalMatrix = ProjMatrix*translate(globalMVMatrix,vec3(-(lines-initialLine-1)-0.5, 0, -initialColumn))*(tCamera -> scale(0.5, 0.5, 0.5));
		glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(finalMatrix));
    		glUniform1i(uTexture, 0);
    		glDrawArrays(GL_TRIANGLES, 0, geo.getVertexCount() );*/

		// Update the display
		windowManager.swapBuffers();
	}

	return EXIT_SUCCESS;
}
