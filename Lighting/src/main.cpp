#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <windows.h>
#include <map>
#include <glad/glad.h>
#include <KHR/khrplatform.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <sstream> //concatenar number in string
//--------------------------------------------------------------------------------------
extern "C" 
{
#define _CRT_SECURE_NO_DEPRECATE
#include <stdio.h>
#include <stdlib.h>
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
#include <libavutil/opt.h>
#include <libavutil/imgutils.h>
}
//--------------------------------------------------------------------------------------
// compatibility with newer API
#if LIBAVCODEC_VERSION_INT < AV_VERSION_INT(55,28,1)
#define av_frame_alloc avcodec_alloc_frame
#define av_frame_free avcodec_free_frame
#endif
//--------------------------------------------------------------------------------------


#include <SDL.h>
#include <SDL_thread.h>

#include "WinS.h"
#include "Shader.h"
#include "Mesh.h"
#include "Texture.h"
#include "Camera.h"
#include "VideoOut.h"

#include <ft2build.h>
#include FT_FREETYPE_H 

//Para ligar a placa gráfica e não a onboard de bosta, Deve ter o pacote windows.h dica ;)
extern "C" {
	_declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
}


// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;
float currentFrame = 0.0f;

std::string toString(float frm) {
	std::stringstream ss;
	ss << frm;

	return ss.str();
}







// Holds all state information relevant to a character as loaded using FreeType
struct Character {
	GLuint TextureID;   // ID handle of the glyph texture
	glm::ivec2 Size;    // Size of glyph
	glm::ivec2 Bearing;  // Offset from baseline to left/top of glyph
	GLuint Advance;    // Horizontal offset to advance to next glyph
};

std::map<GLchar, Character> Characters;
GLuint VAO, VBO;

void RenderText(Shader &shader, std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color);

int main(int argc, char* argv[]) {
	//--------------------------------------------------------------------------------------
	//--------------------------------------------------------------------------------------
	const char *filename, *codec_name;
	filename = "teste";
	codec_name = "mpeg4";
	//--------------------------------------------------------------------------------------
    //--------------------------------------------------------------------------------------
	// settings
	const unsigned int width = 1024;
	const unsigned int height = 768;

	VideoOut write(filename, codec_name, width, height);

	std::vector<Vertex> vertices = {
		{glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(0.0f, 0.0f)},		//baixo esquerda
		{glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(0.0f, 1.0f)},		//alto esquerda
		{glm::vec3( 0.5f,  0.5f,  0.5f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(1.0f, 1.0f)},		//alto direita
		{glm::vec3( 0.5f, -0.5f,  0.5f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(1.0f, 0.0f)},		//baixo direita
		//Lateral Direita
		{glm::vec3(0.5f, -0.5f,  0.5f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f)},		//baixo esquerda
		{glm::vec3(0.5f,  0.5f,  0.5f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 1.0f)},		//alto esquerda
		{glm::vec3(0.5f,  0.5f, -0.5f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 1.0f)},		//alto direita
		{glm::vec3(0.5f, -0.5f, -0.5f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 0.0f)},		//baixo direita
		//Atrás
		{glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(0.0f, 0.0f)},		//baixo esquerda
		{glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(0.0f, 1.0f)},		//alto esquerda
		{glm::vec3( 0.5f,  0.5f, -0.5f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(1.0f, 1.0f)},		//alto direita
		{glm::vec3( 0.5f, -0.5f, -0.5f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(1.0f, 0.0f)},		//baixo direita
		//Lateral Esquerda
		{glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f)},		//baixo esquerda
		{glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 1.0f)},		//alto esquerda
		{glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 1.0f)},		//alto direita
		{glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 0.0f)},		//baixo direita
		//TOP view
		{glm::vec3(-0.5f, 0.5f,  0.5f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 0.0f)},		//baixo esquerda
		{glm::vec3(-0.5f, 0.5f, -0.5f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 1.0f)},		//alto esquerda
		{glm::vec3( 0.5f, 0.5f, -0.5f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(1.0f, 1.0f)},		//alto direita
		{glm::vec3( 0.5f, 0.5f,  0.5f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(1.0f, 0.0f)},		//baixo direita
		//Bottom view
		{glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(0.0f, 0.0f)},		//baixo esquerda
		{glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(0.0f, 1.0f)},	//alto esquerda
		{glm::vec3( 0.5f, -0.5f, -0.5f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(1.0f, 1.0f)},		//alto direita
		{glm::vec3( 0.5f, -0.5f,  0.5f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(1.0f, 0.0f)},		//baixo direita
	};
	std::vector<unsigned int> indices1 = {
		//Frente
		0, 1, 2,
		2, 3, 0,
		//Lateral Direita		
		4, 5, 6,
		6, 7, 4,
		//Back						
		8, 9, 10,
		10, 11, 8,
		//Lateral Esquerda
		12, 13, 14,
		14, 15, 12,
		//TOP
		16, 17, 18,
		18, 19, 16,
		//BOTTOM
		20, 21, 22,
		22, 23, 20

	};


	WinS window("Lighting", width, height);
	Shader lightShader("./vendor/res/basicShader.vs", "./vendor/res/basicShader.fs");
	Shader lampShader("./vendor/res/basicShader01.vs", "./vendor/res/basicShader01.fs");


	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	// Compile and setup the shader
	Shader shader("./vendor/res/basicShader02.vs", "./vendor/res/basicShader02.fs");
	glm::mat4 projection1 = glm::ortho(0.0f, static_cast<GLfloat>(width), 0.0f, static_cast<GLfloat>(height));
	shader.use();
	//
	//shader.setMat4("projection", glm::value_ptr(projection));
	glUniformMatrix4fv(glGetUniformLocation(shader.getShader(), "projection"), 1, GL_FALSE, glm::value_ptr(projection1));

	Mesh lightMesh(vertices, indices1);

	Mesh lampMesh(vertices, indices1);

	// configure global opengl state
	// -----------------------------
	glEnable(GL_DEPTH_TEST);

	lightShader.use(); // don't forget to activate the shader before setting uniforms!  
	lampShader.use();
	// uncomment this call to draw in wireframe polygons.
	
	// Light
	lightShader.setVec3("objectColor", 1.0f, 0.5f, 0.31f);
	lightShader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
	
	// Lamp
	lampShader.setVec3("objectColor", 1.0f, 1.0f, 1.0f);
	lampShader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);

	// create transformations LIGHT
	glm::mat4 model = glm::mat4(1.0f);																	// make sure to initialize matrix to identity matrix first
	glm::mat4 view = glm::mat4(1.0f);
	glm::mat4 projection = glm::mat4(1.0f);

	Camera camera(view);
	glm::mat4 MVP;
	glm::mat4 MVP1;

	//--------------------------------------------------------------------------------------
	//--------------------------------------------------------------------------------------
	/*
	glPixelStorei(GL_UNPACK_SWAP_BYTES, GL_FALSE);
	glPixelStorei(GL_UNPACK_LSB_FIRST, GL_TRUE);
	glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
	glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);
	glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);
	
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	//glPixelStorei(GL_PACK_ALIGNMENT, 1);
	*/
	//unsigned char *pixels = new unsigned char[width * height * 4];
	//--------------------------------------------------------------------------------------
	//--------------------------------------------------------------------------------------
	// FreeType
	FT_Library ft;
	// All functions return a value different than 0 whenever an error occurred
	if (FT_Init_FreeType(&ft))
		std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;

	// Load font as face
	FT_Face face;
	if (FT_New_Face(ft, "fonts/arial.ttf", 0, &face))
		std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;

	// Set size to load glyphs as
	FT_Set_Pixel_Sizes(face, 0, 48);

	// Disable byte-alignment restriction
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	// Load first 128 characters of ASCII set
	for (GLubyte c = 0; c < 128; c++)
	{
		// Load character glyph 
		if (FT_Load_Char(face, c, FT_LOAD_RENDER))
		{
			std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
			continue;
		}
		// Generate texture
		GLuint texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RED,
			face->glyph->bitmap.width,
			face->glyph->bitmap.rows,
			0,
			GL_RED,
			GL_UNSIGNED_BYTE,
			face->glyph->bitmap.buffer
		);
		// Set texture options
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		// Now store character for later use
		Character character = {
			texture,
			glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
			glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
			face->glyph->advance.x
		};
		Characters.insert(std::pair<GLchar, Character>(c, character));
	}
	glBindTexture(GL_TEXTURE_2D, 0);
	// Destroy FreeType once we're finished
	FT_Done_Face(face);
	FT_Done_FreeType(ft);

	// Configure VAO/VBO for texture quads
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	//--------------------------------------------------------------------------------------
	//--------------------------------------------------------------------------------------
	std::string fps;
	float auxx = 0 ;
	float med[10];
	unsigned char cou = 0;
	while(!window.isClosed()) {
		// per-frame time logic
		// --------------------
		currentFrame = (float)SDL_GetTicks();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		// --------------------
		med[cou] = (1/deltaTime)*1000;
		cou++;
		if (cou == 10) {
			for (int k = 0; k < 10; k++) {
				auxx = auxx + med[k];
			}
			fps = "FPS: " + toString(auxx/10);
			cou = 0;
			auxx = 0;
		}


		//Clear the colorbuffer
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// change the light's position values over time (can be done anywhere in the render loop actually, but try to do it at least before using the light source positions)
		// set light position
		float lightX = 2.0f * sin(glm::radians((float)SDL_GetTicks() / 10));
		float lightY = 0.0f;
		float lightZ = 1.5f * cos(glm::radians((float)SDL_GetTicks() / 10));
		glm::vec3 lightPos = glm::vec3(lightX, lightY, lightZ);
		//lightPos.x = 1.0f + sin(glm::radians((float)SDL_GetTicks()/10)) * 2.0f;
		//lightPos.y = -0.3f ;
		//lightPos.z = 1.0f + sin(glm::radians((float)SDL_GetTicks() / 10)) * 2.0f;
		// Light
		lightShader.use();
		lightShader.setVec3("objectColor", 1.0f, 0.5f, 0.31f);
		lightShader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
		lightShader.setVec3("lightPos", lightPos);
		lightShader.setVec3("viewPos", camera.cameraPos);
		
		
		// view/projection transformations
		projection = glm::perspective(camera.Zoom, (float)width / (float)height, 0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();

		// world transformation
		glm::mat4 model = glm::mat4(1.0f);
		//model = glm::rotate(model, glm::radians(45.0f), glm::vec3(0.5f, 0.0f, 0.0f));
		MVP = projection * view * model;
		
		lightShader.setMat4("MVP", MVP);
		lightShader.setMat4("model", model);
		lightMesh.draw(lightShader, 0);
		


		// Lamp
		lampShader.use();
		//glm::mat4 model = glm::mat4(1.0f);
		model = glm::mat4(1.0f);
		model = glm::translate(model, lightPos);
		model = glm::scale(model, glm::vec3(0.2f));
		
		MVP1 = projection * view * model;
		
		lampShader.setMat4("MVP", MVP1);
		lampMesh.draw(lampShader, 0);
		



		// FONT ARIAL
		RenderText(shader, fps.c_str(), 25.0f, 25.0f, 1.0f, glm::vec3(0.5, 0.8f, 0.2f));
		window.pollEvents(camera, deltaTime);

		//textu.bind();
	
		// SDL2: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		
		//--------------------------------------------------------------------------------------
		//--------------------------------------------------------------------------------------
		// 4 bytes per pixel (RGBA), 1x1 bitmap	
		//glReadPixels(0, 0, width, height, GL_BGRA, GL_UNSIGNED_BYTE, pixels);
		//write.writeFrame(pixels);
		//--------------------------------------------------------------------------------------
		//--------------------------------------------------------------------------------------
	}

	return 0;
}

void RenderText(Shader &shader, std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color)
{
	// Activate corresponding render state	
	shader.use();
	glUniform3f(glGetUniformLocation(shader.getShader(), "textColor"), color.x, color.y, color.z);
	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(VAO);

	// Iterate through all characters
	std::string::const_iterator c;
	for (c = text.begin(); c != text.end(); c++)
	{
		Character ch = Characters[*c];

		GLfloat xpos = x + ch.Bearing.x * scale;
		GLfloat ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

		GLfloat w = ch.Size.x * scale;
		GLfloat h = ch.Size.y * scale;
		// Update VBO for each character
		GLfloat vertices[6][4] = {
			{ xpos,     ypos + h,   0.0, 0.0 },
			{ xpos,     ypos,       0.0, 1.0 },
			{ xpos + w, ypos,       1.0, 1.0 },

			{ xpos,     ypos + h,   0.0, 0.0 },
			{ xpos + w, ypos,       1.0, 1.0 },
			{ xpos + w, ypos + h,   1.0, 0.0 }
		};
		// Render glyph texture over quad
		glBindTexture(GL_TEXTURE_2D, ch.TextureID);
		// Update content of VBO memory
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // Be sure to use glBufferSubData and not glBufferData

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		// Render quad
		glDrawArrays(GL_TRIANGLES, 0, 6);
		// Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
		x += (ch.Advance >> 6) * scale; // Bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
	}
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}


// 01 - Disable compiler extensions to ensure your programs (and coding practices) remain compliant with C++ standards and will work on any system.
//Ir em Propriedades C++ linguagem
// 02 - To increase your warning levels, right click on your project name in the Solution Explorer window, then choose Properties: