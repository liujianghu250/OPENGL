#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "learnopengl/shader_s.h"
#include <iostream>
#include <fstream>
#include <string>
#define STB_IMAGE_IMPLEMENTATION
#include "learnopengl/stb_image.h"
using namespace std;



// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

float vertices[35947 * 3];//顶点的存储
int face[69451 * 3];
int vertexNum;
int faceNum;//面的数目 

glm::mat4 model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
glm::mat4 view = glm::mat4(1.0f);
glm::mat4 projection = glm::mat4(1.0f);

glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

float deltaTime = 0.0f, lastFrame = 0.0f; // 上一帧的时间
float lastX = SCR_WIDTH / 2, lastY = SCR_HEIGHT / 2;
float yaw = -90.0f, pitch = 0.0f;
float fov = 45.0f; //视野
bool firstMouse = true;

// lighting
glm::vec3 lightPos(-0.7f, 0.5f, 1.0f);


void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouse_callback(GLFWwindow *window, double x, double y);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
int loadFile();


int main()
{
	loadFile();

	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

	// glfw window creation
	// --------------------
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// 使用回调函数
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetKeyCallback(window, key_callback);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetScrollCallback(window, scroll_callback);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}
	

	// configure global opengl state
	// -----------------------------
	glEnable(GL_DEPTH_TEST);

	// build and compile our shader program
	// ------------------------------------
	Shader lightShader("shaders/lightshader.vs", "shaders/lightshader.fs");
	Shader bunnyShader("shaders/shader.vs", "shaders/shader.fs"); // you can name your shader files however you like
	

	// set up vertex data (and buffer(s)) and configure vertex attributes
	// ------------------------------------------------------------------
	
	unsigned int VBO, VAO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	// bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(face), face, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	float cube[] = {
	-0.5f, -0.5f, -0.5f,
	 0.5f, -0.5f, -0.5f,
	 0.5f,  0.5f, -0.5f,
	 0.5f,  0.5f, -0.5f,
	-0.5f,  0.5f, -0.5f,
	-0.5f, -0.5f, -0.5f,

	-0.5f, -0.5f,  0.5f,
	 0.5f, -0.5f,  0.5f,
	 0.5f,  0.5f,  0.5f,
	 0.5f,  0.5f,  0.5f,
	-0.5f,  0.5f,  0.5f,
	-0.5f, -0.5f,  0.5f,

	-0.5f,  0.5f,  0.5f,
	-0.5f,  0.5f, -0.5f,
	-0.5f, -0.5f, -0.5f,
	-0.5f, -0.5f, -0.5f,
	-0.5f, -0.5f,  0.5f,
	-0.5f,  0.5f,  0.5f,

	 0.5f,  0.5f,  0.5f,
	 0.5f,  0.5f, -0.5f,
	 0.5f, -0.5f, -0.5f,
	 0.5f, -0.5f, -0.5f,
	 0.5f, -0.5f,  0.5f,
	 0.5f,  0.5f,  0.5f,

	-0.5f, -0.5f, -0.5f,
	 0.5f, -0.5f, -0.5f,
	 0.5f, -0.5f,  0.5f,
	 0.5f, -0.5f,  0.5f,
	-0.5f, -0.5f,  0.5f,
	-0.5f, -0.5f, -0.5f,

	-0.5f,  0.5f, -0.5f,
	 0.5f,  0.5f, -0.5f,
	 0.5f,  0.5f,  0.5f,
	 0.5f,  0.5f,  0.5f,
	-0.5f,  0.5f,  0.5f,
	-0.5f,  0.5f, -0.5f,
	};
	// first, configure the cube's VAO (and VBO)
	unsigned int lightVBO, lightVAO;
	glGenVertexArrays(1, &lightVAO);
	glGenBuffers(1, &lightVBO);

	glBindBuffer(GL_ARRAY_BUFFER, lightVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube), cube, GL_STATIC_DRAW);

	glBindVertexArray(lightVAO);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	//线框模式
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// render loop
	// -----------
	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		// input
		// -----
		processInput(window);

		// render
		// ------
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// render the bunny
		//移动缩放与旋转
		// create transformations
		bunnyShader.use();

		model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
		view = glm::mat4(1.0f);
		projection = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.3f, -0.3f, 0.0f));//平移
		model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.5f, 1.0f, 0.0f));//旋转
		model = glm::scale(model, glm::vec3(5, 5, 5));//缩放

		view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
		projection = glm::perspective(glm::radians(fov), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		
		bunnyShader.setMatrix4fv("model", model);
		bunnyShader.setMatrix4fv("view", view);
		bunnyShader.setMatrix4fv("projection", projection);

		glBindVertexArray(VAO);
		//glDrawArrays(GL_TRIANGLES, 0, 36);
		glDrawElements(GL_TRIANGLES, 3 * faceNum, GL_UNSIGNED_INT, 0);

		lightShader.use();
		model = glm::mat4(1.0f);
		model = glm::translate(model, lightPos);
		model = glm::scale(model, glm::vec3(0.1f));
		lightShader.setMatrix4fv("model", model);
		lightShader.setMatrix4fv("view", view);
		lightShader.setMatrix4fv("projection", projection);
		glBindVertexArray(lightVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		
		
		
		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// optional: de-allocate all resources once they've outlived their purpose:
	// ------------------------------------------------------------------------
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);

	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glfwTerminate();
	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow *window, double mouse_x, double mouse_y){
	if (firstMouse)
	{
		lastX = mouse_x;
		lastY = mouse_y;
		firstMouse = false;
	}
	//对于当前鼠标在屏幕中点击的点的坐标就是2D的视图坐标，该坐标就是windowsd的窗口坐标，窗口左上角（0， 0）， 右下角为（w，h）。
	//将当前的视图坐标转换为标准设备坐标：即x的坐标从左往右为[-1, 1]，y轴的坐标从上到下为[1, -1]，z轴从外到里为[-1, 1]
	//将视图坐标转换为ndc坐标为：
	float x = (2.0f * mouse_x) / SCR_WIDTH - 1.0f;
	float y = 1.0f - (2.0f * mouse_y) / SCR_HEIGHT;
	float z = 1.0f;
	glm::vec3 ray_nds = glm::vec3(x, y, z);
	//将NDC坐标变换到四维齐次坐标，所以这步我们只需要加上w的值即可：
	glm::vec4 ray_clip = glm::vec4(ray_nds, 1.0);
	//由于从视觉坐标变换到裁剪坐标是通过一个投影矩阵来变换的，所以在这我们只需要取得当前的投影矩阵，对投影矩阵求逆即可将坐标从裁剪坐标变换回到视觉坐标：
	glm::vec4 ray_eye = inverse(projection) * ray_clip;
	//由世界坐标变换到视觉坐标，则将当前的世界坐标与视图矩阵做变换，所以为了得到世界坐标，则取得当前的视图矩阵，将当前的视觉坐标与视图矩阵的逆矩阵做变换，即可得到当前的世界坐标：
	glm::vec4 ray_world = inverse(view) * ray_eye;
	//由于我们当前的坐标是带w值的，所以我们除以w，然后取得当前摄像机的位置，即可得到当前的射线：
	if (ray_world.w != 0.0)
	{
		ray_world.x /= ray_world.w;
		ray_world.y /= ray_world.w;
		ray_world.z /= ray_world.w;
	}
	glm::vec3 ray_w = glm::vec3(ray_w.x, ray_w.y, ray_w.z);
	glm::vec3 ray_dir = glm::normalize(ray_w - cameraPos);

	cout << x << "," << y << endl;
	float xoffset = mouse_x - lastX;
	float yoffset = lastY - mouse_y; // 注意这里是相反的，因为y坐标是从底部往顶部依次增大的
	lastX = mouse_x;
	lastY = mouse_y;

	float sensitivity = 0.01f;
	xoffset *= sensitivity;
	yoffset *= sensitivity;
	
	yaw += xoffset;
	pitch += yoffset;
	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	glm::vec3 front;
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraFront = glm::normalize(front);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	float cameraSpeed = 5.0f * deltaTime;
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		cameraPos += cameraSpeed * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		cameraPos -= cameraSpeed * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	if (fov >= 1.0f && fov <= 45.0f)
		fov -= yoffset;
	if (fov <= 1.0f)
		fov = 1.0f;
	if (fov >= 45.0f)
		fov = 45.0f;
}

int loadFile() {


	ifstream in;
	in.open("bunny.ply", ios::in);

	if (!in) {
		//读取文件失败
		cout << "Read File Fail!" << endl;
		return 0;

	}

	//开始读取文件
	string filein;
	while (!in.eof()) {
		in >> filein;
		if (filein == "ply" || filein == "PLY") {
			//文件开头
		}
		else if (filein == "comment") {
			getline(in, filein, '\n');
		}
		else if (filein == "forment") {
			getline(in, filein, '\n');
		}
		else if (filein == "element") {
			in >> filein;
			if (filein == "vertex") {
				in >> vertexNum;
				cout << "顶点个数为" << vertexNum << endl;
				getline(in, filein, '\n');
			}
			else if (filein == "face") {
				in >> faceNum;
				cout << "面个数为" << faceNum << endl;
				getline(in, filein, '\n');
			}
			else {
				cout << "ignore user-defined element!" << endl;
				getline(in, filein, '\n');
			}
		}
		else if (filein == "property") {
			getline(in, filein, '\n');
		}
		else if (filein == "end_header") {
			getline(in, filein, '\n');
			int j = -1, n = -1;
			int nums;//面数据的第一个数据的值，判断是否为3
			float confidence;
			float intensity;
			//添加点信息到数组
			for (int i = 0; i < vertexNum; i++) {
				in >> vertices[++j];
				in >> vertices[++j];
				in >> vertices[++j];

				in >> confidence;
				in >> intensity;
			}
			cout << "confidence: " << confidence << endl;
			cout << "intensity: " << intensity << endl;
			//添加面信息到数组
			for (int k = 0; k < faceNum; k++) {
				in >> nums;
				if (nums == 3) {
					in >> face[++n];
					in >> face[++n];
					in >> face[++n];
				}
			}
			cout << "File read sucessed.";
		}

	}
	in.close();
	return 0;
}