#include "Source.h"

#include <GL/glew.h> /* include GLEW and new version of GL on Windows */
#include <GLFW/glfw3.h> /* GLFW helper library */
#include <stdio.h>
#include <math.h>
#include <iostream>
#include "Shader.h"
#include "OBJWriter.h";
using namespace std;

float CURVE_DISTANCE = 15.0f;
float PI = 3.14159265358979323846;

void initializeGraphics();
void initializeBuffers();
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void convertCoordinates(float& x, float& y);
void draw();
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void createBspline();
void createBsplineInternalAndExternal();
void createSpeedway();
void writeTxt();
void writeMtl();
void writeObj();
void normalizeInternalExternalCords();

GLFWwindow* window = NULL;
vector<glm::vec3*>* controlPointsVec = new vector<glm::vec3*>();
vector<glm::vec3*>* bsplineVec = new vector<glm::vec3*>();
vector<glm::vec3*>* bsplineInternalVec = new vector<glm::vec3*>();
vector<glm::vec3*>* bsplineExternalVec = new vector<glm::vec3*>();
vector<glm::vec3*>* speedwayVec = new vector<glm::vec3*>();
vector<float> controlPoints, bspline, bsplineInternal, bsplineExternal, speedwayPoints;
GLuint vaoControlPoints, vaoBspline, vaoBsplineInternal, vaoBsplineExternal, vaoSpeedway;
GLuint vboControlPoints, vboBspline, vboBsplineInternal, vboBsplineExternal, vboSpeedway;

const float WIDTH = 600, HEIGHT = 600;
glm::mat4 projection = glm::ortho(0.0f, WIDTH, HEIGHT, 0.0f, -1.0f, 1.0f);

int main() {

	//Inicialização dos gráficos
	initializeGraphics();

	//Inicialização dos shaders
	Shader shader("shader.vert", "shader.frag");

	//Define o callback do mouse
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	//inicializa vao e vbo
	initializeBuffers();

	while (!glfwWindowShouldClose(window)) {
		//clear screen
		glClearColor(0.2f, 0.2f, 0.2f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		shader.use();
		shader.setMat4("projection", projection);

		draw();

		glfwSwapBuffers(window);
		glfwPollEvents();

		if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_ESCAPE)) {
			glfwSetWindowShouldClose(window, 1);
		}
	}

	/* close GL context and any other GLFW resources */
	glfwTerminate();
	return 0;
}

void initializeGraphics() {
	/* start GL context and O/S window using the GLFW helper library */
	if (!glfwInit()) {
		fprintf(stderr, "ERROR: could not start GLFW3\n");
		glfwTerminate();
	}

	/* change to 3.2 if on Apple OS X */
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(
		WIDTH, HEIGHT, "CGGB - Gerador de Pista", NULL, NULL
	);
	if (!window) {
		fprintf(stderr, "ERROR: could not open window with GLFW3\n");
		glfwTerminate();
		glfwTerminate();
	}
	glfwMakeContextCurrent(window);
	/* start GLEW extension handler */
	glewExperimental = GL_TRUE;
	glewInit();
	
	projection = glm::ortho(0.0f, WIDTH, HEIGHT, 0.0f, -1.0f, 1.0f);
}

void initializeBuffers() {
	//control points
	glGenVertexArrays(1, &vaoControlPoints);

	glGenBuffers(1, &vboControlPoints);

	glBindVertexArray(vaoControlPoints);
	glBindBuffer(GL_ARRAY_BUFFER, vboControlPoints);
	glBufferData(GL_ARRAY_BUFFER, controlPoints.size() * sizeof(float), controlPoints.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);

	//bspline
	glGenVertexArrays(1, &vaoBspline);

	glGenBuffers(1, &vboBspline);

	glBindVertexArray(vaoBspline);
	glBindBuffer(GL_ARRAY_BUFFER, vboBspline);
	glBufferData(GL_ARRAY_BUFFER, bspline.size() * sizeof(float), bspline.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);

	//bspline internal
	glGenVertexArrays(1, &vaoBsplineInternal);

	glGenBuffers(1, &vboBsplineInternal);

	glBindVertexArray(vaoBsplineInternal);
	glBindBuffer(GL_ARRAY_BUFFER, vboBsplineInternal);
	glBufferData(GL_ARRAY_BUFFER, bsplineInternal.size() * sizeof(float), bsplineInternal.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);

	//bspline external
	glGenVertexArrays(1, &vaoBsplineExternal);

	glGenBuffers(1, &vboBsplineExternal);

	glBindVertexArray(vaoBsplineExternal);
	glBindBuffer(GL_ARRAY_BUFFER, vboBsplineExternal);
	glBufferData(GL_ARRAY_BUFFER, bsplineExternal.size() * sizeof(float), bsplineExternal.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);

	//speedway
	glGenVertexArrays(1, &vaoSpeedway);

	glGenBuffers(1, &vboSpeedway);

	glBindVertexArray(vaoSpeedway);
	glBindBuffer(GL_ARRAY_BUFFER, vboSpeedway);
	glBufferData(GL_ARRAY_BUFFER, speedwayPoints.size() * sizeof(float), speedwayPoints.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);
		
		cout << "x: " << xpos << "  y: " << ypos << endl;

		controlPoints.push_back(xpos);
		controlPoints.push_back(ypos);
		controlPoints.push_back(0.0f);

		glm::vec3* point = new glm::vec3(xpos, ypos, 0.0);
		controlPointsVec->push_back(point);

		cout << "tamanho do array de pontos: " << controlPoints.size() << endl;
		
		//a bspline é usada temporariamente só para desenhar as linhas entre os pontos
		bspline.push_back(xpos);
		bspline.push_back(ypos);
		bspline.push_back(0.0f);
		
		glGenVertexArrays(1, &vaoBspline);

		glGenBuffers(1, &vboBspline);

		glBindVertexArray(vaoBspline);
		glBindBuffer(GL_ARRAY_BUFFER, vboBspline);
		glBufferData(GL_ARRAY_BUFFER, bspline.size() * sizeof(float), bspline.data(), GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(0);
		
	}

	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
		
		createBspline();
		createBsplineInternalAndExternal();
		createSpeedway();
		writeTxt();
		writeMtl();
		normalizeInternalExternalCords();
		writeObj();
	}
}

void draw() {
	//control points
	//glBindVertexArray(vaoControlPoints);
	//glDrawArrays(GL_LINE_STRIP, 0, (float)controlPoints.size() / 3);

	//bsplines
	glBindVertexArray(vaoBspline);
	glDrawArrays(GL_LINE_STRIP, 0, (float)bspline.size() / 3);

	//bsplines internal
	glBindVertexArray(vaoBsplineInternal);
	glDrawArrays(GL_LINE_STRIP, 0, (float)bsplineInternal.size() / 3);

	//bsplines external
	glBindVertexArray(vaoBsplineExternal);
	glDrawArrays(GL_LINE_STRIP, 0, (float)bsplineExternal.size() / 3);

	//speedway
	glBindVertexArray(vaoSpeedway);
	glDrawArrays(GL_LINE_STRIP, 0, speedwayPoints.size()/3);
	
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

void createBspline() {
	bspline.clear();
	//int size = controlPoints.size();
	controlPointsVec->push_back(controlPointsVec->at(0));
	controlPointsVec->push_back(controlPointsVec->at(1));
	controlPointsVec->push_back(controlPointsVec->at(2));

	for (int i = 0; i < controlPointsVec->size() -3 ; i++) {
		for (float j = 0; j <= 1; j+= 0.01) {
			float t = j;

			// calculando bspline para x
			GLfloat x = (((-1 * pow(t, 3) + 3 * pow(t, 2) - 3 * t + 1) * controlPointsVec->at(i)->x +
				(3 * pow(t, 3) - 6 * pow(t, 2) + 0 * t + 4) * controlPointsVec->at(i + 1)->x +
				(-3 * pow(t, 3) + 3 * pow(t, 2) + 3 * t + 1) * controlPointsVec->at(i + 2)->x +
				(1 * pow(t, 3) + 0 * pow(t, 2) + 0 * t + 0) * controlPointsVec->at(i + 3)->x) / 6);

			// calculando bspline para y
			GLfloat y = (((-1 * pow(t, 3) + 3 * pow(t, 2) - 3 * t + 1) * controlPointsVec->at(i)->y +
				(3 * pow(t, 3) - 6 * pow(t, 2) + 0 * t + 4) * controlPointsVec->at(i + 1)->y +
				(-3 * pow(t, 3) + 3 * pow(t, 2) + 3 * t + 1) * controlPointsVec->at(i + 2)->y +
				(1 * pow(t, 3) + 0 * pow(t, 2) + 0 * t + 0) * controlPointsVec->at(i + 3)->y) / 6);

			bspline.push_back(x);
			bspline.push_back(y);
			bspline.push_back(0.0);

			glm::vec3* point = new glm::vec3(x, y, 0.0);
			bsplineVec->push_back(point);
		}
	}
	//bspline
	glGenVertexArrays(1, &vaoBspline);

	glGenBuffers(1, &vboBspline);

	glBindVertexArray(vaoBspline);
	glBindBuffer(GL_ARRAY_BUFFER, vboBspline);
	glBufferData(GL_ARRAY_BUFFER, bspline.size() * sizeof(float), bspline.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);
}

void createBsplineInternalAndExternal() {
	bsplineExternal.clear();
	bsplineInternal.clear();
	for (int j = 0; j < bsplineVec->size(); j += 1) {

		glm::vec3* a = bsplineVec->at(j);
		glm::vec3* b;
		
		if (j == bsplineVec->size() - 1) {
			b = bsplineVec->at(0);
		}
		else {
			b = bsplineVec->at(j + 1);
		}

		GLfloat w = b->x - a->x;
		GLfloat h = b->y - a->y;

		if (w == 0 || h == 0) {
			w = b->x - bsplineVec->at(j - 1)->x;
			h = b->y - bsplineVec->at(j - 1)->y;
		}

		// bspline internal
		GLfloat angleInternal = glm::atan(h, w);
		angleInternal -= PI / 2;

		GLfloat CxInternal = glm::cos(angleInternal) * CURVE_DISTANCE + a->x;
		GLfloat CyInternal = glm::sin(angleInternal) * CURVE_DISTANCE + a->y;

		bsplineInternal.push_back(CxInternal);
		bsplineInternal.push_back(CyInternal);
		bsplineInternal.push_back(0.0);

		glm::vec3* pointInternal = new glm::vec3(CxInternal, CyInternal, 0.0);
		bsplineInternalVec->push_back(pointInternal);

		//bspline external
		GLfloat angleExternal = glm::atan(h, w);
		angleExternal += PI / 2;

		GLfloat CxExternal = glm::cos(angleExternal) * CURVE_DISTANCE + a->x;
		GLfloat CyExternal = glm::sin(angleExternal) * CURVE_DISTANCE + a->y;

		bsplineExternal.push_back(CxExternal);
		bsplineExternal.push_back(CyExternal);
		bsplineExternal.push_back(0.0);

		glm::vec3* pointExternal = new glm::vec3(CxExternal, CyExternal, 0.0);
		bsplineExternalVec->push_back(pointExternal);
		
	}

	//bspline internal
	glGenVertexArrays(1, &vaoBsplineInternal);

	glGenBuffers(1, &vboBsplineInternal);

	glBindVertexArray(vaoBsplineInternal);
	glBindBuffer(GL_ARRAY_BUFFER, vboBsplineInternal);
	glBufferData(GL_ARRAY_BUFFER, bsplineInternal.size() * sizeof(float), bsplineInternal.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);

	//bspline external
	glGenVertexArrays(1, &vaoBsplineExternal);

	glGenBuffers(1, &vboBsplineExternal);

	glBindVertexArray(vaoBsplineExternal);
	glBindBuffer(GL_ARRAY_BUFFER, vboBsplineExternal);
	glBufferData(GL_ARRAY_BUFFER, bsplineExternal.size() * sizeof(float), bsplineExternal.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);

}

void createSpeedway() {
	//adiciona os pontos para ligar o inicio com o fim
	bsplineInternalVec->push_back(bsplineInternalVec->at(0));
	bsplineExternalVec->push_back(bsplineExternalVec->at(0));

	for (int i = 0; i < bsplineInternalVec->size()-1; i ++) {
		// Ponto Interno 1
		speedwayVec->push_back(bsplineInternalVec->at(i));
		speedwayPoints.push_back(bsplineInternalVec->at(i)->x);
		speedwayPoints.push_back(bsplineInternalVec->at(i)->y);
		speedwayPoints.push_back(0.0f);
		// Ponto Interno 2
		speedwayVec->push_back(bsplineInternalVec->at(i + 1));
		speedwayPoints.push_back(bsplineInternalVec->at(i + 1)->x);
		speedwayPoints.push_back(bsplineInternalVec->at(i + 1)->y);
		speedwayPoints.push_back(0.0f);
		// Ponto Externo 1
		speedwayVec->push_back(bsplineExternalVec->at(i));
		speedwayPoints.push_back(bsplineExternalVec->at(i)->x);
		speedwayPoints.push_back(bsplineExternalVec->at(i)->y);
		speedwayPoints.push_back(0.0f);

		// Ponto Interno 2
		speedwayVec->push_back(bsplineInternalVec->at(i + 1));
		speedwayPoints.push_back(bsplineInternalVec->at(i + 1)->x);
		speedwayPoints.push_back(bsplineInternalVec->at(i + 1)->y);
		speedwayPoints.push_back(0.0f);
		// Ponto Externo 2
		speedwayVec->push_back(bsplineExternalVec->at(i + 1));
		speedwayPoints.push_back(bsplineExternalVec->at(i + 1)->x);
		speedwayPoints.push_back(bsplineExternalVec->at(i + 1)->y);
		speedwayPoints.push_back(0.0f);
		// Ponto Externo 1
		speedwayVec->push_back(bsplineExternalVec->at(i));
		speedwayPoints.push_back(bsplineExternalVec->at(i)->x);
		speedwayPoints.push_back(bsplineExternalVec->at(i)->y);
		speedwayPoints.push_back(0.0f);

	}
	glGenVertexArrays(1, &vaoSpeedway);

	glGenBuffers(1, &vboSpeedway);

	glBindVertexArray(vaoSpeedway);
	glBindBuffer(GL_ARRAY_BUFFER, vboSpeedway);
	glBufferData(GL_ARRAY_BUFFER, speedwayPoints.size() * sizeof(float), speedwayPoints.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);
}

void writeTxt() {
	ofstream file;
	file.open("output/originalCurve.txt");
	for (int i = 1; i < bsplineVec->size(); i++) {
		convertCoordinates(bsplineVec->at(i)->x, bsplineVec->at(i)->y);
		file << "v " << bsplineVec->at(i)->x << " " << bsplineVec->at(i)->z << " " << bsplineVec->at(i)->y << endl;
	}
	file.close();
}

void writeMtl() {
	ofstream mtlFile;
	mtlFile.open("output/curve.mtl");
	mtlFile << "newmtl " << "road\n" << endl;
	mtlFile << "Kd " << 1.0 << " " << 0.5 << " " << 0 << endl;
	mtlFile << "Ka " << 1.0 << " " << 1.0 << " " << 1.0 << endl;
	mtlFile << "Tf " << 1.0 << " " << 1.0 << " " << 1.0 << endl;
	mtlFile << "map_Kd " << "road.png" << endl;
	mtlFile << "Ni " << 1.0 << endl;
	mtlFile << "Ns " << 100.0 << endl;
	mtlFile << "Ks " << 1.0 << " " << 1.0 << " " << 1.0 << endl;
	mtlFile.close();
}

void writeObj() {
	OBJWriter* objWriter = new OBJWriter(bsplineInternalVec, bsplineExternalVec);
	objWriter->write();
}

void normalizeInternalExternalCords() {
	//convertCoordinates(bsplineInternalVec->at(0)->x, bsplineInternalVec->at(0)->y);
	for (int i = 1; i < bsplineInternalVec->size(); i++) {
		convertCoordinates(bsplineInternalVec->at(i)->x, bsplineInternalVec->at(i)->y);
	}
	//convertCoordinates(bsplineExternalVec->at(0)->x, bsplineExternalVec->at(0)->y);
	for (int i = 1; i < bsplineExternalVec->size(); i++) {
		convertCoordinates(bsplineExternalVec->at(i)->x, bsplineExternalVec->at(i)->y);
	}
}

// convertendo coordenadas da tela para coordenadas graficas, x e y para valores entre -1 0 1
void convertCoordinates(float& x, float& y) {

	// se x for maior que a metade da tela da esquerda
	// ou seja clique na direita
	// resultando em valores entre 0 e 1
	if (x > (WIDTH / 2)) {
		x = x - (WIDTH / 2);
		x = x / (WIDTH / 2);
	}
	// se estiver no meio
	else if (x == (WIDTH / 2)) {
		x = 0;
	}
	// se o clique for na parte esquerda da tela, entao converte para valores entre -1 e 0
	else {
		x = -(((WIDTH / 2) - x) / (WIDTH / 2));
	}

	// se y for maior que a metade de baixo da tela
	// ou seja clique na parte de baixo
	// resultando em valores entre 0 e -1
	if (y > (HEIGHT / 2)) {
		y = y - (HEIGHT / 2);
		y = y / (HEIGHT / 2);
		y = y * (-1);
	}
	// se estiver no meio
	else if (y == (HEIGHT / 2)) {
		y = 0;
	}
	// se o clique for na parte de cima da tela, entao converte para valores entre 0 e 1
	else {
		y = -(((HEIGHT / 2) - y) / (HEIGHT / 2));
		y = y * (-1);
	}
}