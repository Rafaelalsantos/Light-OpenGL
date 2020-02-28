#include "WinS.h"
#include <glad/glad.h>

WinS::WinS(const std::string &title, int width, int height) {
	
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		std::cerr << "Falied to initialize SDL\n";
	}
	window = SDL_CreateWindow(
		title.c_str(),
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		width, height,
		SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE 
	);

	if (window == nullptr) {
		std::cerr << "Falied to create window.\n";
		closed = true;
	}

	if (1) {
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
		context = SDL_GL_CreateContext(window);
		
		initOpenGL(width,height);
	}

}
WinS::~WinS() {
	SDL_DestroyWindow(window);
	SDL_Quit();
}
void WinS::initOpenGL(int width, int height) {
	// Load GL extensions using glad
	if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
		std::cerr << "Failed to initialize the OpenGL context." << std::endl;
		exit(1);
	}
	glViewport(0, 0, width, height);
	//glPixelStorei(GL_PACK_ALIGNMENT, 1);
	// Set OpenGL options
	//glEnable(GL_CULL_FACE);
	//glEnable(GL_BLEND);									//
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}



void WinS::pollEvents(Camera& camera, float deltaTime) {
	//SDL_SetWindowTitle(window, "A");
	//float cameraSpeed = 0.05 * deltaTime;
	//static GLubyte *pixels = NULL;
	SDL_GL_SwapWindow(window);
	//glReadPixels(0, 0, 800.0f, 600.0f, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
	SDL_Event event;
	if (SDL_PollEvent(&event)) {
		switch (event.type) {
		case SDL_QUIT:
			closed = true;
			break;
		case SDL_KEYDOWN:
			switch (event.key.keysym.sym) {
				case SDLK_w:
					camera.processKeyboard(FORWARD, deltaTime);
					break;
				case SDLK_a:
					camera.processKeyboard(RIGHT, deltaTime);
					break;
				case SDLK_d:
					camera.processKeyboard(LEFT, deltaTime);
					break;
				case SDLK_s:
					camera.processKeyboard(BACKWARD, deltaTime);
					break;
				default:
					break;
			}
			break;
		case SDL_MOUSEMOTION:
			camera.processMouseMovement(event.motion.xrel, event.motion.yrel);
			break;
		case SDL_MOUSEWHEEL:
			camera.processMouseScroll(event.wheel.y, deltaTime);
			break;
		default:
			break;
		}
	}
}