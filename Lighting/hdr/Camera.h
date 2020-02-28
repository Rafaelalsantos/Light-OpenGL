#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum Camera_Movement {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};

const float ZOOM = 45.0f;

class Camera
{
public:
	Camera(glm::mat4& view);
	~Camera();

	glm::vec3 cameraPos;
	float Zoom;
	glm::mat4 GetViewMatrix();
	void processMouseMovement(float xoffset, float yoffset);
	void processKeyboard(Camera_Movement direction, float deltaTime);
	void processMouseScroll(float yoffset, float deltaTime);
private:
	float speedCamera = 0.01;
	float speedMouse = 0.1;
	// Euler Angles
	float yaw = -90.0f;
	float pitch = 0.0f;


	// camera
	glm::vec3 cameraFront; 
	glm::vec3 cameraUp; 
	glm::vec3 Right;
	glm::vec3 WorldUp;

	void updateCameraVectors();
};

