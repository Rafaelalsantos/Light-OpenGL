#pragma once

#include <iostream>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <SDL.h>

#include "Camera.h"

class WinS {
public:

	WinS(const std::string &title, int width, int height);	// Constructor and Destructor
	~WinS();

	inline bool isClosed() const { return closed; };		// Aprende o porque usar inline e se um jeito aceitável de usar
	void pollEvents(Camera& camera, float deltaTime);

private:
	bool closed = false;

	void initOpenGL(int width, int height);

	SDL_Window *window = nullptr;
	SDL_GLContext context;
};
