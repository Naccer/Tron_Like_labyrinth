#include "glimac/TrackballCamera.hpp"
#include "glimac/common.hpp"
#include <cmath>
#include <glm/gtc/random.hpp>
using namespace glm;
		TrackballCamera::TrackballCamera()
		{
			m_fDistance = 5;
			m_fAngleX = 0;
			m_fAngleY = 0;
		}
		
		glm::mat4 translate(float tx, float ty, float tz)
		{
			glm::mat4 T = glm::mat4(vec4(1, 0, 0, 0), vec4(0, 1, 0, 0), vec4(0, 0, 1, 0), vec4(tx, ty, tz, 1));
			return T;
		}

		glm::mat4 TrackballCamera::scale(float sx, float sy, float sz) 
		{
			glm::mat4 S =  glm::mat4(vec4(sx, 0, 0, 0), vec4(0, sy, 0, 0), vec4(0, 0, sz, 0), vec4(0, 0, 0, 1));
			return S;
		}

		glm::mat4 rotateX(float b)
		{
			float a = radians(b);
			glm::mat4 R =  glm::mat4(vec4(1, 0, 0, 0), vec4(0, cos(a), -sin(a), 0), vec4(0, sin(a), cos(a), 0), vec4(0, 0, 0, 1));
			return R;
		}

		glm::mat4 rotateY(float b)
		{
			float a = radians(b);
			glm::mat4 R =  glm::mat4(vec4(cos(a), 0, sin(a), 0), vec4(0, 1, 0, 0), vec4(-sin(a), 0, cos(a), 0), vec4(0, 0, 0, 1));
			return R;
		}

		glm::mat4 rotateZ(float b)
		{
			float a = radians(b);
			glm::mat4 R =  glm::mat4(vec4(cos(a), -sin(a), 0, 0), vec4(sin(a), cos(a), 0, 0), vec4(0, 0, 1, 0), vec4(0, 0, 0, 1));
			return R;
		}

                void TrackballCamera::moveFront(float delta)
		{
			m_fDistance -= delta;
		}
		void TrackballCamera::rotateLeft(float degrees)
		{
			m_fAngleY -= degrees;
		}
		void TrackballCamera::rotateUp(float degrees)
		{
			m_fAngleX += degrees;
		}
		glm::mat4 TrackballCamera::getViewMatrix() const
		{
			glm::mat4 result = translate(0,0,m_fDistance)*rotateX(m_fAngleX)*rotateY(m_fAngleY);	
			return result;
		}


