#include<filesystem>
#include<iostream>
#include <Windows.h>
#include"Model.h"
#include <random>
#include <ctime>
#include <C:\Users\bogac\Downloads\Kursach\Bladokrut\Libraries\irrKlang-64bit-1.6.0\include\irrKlang.h>

namespace fs = std::filesystem;


const unsigned int width = 1024;
const unsigned int height = 768;


float skyboxVertices[] =
{
	//   Coordinates
	-1.0f, -1.0f,  1.0f,
	 1.0f, -1.0f,  1.0f,
	 1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f, -1.0f,
	-1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f
};

unsigned int skyboxIndices[] =
{
	// Right
	1, 2, 6,
	6, 5, 1,
	// Left
	0, 4, 7,
	7, 3, 0,
	// Top
	4, 5, 6,
	6, 7, 4,
	// Bottom
	0, 3, 2,
	2, 1, 0,
	// Back
	0, 1, 5,
	5, 4, 0,
	// Front
	3, 7, 6,
	6, 2, 3
};

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//glfwGetPrimaryMonitor()
	GLFWwindow* window = glfwCreateWindow(width, height, "Iddontknow 18+", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	gladLoadGL();

	glViewport(0, 0, width, height);

	Shader shaderProgram("default.vert", "default.frag");
	Shader skyboxShader("skybox.vert", "skybox.frag");

	glm::vec4 lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	glm::vec3 lightPos = glm::vec3(0.5f, 0.5f, 0.5f);
	glm::mat4 lightModel = glm::mat4(1.0f);
	lightModel = glm::translate(lightModel, lightPos);

	skyboxShader.Activate();
	glUniform1i(glGetUniformLocation(skyboxShader.ID, "skybox"), 0);
	glEnable(GL_DEPTH_TEST);

	Camera camera(width, height, glm::vec3(13.5373f, 21.8355f, 10.6905f));

	std::string parentDir = (fs::current_path().fs::path::parent_path()).string();
	std::string modelPathGirl = "/Resource/lowgirl/scene.gltf";
	std::string modelPathGround = "/Resource/ground/scene.gltf";
	std::string modelPathAltar = "/Resource/altar/scene.gltf";
	std::string modelPathVillain = "/Resource/villain/scene.gltf";
	std::string texPath = "/Resource/brick.png";

	Texture brickTex((parentDir + texPath).c_str(), GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE);
	brickTex.texUnit(shaderProgram, "tex1", 0);

	Model modelGirl((parentDir + modelPathGirl).c_str());
	Model modelGround((parentDir + modelPathGround).c_str());
	Model modelAltar((parentDir + modelPathAltar).c_str());
	Model modelVillain1((parentDir + modelPathVillain).c_str());
	Model modelVillain2((parentDir + modelPathVillain).c_str());

	float rotation = 0.25f;
	double befTime = glfwGetTime();

	srand(time(NULL));

	unsigned int skyboxVAO, skyboxVBO, skyboxEBO;
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glGenBuffers(1, &skyboxEBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, skyboxEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(skyboxIndices), &skyboxIndices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	std::string facesCubemap[6] =
	{
		parentDir + "/Resource/right.jpg",
		parentDir + "/Resource/left.jpg",
		parentDir + "/Resource/top.jpg",
		parentDir + "/Resource/bottom.jpg",
		parentDir + "/Resource/front.jpg",
		parentDir + "/Resource/back.jpg"
	};

	unsigned int cubemapTexture;
	glGenTextures(1, &cubemapTexture);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	//glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	for (unsigned int i = 0; i < 6; i++)
	{
		int width, height, nrChannels;
		unsigned char* data = stbi_load(facesCubemap[i].c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			stbi_set_flip_vertically_on_load(false);
			glTexImage2D
			(
				GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0,
				GL_RGB,
				width,
				height,
				0,
				GL_RGB,
				GL_UNSIGNED_BYTE,
				data
			);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Failed to load texture: " << facesCubemap[i] << std::endl;
			stbi_image_free(data);
		}
	}

	for (int i = 0; i < modelGirl.getMatricesMeshes().size(); i++)
	{
		modelGirl.setMatricesMeshes(glm::rotate(modelGirl.getMatricesMeshes()[i], glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f)), i);
	}

	for (int i = 0; i < modelGirl.getMatricesMeshes().size(); i++)
	{
		modelGirl.setMatricesMeshes(glm::rotate(modelGirl.getMatricesMeshes()[i], glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f)), i);
	}

	for (int i = 0; i < modelGirl.getMatricesMeshes().size(); i++)
	{
		modelGirl.setMatricesMeshes(glm::rotate(modelGirl.getMatricesMeshes()[i], glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f)), i);
	}

	for (int i = 0; i < modelGirl.getMatricesMeshes().size(); i++)
	{
		modelGirl.setMatricesMeshes(glm::scale(modelGirl.getMatricesMeshes()[i], glm::vec3(3.0f, 3.0f, 3.0f)), i);
	}

	for (int i = 0; i < modelGirl.getMatricesMeshes().size(); i++)
	{
		modelGirl.setMatricesMeshes(glm::translate(modelGirl.getMatricesMeshes()[i], glm::vec3(10.0f, -10.0f, 0.0f)), i);
	}

	for (int i = 0; i < modelGround.getMatricesMeshes().size(); i++)
	{
		modelGround.setMatricesMeshes(glm::scale(modelGround.getMatricesMeshes()[i], glm::vec3(0.01f, 0.01f, 0.01f)), i);
	}

	for (int i = 0; i < modelAltar.getMatricesMeshes().size(); i++)
	{
		modelAltar.setMatricesMeshes(glm::translate(modelAltar.getMatricesMeshes()[i], glm::vec3(0.0f, -1.35f, 0.0f)), i);
	}

	for (int i = 0; i < modelAltar.getMatricesMeshes().size(); i++)
	{
		modelAltar.setMatricesMeshes(glm::scale(modelAltar.getMatricesMeshes()[i], glm::vec3(2.5f, 2.5f, 2.5f)), i);
	}

	for (int i = 0; i < modelVillain1.getMatricesMeshes().size(); i++)
	{
		modelVillain1.setMatricesMeshes(glm::rotate(modelVillain1.getMatricesMeshes()[i], glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f)), i);
	}

	for (int i = 0; i < modelVillain1.getMatricesMeshes().size(); i++)
	{
		modelVillain1.setMatricesMeshes(glm::scale(modelVillain1.getMatricesMeshes()[i], glm::vec3(0.2f, 0.2f, 0.2f)), i);
	}

	for (int i = 0; i < modelVillain1.getMatricesMeshes().size(); i++)
	{
		modelVillain1.setMatricesMeshes(glm::translate(modelVillain1.getMatricesMeshes()[i], glm::vec3(-15.0f, 100.0f, -60.0f)), i);
	}

	for (int i = 0; i < modelVillain2.getMatricesMeshes().size(); i++)
	{
		modelVillain2.setMatricesMeshes(glm::rotate(modelVillain2.getMatricesMeshes()[i], glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f)), i);
	}

	for (int i = 0; i < modelVillain2.getMatricesMeshes().size(); i++)
	{
		modelVillain2.setMatricesMeshes(glm::rotate(modelVillain2.getMatricesMeshes()[i], glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f)), i);
	}

	for (int i = 0; i < modelVillain2.getMatricesMeshes().size(); i++)
	{
		modelVillain2.setMatricesMeshes(glm::scale(modelVillain2.getMatricesMeshes()[i], glm::vec3(0.2f, 0.2f, 0.2f)), i);
	}

	for (int i = 0; i < modelVillain2.getMatricesMeshes().size(); i++)
	{
		modelVillain2.setMatricesMeshes(glm::translate(modelVillain2.getMatricesMeshes()[i], glm::vec3(30.0f, 102.0f, -5.0f)), i);
	}

	/*int flag1 = 0;
	int flag2 = 0;
	int flag3 = 0;*/

	while (!glfwWindowShouldClose(window))
	{
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		camera.Inputs(window);
		camera.updateMatrix(45.0f, 0.1f, 1000.0f);

		shaderProgram.Activate();
		glUniform4f(glGetUniformLocation(shaderProgram.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
		glUniform3f(glGetUniformLocation(shaderProgram.ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);

		auto iTime = glfwGetTime();

		GLint uTime_location = glGetUniformLocation(shaderProgram.ID, "iTime");
		glUniform1f(uTime_location, (float)iTime);

		if (iTime - befTime >= 1 / 60)
		{
			befTime = iTime;
		}
		brickTex.BindOld();

		for (int i = 0; i < modelVillain1.getMatricesMeshes().size(); i++)
		{
			modelVillain1.setMatricesMeshes(glm::rotate(modelVillain1.getMatricesMeshes()[i], glm::radians(0.8f), glm::vec3(1.0f, 0.0f, 0.0f)), i);
		}

		for (int i = 0; i < modelVillain1.getMatricesMeshes().size(); i++)
		{
			modelVillain1.setMatricesMeshes(glm::translate(modelVillain1.getMatricesMeshes()[i], glm::vec3(0.0f, 1.0f, 0.0f)), i);
		}

		if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
		{
			for (int i = 0; i < modelVillain2.getMatricesMeshes().size(); i++)
			{
				modelVillain2.setMatricesMeshes(glm::translate(modelVillain2.getMatricesMeshes()[i], glm::vec3(0.05f, 0.0f, 0.0f)), i);
			}
		}
		if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
		{
			for (int i = 0; i < modelVillain2.getMatricesMeshes().size(); i++)
			{
				modelVillain2.setMatricesMeshes(glm::translate(modelVillain2.getMatricesMeshes()[i], glm::vec3(-0.05f, 0.0f, 0.0f)), i);
			}
		}
		
		modelGirl.Draw(shaderProgram, camera);
		modelGround.Draw(shaderProgram, camera);
		modelAltar.Draw(shaderProgram, camera);
		modelVillain1.Draw(shaderProgram, camera);
		modelVillain2.Draw(shaderProgram, camera);

		/*for (Model* paun : Pauns)
		{
			paun->Draw(shaderProgram, camera);
		}*/

		glDepthFunc(GL_LEQUAL);

		skyboxShader.Activate();

		GLint uTime_location2 = glGetUniformLocation(skyboxShader.ID, "iTime");
		glUniform1f(uTime_location2, (float)iTime);
		glm::mat4 view = glm::mat4(1.0f);
		glm::mat4 projection = glm::mat4(1.0f);

		view = glm::mat4(glm::mat3(glm::lookAt(camera.Position, camera.Position + camera.Orientation, camera.Up)));
		projection = glm::perspective(glm::radians(45.0f), (float)width / height, 0.1f, 100.0f);
		glUniformMatrix4fv(glGetUniformLocation(skyboxShader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(skyboxShader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

		glBindVertexArray(skyboxVAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		glDepthFunc(GL_LESS);

		//std::cout << Pauns.size() << "\n";

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	shaderProgram.Delete();
	glfwDestroyWindow(window);
	brickTex.Delete();
	glfwTerminate();
	return 0;
}