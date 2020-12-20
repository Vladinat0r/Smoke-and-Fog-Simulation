#ifndef _threeDMODEL_H
#define _threeDMODEL_H


#include "..//3DStruct//BoundingBox.h"
#include "..//Structures//Vector3d.h"
#include "..//Structures//Vector2d.h"
#include <windows.h>
#include <set>
#include <vector>
#include <string>

#include <gl/gl.h>
#include <gl/glu.h>


class Shader;

using namespace std;

class aMaterial {
public:
	char matName[64];     //Name of material
	char fileName[255];   //Name of the texture on disk
	float ambient[4];  //Ambient color
	float diffuse[4];  //Diffuse color
	float specular[4]; //Specular color
	float shininess;   //Matl shininess
	int textureID;			//Texturemap
	bool hasTexture;

	aMaterial() //defualt const
	{
		matName[0] = ' ';
		fileName[0] = ' ';
		
		ambient[0] = ambient[1] = ambient[2] = ambient[3] = 0.0f;
		diffuse[0] = diffuse[1] = diffuse[2] = diffuse[3] = 0.0f;
		specular[0] = specular[1] = specular[2] = specular[3] = 0.0f;

		shininess = 0;
		textureID = 0;
		hasTexture = false;
	}

};


class aFace {
	public:		
	unsigned int thePoints[3]; //index into the Vert List
	unsigned int theFaceNormal; //index into the Face Normal List;
	unsigned int theTexCoord[3]; //index into the TexCoords List;

	unsigned int materialId ; //index into the Material List

	Vector3d genNormal(Vector3d *theVerts) {
		Vector3d result = (theVerts[thePoints[1]] - theVerts[thePoints[0]]) * (theVerts[thePoints[2]] - theVerts[thePoints[0]]);
		return result;
	}

	bool operator < (const aFace b) const
	{
		if (materialId < b.materialId) return true;
		else return false;
	}
};


class Octree;


class ThreeDModel {
public:
	string fileName;

	int numberOfVertices;  //Store the size of the Vert Array
	int numberOfFaceNormals;   //Either same as above for smooth, or faces for flat shading
	int numberOfVertNormals; 
	int numberOfTriangles; //Store the size of the Face Array
	int numberOfTexCoords; 
	int numberOfMaterials;

	BoundingBox theBBox;	
	
	Vector3d * theVerts;
	Vector3d * theFaceNormals;
	Vector3d * theVertNormals;
	Vector2d * theTexCoords;

	unsigned int* indexArray;
	std::vector<unsigned int> startPoints;
	std::vector<unsigned int> length;	

	GLfloat *vertexPositionList;
	GLfloat *vertexNormalList;
	GLfloat *vertexTexCoordList;
	GLfloat *vertexColorList;
	GLuint *faceIDsList;
	GLuint *glBuffer;
	GLuint m_vaoID;
	void initVBO(Shader* myShader);
	void deleteVertexFaceData();

	aFace * theFaces;
	aMaterial * theMaterials;

	Octree* octree;

	glm::vec3 BVPoints[4];
	glm::vec3 updatedBVPoints[4];
	float BVRadius[4];
	glm::vec3 colNormal;
	int xCentre;
	int yCentre;

	// *************** Methods *****************

	ThreeDModel();
	~ThreeDModel();
	ThreeDModel(const ThreeDModel& p);	//Make an assignment opperator

	void operator=(const ThreeDModel &p);//for Vector	
	
	float calcSidePointPlane(glm::vec3 theBVPoint, float BVRadius, glm::vec3 point, glm::vec3 theFaceNormal);	//Calculates distance between a point with radius and a plane
	bool isOctreeCollision(Octree *octree2, glm::mat4 modelMatrix2, glm::vec3 updatedBVPoint);	//Checks for octree collisions

	void calcFaceNormals();
	void calcVertNormals();
	void calcVertNormals(int* VertList, int VertListSize, int* TriList, int TriListSize);
	void calcVertNormalsUsingOctree();
	void calcFakeVertNormals();
	void calcCentrePoint();
	void constructOctree();
	
	void centreOnZero();
	void adjustBoundingBox();

	void scale(float scaleAmount);
	void translate(Vector3d transVec);

	//Draw Methods 
	virtual void drawElementsUsingVBO(Shader* myShader);
	
	void initDrawElements();
	void sortFacesOnMaterial();

	//Methods useful for octree
	int getOctreePrimitiveListSize();
	int getOctreeVertexListSize();
	bool isPrimitiveIntersectingOctreeCell(double boxcenter[3],double boxhalfsize[3], int PrimIndex);
	bool isVertexIntersectingOctreeCell(double min[3], double max[3], int VertIndex);
	void calcBoundingBox(double& minX, double& minY, double& minZ, double& maxX, double& maxY, double& maxZ);
	void drawBoundingBox(Shader* myShader);
	void drawOctreeLeaves(Shader* myShader);

	//End of octree methods

	//Returns corner points of octree bounding boxes
	glm::vec4 getLTF(glm::mat4 modelMatrix);
	glm::vec4 getLTB(glm::mat4 modelMatrix);
	glm::vec4 getLBF(glm::mat4 modelMatrix);
	glm::vec4 getLBB(glm::mat4 modelMatrix);
	glm::vec4 getRTF(glm::mat4 modelMatrix);
	glm::vec4 getRTB(glm::mat4 modelMatrix);
	glm::vec4 getRBF(glm::mat4 modelMatrix);
	glm::vec4 getRBB(glm::mat4 modelMatrix);
	///End of point retrieval

	glm::vec4 getCurrPos(glm::vec4 modelMatrix);	//Returns current position

	void updatePoints(glm::mat4 modelMatrix);	//Updates all collision points' position on a drone

	//Collision reaction
	void collisionFront(float & speedY, float & speedZ);
	void collisionRear(float & speedY, float & speedZ);
	void collisionLeft(float & speedX, float & speedY);
	void collisionRight(float & speedX, float & speedY);
	///End of collision reaction
};

#endif