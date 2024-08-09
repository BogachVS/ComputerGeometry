#ifndef TEXTURE_CLASS_H
#define TEXTURE_CLASS_H

#include<glad/glad.h>
#include<stb/stb_image.h>

#include"shaderClass.h"

class Texture
{
public:
	GLuint ID;
	const char* type;
	GLenum type2;
	GLuint unit;

	Texture(const char* image, const char* texType, GLuint slot);
	Texture(const char* image, GLenum texType, GLenum slot, GLenum format, GLenum pixelType);

	// Assigns a texture unit to a texture
	void texUnit(Shader& shader, const char* uniform, GLuint unit);
	// Binds a texture
	void Bind();
	void BindOld();
	// Unbinds a texture
	void Unbind();
	void UnbindOld();
	// Deletes a texture
	void Delete();
};
#endif