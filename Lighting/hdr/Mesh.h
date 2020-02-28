#include <string>
#include <iostream>
#include <vector>

#include <glm/glm.hpp>
#include <glad/glad.h>

#include "Shader.h"

struct Vertex {
	// position
	glm::vec3 Position;
	// normal
	glm::vec3 Normal;
	// texCoords
	glm::vec2 TexCoords;
	// tangent
	glm::vec3 Tangent;
	// bitangent
	glm::vec3 Bitangent;
};
/*
struct Texture {
	unsigned int id;
	std::string type;
	std::string path;
};
*/
class Mesh
{
public:

	/*  Mesh Data  */
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	//std::vector<Texture> textures;

	Mesh(const std::vector<Vertex>& vertices, std::vector<unsigned int> indices); 
	//Mesh(std::vector<Vertex> vertices, unsigned int numVertices); //Pode-se ter muitos triangulos por isso a quantidade de vertices
	//std::vector<Vertex> vertices;
	void draw(Shader &shader, unsigned int texture);

	~Mesh();

protected:

private:

	/*  Render data  */
	unsigned int VAO, VBO, EBO;
	/*  Functions    */
	void setupMesh();
};

