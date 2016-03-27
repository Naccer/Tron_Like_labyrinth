#pragma once
#include "glm.hpp"


class FreeflyCamera
{
	private:
		glm::vec3 m_Position;
		float m_fPhi; // en radinans
		float  m_fTheta; // en radians
		glm::vec3 m_FrontVector;
		glm::vec3 m_LeftVector;
		glm::vec3 m_UpVector;
	public:
		FreeflyCamera();
        	void computeDirectionVectors();
		void moveLeft(float t);
		void moveFront(float t);
		void rotateLeft(float degrees);
		void rotateUp(float degrees);
		glm::mat4 getViewMatrix() const;
		void displayPosition();
		
		glm::vec3 getPosition()
		{return m_Position;}
		
		float getPhi()
		{return m_fPhi;}
		
		float getTheta()
		{return m_fTheta;}
		
		glm::vec3 getFrontVector()
		{return m_FrontVector;}
		
		glm::vec3 getLeftVector()
		{return m_LeftVector;}
		
		glm::vec3 getUpVector()
		{return m_UpVector;}

		glm::vec3 getNormal()
		{
			return m_FrontVector + m_LeftVector + m_UpVector;
		}
		
};
