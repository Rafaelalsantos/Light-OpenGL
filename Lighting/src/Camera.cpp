#include "Camera.h"

Camera::Camera(glm::mat4& view) : cameraPos(glm::vec3(0.0f, 0.0f, 3.0f)), cameraFront(glm::vec3(0.0f, 0.0f, -1.0f)), cameraUp(glm::vec3(0.0f, 1.0f, 0.0f))
{
	WorldUp = cameraUp;
	Zoom = ZOOM;
	view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
	updateCameraVectors();
}

// Processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
void Camera::processKeyboard(Camera_Movement direction, float deltaTime)
{
	float velocity = speedCamera * deltaTime;
	if (direction == FORWARD)
		cameraPos += cameraFront * velocity;
	if (direction == BACKWARD)
		cameraPos -= cameraFront * velocity;
	if (direction == LEFT)
		cameraPos -= Right * velocity;// glm::normalize(glm::cross(cameraFront, cameraUp)) * velocity;
	if (direction == RIGHT)
		cameraPos += Right * velocity;//glm::normalize(glm::cross(cameraFront, cameraUp)) * velocity;
}

// Processes input received from a mouse input system. Expects the offset value in both the x and y direction.
void Camera::processMouseMovement(float xoffset, float yoffset)
{
	xoffset *= speedMouse;
	yoffset *= speedMouse;

	yaw += xoffset;
	pitch += yoffset;

	// Make sure that when pitch is out of bounds, screen doesn't get flipped
	if (true)
	{
		if (pitch > 89.0f)
			pitch = 89.0f;
		if (pitch < -89.0f)
			pitch = -89.0f;
	}

	// Update Front, Right and Up Vectors using the updated Euler angles
	updateCameraVectors();
}

// Calculates the front vector from the Camera's (updated) Euler Angles
void Camera::updateCameraVectors()
{
	// Calculate the new Front vector
	glm::vec3 front;
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraFront = glm::normalize(front);
	// Also re-calculate the Right and Up vector
	Right = glm::normalize(glm::cross(cameraFront, WorldUp));  // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
	cameraUp = glm::normalize(glm::cross(Right, cameraFront));
}
// Processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
void Camera::processMouseScroll(float yoffset, float deltaTime)
{
	float velocity = 0.01* deltaTime;
	if (Zoom >= 1.0f && Zoom <= 45.0f)
		Zoom -= yoffset*velocity;
	if (Zoom <= 1.0f)
		Zoom = 1.0f;
	if (Zoom >= 45.0f)
		Zoom = 45.0f;
}


// Returns the view matrix calculated using Euler Angles and the LookAt Matrix
glm::mat4 Camera::GetViewMatrix()
{
	return glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
}

Camera::~Camera() {

}
