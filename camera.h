#pragma once 
#include "Glm/glm.hpp"
#include "GLm/ext.hpp"
#include "vector3f.h"

class Camera
{
protected:
	void RotateView(float angle, float x, float y,
		float z);
public:
	Camera();
	glm::mat4 mView, mModel, mProj;
	Vector3f mPos, mViewCenter, mUp;
	bool mbMoveLeft, mbMoveRight, 
		mbMoveForward, mbMoveBackWard, 
		mbRotateLeft, mbRotateRight;
	void Yaw(float angle);
	void Pitch(float angle);
	void Update(float deltaTime);
};