#include "stdafx.h"
#include "classes/system/Shader.h"
#include "classes/system/Scene.h"

GLFWwindow* window;
bool Pause;
bool keys[1024] = {0};
int WindowWidth = 800, WindowHeight = 600;
bool EnableVsync = 1;
double DeltaTime = 0;

MShader Shader;
MScene Scene;

static void error_callback(int error, const char* description) {
    fprintf(stderr, "Error: %s\n", description);
}

static void mousepos_callback(GLFWwindow* window, double x, double y) {
}

static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
}

void window_size_callback(GLFWwindow* window, int width, int height) {
	Scene.Size(width, height);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GLFW_TRUE);
		return;
	}
}

bool InitApp() {
	LogFile<<"Starting application"<<endl;    
    glfwSetErrorCallback(error_callback);
    
    if(!glfwInit()) return false;
    window = glfwCreateWindow(WindowWidth, WindowHeight, "TestApp", NULL, NULL);
    if(!window) {
        glfwTerminate();
        return false;
    }
    glfwSetKeyCallback(window, key_callback);
    glfwSetWindowSizeCallback(window, window_size_callback);
    glfwSetCursorPosCallback(window, mousepos_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwMakeContextCurrent(window);
    if(glfwExtensionSupported("WGL_EXT_swap_control")) {
    	LogFile<<"Window: V-Sync supported. V-Sync: "<<EnableVsync<<endl;
		glfwSwapInterval(EnableVsync);//0 - disable, 1 - enable
	}
	else LogFile<<"Window: V-Sync not supported"<<endl;
    LogFile<<"Window created: width: "<<WindowWidth<<" height: "<<WindowHeight<<endl;

	//glew
	glewExperimental = GL_TRUE;
	GLenum Error = glewInit();
	if(GLEW_OK != Error) {
		LogFile<<"Window: GLEW Loader error: "<<glewGetErrorString(Error)<<endl;
		return false;
	}
	LogFile<<"GLEW initialized"<<endl;
	
	if(!CheckOpenglSupport()) return false;

	//shaders
	if(!Shader.CreateShaderProgram("shaders/main.vertexshader.glsl", "shaders/main.fragmentshader.glsl")) return false;
	if(!Shader.AddUnifrom("model", "model")) return false;
	if(!Shader.AddUnifrom("view", "view")) return false;
	if(!Shader.AddUnifrom("proj", "proj")) return false;
	LogFile<<"Shaders loaded"<<endl;

	//scene
	if(!Scene.Initialize(WindowWidth, WindowHeight)) return false;
	LogFile<<"Scene initialized"<<endl;

	//randomize
    srand(time(NULL));
    LogFile<<"Randomized"<<endl;
    
    //use shader program
	glUseProgram(Shader.GetProgramId());
	glUniformMatrix4fv(Shader.GetUniformId("view"), 1, GL_FALSE, Scene.GetMatrixView());
	glUniformMatrix4fv(Shader.GetUniformId("proj"), 1, GL_FALSE, Scene.GetMatrixProjection());
	glUniformMatrix4fv(Shader.GetUniformId("model"), 1, GL_FALSE, Scene.GetMatrixModel());
    
    //other initializations
	
	//turn off pause
	Pause = false;
    
    return true;
}

void KeysProcessing() {
	if(keys[GLFW_KEY_ESCAPE]) {
		glfwSetWindowShouldClose(window, GLFW_TRUE);
		return;
	}
}

void UpdateStep() {
}

void RenderStep() {
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	//draw functions
}

void ClearApp() {
	//clear funstions
	
	memset(keys, 0, 1024);
	Shader.Close();
	LogFile<<"Application: closed"<<endl;
}

int main(int argc, char** argv) {
	const double LimitFPS = (double) 1.0 / 60.0;
	int Frames = 0;
	int Updates = 0;
	double LastTime, NowTime = 0;
	double RenderTimer = 0;
	
	LogFile<<"Application: started"<<endl;
	if(!InitApp()) {
		ClearApp();
		glfwTerminate();
		LogFile.close();
		return 0;
	}

	while(!glfwWindowShouldClose(window)) {
		NowTime = glfwGetTime();
		DeltaTime = NowTime - LastTime;
		RenderTimer += DeltaTime;
		LastTime = NowTime;
		
		if(!Pause) UpdateStep();
		if(RenderTimer >= LimitFPS) {
			RenderStep();
			RenderTimer -= LimitFPS;
		}
		
        glfwSwapBuffers(window);
		glfwPollEvents();
		KeysProcessing();
	}
	ClearApp();
    glfwTerminate();
    LogFile.close();
}
