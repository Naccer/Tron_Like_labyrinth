#include "glimac/FreeflyCamera.hpp"
#include "glimac/common.hpp"
#include <cmath>
#include <glm/gtc/random.hpp>
#include <iostream>

const double PI = std::atan(1.0)*4;

using namespace glm;
	FreeflyCamera::FreeflyCamera() {
		m_Position = glm::vec3(0.f,0.f,6.f);
		m_fPhi = PI;
		m_fTheta = 0;
		computeDirectionVectors();
	}

    void FreeflyCamera::computeDirectionVectors() {
		m_FrontVector = glm::vec3(cos(m_fTheta)*sin(m_fPhi), sin(m_fTheta), cos(m_fTheta)*cos(m_fPhi));
		m_LeftVector = glm::vec3(sin(m_fPhi + PI/2), 0, cos(m_fPhi + PI/2));
		m_UpVector = glm::cross(m_FrontVector, m_LeftVector);
		//std::cout << "F(" << m_FrontVector << ")\tL(" << m_LeftVector << ")\tU(" << m_UpVector << ")" << std::endl;
	}
	void FreeflyCamera::displayPosition()
	{
		std::cout << "Camera position: " << m_Position << std::endl;
		std::cout << "Front vector: " << m_FrontVector << std::endl;
		std::cout << "Left vector: " << m_LeftVector << std::endl;
		
	}

	void FreeflyCamera::moveLeft(float t) {
		m_Position += t*m_LeftVector;
	}
	
	void FreeflyCamera::moveFront(float t) {
		m_Position += t*m_FrontVector;
	}
	
	void FreeflyCamera::rotateLeft(float degrees) {
		float angle = radians(degrees);
		//std::cout << m_fPhi << " -> ";
		m_fPhi += angle;
		//std::cout << m_fPhi << std::endl;
	}

	void FreeflyCamera::rotateUp(float degrees) {
		float angle = radians(degrees);
		//std::cout << m_fTheta << " -> ";
		m_fTheta += angle;
		//std::cout << m_fTheta << std::endl;
	}

	glm::mat4 FreeflyCamera::getViewMatrix() const {
		glm::vec3 target = m_Position + m_FrontVector;
		return glm::lookAt(m_Position, target, m_UpVector);
	}
