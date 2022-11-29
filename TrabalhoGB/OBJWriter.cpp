#include "OBJWriter.h"


OBJWriter::OBJWriter(vector<glm::vec3*>* internalVec, vector<glm::vec3*>* externalVec) {
	this->internalVec = internalVec;
	this->externalVec = externalVec;
}

void OBJWriter::write() {
	this->objFile.open("output/curve.obj", std::ofstream::out | std::ofstream::trunc);
	this->createOBJFile();
	this->writeVertices();
	this->writeTextures();
	this->writeNormals();
	this->writeFaces();
	this->objFile.close();
}

void OBJWriter::createOBJFile() {
	this->objFile << "mtllib " << "curve.mtl" << "\n" << std::endl;
	this->objFile << "g " << "road" << "\n" << std::endl;
	this->objFile << "usemtl road\n" << std::endl;
}

void OBJWriter::writeVertices() {
	for (int i = 0; i < internalVec->size(); i++) {
		this->objFile << "v " << internalVec->at(i)->x << " " << internalVec->at(i)->z << " " << internalVec->at(i)->y << std::endl;
	}
	for (int i = 0; i < externalVec->size(); i++) {
		this->objFile << "v " << externalVec->at(i)->x << " " << externalVec->at(i)->z << " " << externalVec->at(i)->y << std::endl;
	}
}

void OBJWriter::writeTextures() {
	this->objFile << std::endl;
	this->objFile << "vt " << "0.0" << " " << "0.0" << std::endl;
	this->objFile << "vt " << "0.0" << " " << "1.0" << std::endl;
	this->objFile << "vt " << "1.0" << " " << "0.0" << std::endl;
	this->objFile << "vt " << "1.0" << " " << "1.0" << std::endl;
	this->objFile << std::endl;
}

void OBJWriter::writeNormals() {
	for (int i = 0; i < internalVec->size() - 1; i++) {
		glm::vec3* a = internalVec->at(i);
		glm::vec3* b = internalVec->at(i + 1);
		glm::vec3* c = externalVec->at(i);
		glm::vec3* d = externalVec->at(i + 1);

		// produto escalar
		glm::vec3 ab = glm::vec3(b->x - a->x, b->z - a->z, b->y - a->y);
		glm::vec3 ac = glm::vec3(c->x - a->x, c->z - a->z, c->y - a->y);
		glm::vec3 dc = glm::vec3(c->x - d->x, c->z - d->z, c->y - d->y);
		glm::vec3 db = glm::vec3(b->x - d->x, b->z - d->z, b->y - d->y);

		// prooduto vetorial
		glm::vec3 normalAbac = glm::cross(ac, ab);
		glm::vec3 normalDbdc = glm::cross(db, dc);
		
		this->objFile << "vn " << normalAbac[0] << " " << normalAbac[1] << " " << normalAbac[2] << std::endl;
		this->objFile << "vn " << normalDbdc[0] << " " << normalDbdc[1] << " " << normalDbdc[2] << std::endl;
	}
}

void OBJWriter::writeFaces() {
	int facesCount = 0;

	for (int i = 0; i < internalVec->size() - 1; i++) {
		int j = i+1;
		this->objFile <<
			"f " <<
			j                       << "/" << 1 << "/" << ++facesCount << " " <<
			j + 1                   << "/" << 2 << "/" << facesCount << " " <<
			j + internalVec->size() << "/" << 4 << "/" << facesCount <<
			std::endl;

		this->objFile <<
			"f " <<
			j + internalVec->size()     << "/" << 4 << "/" << ++facesCount << " " <<
			j + 1                       << "/" << 2 << "/" << facesCount << " " <<
			j + 1 + internalVec->size() << "/" << 3 << "/" << facesCount <<
			std::endl;
	}
}