#pragma once
#include <GL/glew.h> 
#include <GLFW/glfw3.h> 
#include <stdio.h>
#include <math.h>
#include <string>
#include <glm/glm.hpp> 
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdlib.h>
#include <fstream>

using std::vector;
using std::string;
using glm::vec2;
using glm::vec3;
using glm::mat4;

class OBJWriter {
	private: 
		vector<glm::vec3*>* internalVec = new vector<glm::vec3*>();
		vector<glm::vec3*>* externalVec = new vector<glm::vec3*>();
		std::ofstream objFile;

		void createOBJFile();
		void writeVertices();
		void writeTextures();
		void writeNormals();
		void writeFaces();

	public:
		OBJWriter(vector<glm::vec3*>* internalVec, vector<glm::vec3*>* externalVec);
		void write();

};

