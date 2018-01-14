#include "camera.h"
#include <windows.h>
#include <gl/GL.h>
#include <gl/GLU.h>

Camera::Camera():mPos(0.0f, 0.0f, 0.0f), 
mViewCenter(0.0f, 0.0f, -1.0f),
mUp(0.0f, 1.0f, 0.0f), mbMoveRight(false),
mbMoveLeft(false), mbMoveForward(false), 
mbMoveBackWard(false)
{

}

void Camera::RotateView(float angle, float x, float y, float z)
{
	Vector3f viewDirection = mViewCenter - mPos;
	Vector3f newDirection;
	float C = cosf(angle);
	float S = sinf(angle);

	Vector3f tempX(C + x*x*(1 - C), x*y*(1 - C) - z*S,
		x*z*(1 - C) + y*S);
	newDirection.x = tempX*viewDirection;

	Vector3f tempY(x*y*(1 - C) + z*S, C + y*y*(1 - C),
		y*z*(1 - C) - x*S);
	newDirection.y = tempY*viewDirection;

	Vector3f tempZ(x*z*(1 - C) - y*S, y*z*(1 - C) + x*S, C + z*z*(1 - C));
	newDirection.z = tempZ*viewDirection;

	mViewCenter = mPos + newDirection;
}

void Camera::Yaw(float angle)
{
	RotateView(angle, mUp.x, mUp.y, mUp.z);
}

void Camera::Pitch(float angle)
{
	//算出strife
	Vector3f viewDirection = mViewCenter - mPos;
	viewDirection.Normalize();
	Vector3f rightDirection = viewDirection^mUp;
	rightDirection.Normalize();
	RotateView(angle, rightDirection.x,
		rightDirection.y, rightDirection.z);
}

void Camera::Update(float deltaTime)
{
	//更新;
	float moveSpeed = 10.0f;
	float rotateSpeed = 1.0f;
	if (mbMoveLeft)
	{
		//摄像机的左方向;
		Vector3f viewDirection = mViewCenter - mPos;
		viewDirection.Normalize();
		Vector3f leftDirection = mUp^viewDirection;
		leftDirection.Normalize();
		mPos = mPos + leftDirection*moveSpeed*deltaTime;
		mViewCenter = mViewCenter + leftDirection*moveSpeed*deltaTime;

	}
	if (mbMoveRight)
	{
		Vector3f viewDirection = mViewCenter - mPos;
		viewDirection.Normalize();
		Vector3f rightDirection = viewDirection^mUp;
		rightDirection.Normalize();
		mPos = mPos + rightDirection*moveSpeed*deltaTime;
		mViewCenter = mViewCenter + rightDirection*moveSpeed*deltaTime;
	}
	if (mbMoveForward)
	{
		//摄像机的左方向;
		Vector3f forwardDirection = mViewCenter - mPos;
		forwardDirection.Normalize();
		mPos = mPos + forwardDirection*moveSpeed*deltaTime;
		mViewCenter = mViewCenter + forwardDirection*moveSpeed*deltaTime;
	}
	if (mbMoveBackWard)
	{
		Vector3f backDirection = mPos - mViewCenter;
		backDirection.Normalize();
		mPos = mPos + backDirection*moveSpeed*deltaTime;
		mViewCenter = mViewCenter + backDirection*moveSpeed*deltaTime;
	}
	if (mbRotateLeft)
	{
		RotateView(rotateSpeed*deltaTime, mUp.x, mUp.y, mUp.z);
	}
	if (mbRotateRight)
	{
		RotateView(-rotateSpeed*deltaTime, mUp.x, mUp.y, mUp.z);
	}
	//设置矩阵;
	mView = glm::lookAt(glm::vec3(mPos.x, mPos.y, mPos.z),
		glm::vec3(mViewCenter.x, mViewCenter.y, mViewCenter.z),
		glm::vec3(mUp.x, mUp.y, mUp.z));
	//
}