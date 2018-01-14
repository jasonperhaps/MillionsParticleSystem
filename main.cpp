#include <windows.h>
#include <stdio.h>
#include "glew.h"
#include "Glm/glm.hpp"
#include "GLm/ext.hpp"
#include "camera.h"
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "glu32.lib")
#pragma comment(lib, "winmm.lib")

Camera camera;

struct Vertex
{
	float pos[4];
};

float frandom()//0-1
{
	return (float)rand() / (float)RAND_MAX;
}

float sfrandom()
{
	return frandom()*2.0f-1.0f;
}

POINT originalPos;
bool bRotateView;
LRESULT CALLBACK GLWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_LBUTTONDOWN:
		originalPos.x = LOWORD(lParam);
		originalPos.y = HIWORD(lParam);
		ClientToScreen(hwnd, &originalPos);
		SetCapture(hwnd);
		ShowCursor(false);
		bRotateView = true;
		break;
	case WM_LBUTTONUP:
		bRotateView = false;
		SetCursorPos(originalPos.x, originalPos.y);
		ReleaseCapture();
		ShowCursor(true);
		break;
	case WM_MOUSEMOVE:
		if (bRotateView)
		{
			POINT currentPos;
			currentPos.x = LOWORD(lParam);
			currentPos.y = HIWORD(lParam);
			ClientToScreen(hwnd, &currentPos);
			int deltaX = currentPos.x - originalPos.x;
			int deltaY = currentPos.y - originalPos.y;
			float angleRotateByRight = (float)deltaY / 500.0f;
			float angleRotateByUp = (float)deltaX / 500.0f;
			camera.Yaw(angleRotateByUp);
			camera.Pitch(angleRotateByRight);
			
			SetCursorPos(originalPos.x, originalPos.y);
		}
	case WM_KEYUP:
		switch (wParam)
		{
		case 'A':
			camera.mbMoveLeft = false;
			break;
		case 'D':
			camera.mbMoveRight = false;
			break;
		case 'W':
			camera.mbMoveForward = false;
		case 'S':
			camera.mbMoveBackWard = false;
		case 'Q':
			camera.mbRotateLeft = false;
			break;
		case 'E':
			camera.mbRotateRight = false;
			break;
		}
		break;
	case WM_KEYDOWN:
		switch (wParam)
		{
		case 'A':
			camera.mbMoveLeft = true;
			break;
		case 'D':
			camera.mbMoveRight = true;
			break;
		case 'W':
			camera.mbMoveForward = true;
			break;
		case 'S':
			camera.mbMoveBackWard = true;
			break;
		case 'Q':
			camera.mbRotateLeft = true;
			break;
		case 'E':
			camera.mbRotateRight = true;
			break;
		}
		break;
	case WM_CLOSE:
		PostQuitMessage(0);
		break;
	}
	return DefWindowProc(hwnd, msg, wParam, lParam);
}

char *LoadFileContent(const char*path)
{
	FILE*pFile = fopen(path, "rb");
	if (pFile)
	{
		fseek(pFile, 0, SEEK_END);
		int nLen = ftell(pFile);
		
		char*buffer = new char[nLen + 1];
		rewind(pFile);
		fread(buffer, nLen, 1, pFile);
		buffer[nLen] = '\0';
		fclose(pFile);
		return buffer;
	}
	fclose(pFile);
	return nullptr;
}

GLuint CreateGPUProgram(const char*vsShaderPath, const char*fsShaderPath)
{
	GLuint vsShader = glCreateShader(GL_VERTEX_SHADER);
	GLuint fsShader = glCreateShader(GL_FRAGMENT_SHADER);
	const char*vsCode = LoadFileContent(vsShaderPath);
	const char*fsCose = LoadFileContent(fsShaderPath);
	glShaderSource(vsShader, 1, &vsCode, nullptr);
	glShaderSource(fsShader, 1, &fsCose, nullptr);
	glCompileShader(vsShader);
	glCompileShader(fsShader);
	GLuint program = glCreateProgram();
	glAttachShader(program, vsShader);
	glAttachShader(program, fsShader);
	glLinkProgram(program);
	glDetachShader(program, vsShader);
	glDetachShader(program, fsShader);
	glDeleteShader(vsShader);
	glDeleteShader(fsShader);

	return program;
}

GLuint CreateElementArrayBufferObject(int particleCount)
{
	unsigned int *indexes = new unsigned int[particleCount * 6];
	unsigned int *temp = indexes;
	for (size_t i = 0; i < (size_t)particleCount; ++i)
	{
		unsigned int index = unsigned int(i << 2);
		*temp++ = index;
		*temp++ = index+1;
		*temp++ = index+2;
		*temp++ = index;
		*temp++ = index+2;
		*temp++ = index+3;
	}
	
	GLuint ibo;
	glGenBuffers(1, &ibo);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ibo);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(unsigned int)*particleCount * 6, indexes, GL_STATIC_DRAW);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	return ibo;
}

GLuint CreateTexture3D(int w, int h, int d)
{
	char *data = new char[w*h*d*4];
	char *temp = data;
	for (int i=0;i<w;++i)
	{
		for (int ii=0;ii<h;++ii)
		{
			for (int iii=0;iii<d;++iii)
			{
				*temp++ = rand() & 0xff;
				*temp++ = rand() & 0xff;
				*temp++ = rand() & 0xff;
				*temp++ = 0;
			}
		}
	}
	
	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_3D, texture);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA8_SNORM, w, h, d, 0, GL_RGBA, GL_BYTE, data);
	delete data;
	return texture;
}

INT WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{
	WNDCLASSEX wndclass;
	wndclass.cbClsExtra = 0;
	wndclass.cbSize = sizeof(WNDCLASSEX);
	wndclass.cbWndExtra = 0;
	wndclass.hbrBackground = NULL;
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hIcon = NULL;
	wndclass.hIconSm = NULL;
	wndclass.hInstance = hInstance;
	wndclass.lpfnWndProc = GLWndProc;
	wndclass.lpszClassName = L"OpenGl";
	wndclass.lpszMenuName = NULL;
	wndclass.style = CS_VREDRAW | CS_HREDRAW;
	ATOM atom = RegisterClassEx(&wndclass);

	HWND hwnd = CreateWindowEx(NULL, L"OpenGL", L"RenderWindow", WS_OVERLAPPEDWINDOW,
		100, 100, 800, 600, NULL, NULL, hInstance, NULL);

	HDC dc = GetDC(hwnd);
	PIXELFORMATDESCRIPTOR pfd;
	memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR));
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_TYPE_RGBA | PFD_DOUBLEBUFFER;
	pfd.iLayerType = PFD_MAIN_PLANE;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cDepthBits = 24;
	pfd.cStencilBits = 8;

	int pixelFormatID = ChoosePixelFormat(dc, &pfd);
	SetPixelFormat(dc, pixelFormatID, &pfd);
	//在绘制窗口之前初始化opengl绘图环境;
	HGLRC rc = wglCreateContext(dc);
	wglMakeCurrent(dc, rc);
	glewInit();//after
	GLint program = CreateGPUProgram("sample.vs", "sample.fs");
	GLuint mainTexture = CreateTexture3D(16, 16, 16);
	const char*csCode = LoadFileContent("sample.cs");
	GLuint csProgram = glCreateShaderProgramv(GL_COMPUTE_SHADER, 1, &csCode);
	GLint textureLocation = glGetUniformLocation(csProgram, "U_Texture");
	glUniform1i(textureLocation, 0);
	GLint posLocation, MLocation, VLocation, PLocation;
	posLocation = glGetAttribLocation(program, "pos");

	MLocation = glGetUniformLocation(program, "M");
	VLocation = glGetUniformLocation(program, "V");
	PLocation = glGetUniformLocation(program, "P");

	int nParticleCount = 1<<20;
	Vertex *vertex = new Vertex[nParticleCount];
	for (int i=0;i<nParticleCount;++i)
	{
		vertex[i].pos[0] = sfrandom();
		vertex[i].pos[1] = sfrandom();
		vertex[i].pos[2] = sfrandom();
		vertex[i].pos[3] = 1.0f;
	}

	Vertex *velocity = new Vertex[nParticleCount];
	for (int i = 0; i < nParticleCount; ++i)
	{
		velocity[i].pos[0] = 0.0f;
		velocity[i].pos[1] = 0.0f;
		velocity[i].pos[2] = 0.0f;
		velocity[i].pos[3] = 1.0f;
	}
	

	GLuint vbo, ssbo_v;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, vbo);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(Vertex) * nParticleCount, vertex, GL_STATIC_DRAW);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	glGenBuffers(1, &ssbo_v);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo_v);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(Vertex) * nParticleCount, velocity, GL_STATIC_DRAW);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	delete vertex;
	delete velocity;
	GLuint ibo = CreateElementArrayBufferObject(nParticleCount);
	
	glClearColor(41.0f/255.0f, 71.0f/255.0f, 121.0f/255.0f, 1.0f);
	
	ShowWindow(hwnd, SW_SHOW);
	UpdateWindow(hwnd);

	float identity[] = {
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	};
	//glm::mat4 model = glm::translate(glm::vec3(0.0f, 0.0f, 10.0f));
	glm::mat4 projection = glm::perspective(45.0f,
		800.0f / 600.0f, 0.1f, 1000.0f);
	glm::mat4 model = glm::translate(glm::vec3(0.0f, 0.0f, -4.0f))*glm::rotate(45.0f, glm::vec3(1.0f, 1.0f, 1.0f));
	static float sTimeSinceStartUp = timeGetTime() / 1000.0f;
	MSG msg;
	while (true)
	{
		if (PeekMessage(&msg, NULL, NULL, NULL, PM_REMOVE))
		{
			if (msg.message==WM_QUIT)
			{
				break;
			}
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		//绘制图形;
		float currentTime = timeGetTime() / 1000.0f;
		float timeElapse = currentTime - sTimeSinceStartUp;
		sTimeSinceStartUp = currentTime;
		//设置相机;
		model = model*glm::rotate(0.01f, glm::vec3(0.0f, 1.0f, 0.0f));
 		camera.Update(0.016f);//1/60
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
		glUseProgram(csProgram);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, vbo);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, ssbo_v);
		glDispatchCompute(nParticleCount / 128, 1, 1);
		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, 0);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, 0);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		glUseProgram(program);
		glUniformMatrix4fv(MLocation, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(VLocation, 1, GL_FALSE, glm::value_ptr(camera.mView));
		glUniformMatrix4fv(PLocation, 1, GL_FALSE, glm::value_ptr(projection));

		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, vbo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
		glDrawElements(GL_TRIANGLES, nParticleCount * 6, GL_UNSIGNED_INT, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, 0);
		glUseProgram(0);
		SwapBuffers(dc);
	}
	return 0;
}