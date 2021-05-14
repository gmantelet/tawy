/****************************************************************************
* Title   : Tawy   
* Filename: texture.c
* Author  : Guillaume Mantelet
* Version : 1.0.0
* Brief   : This module manages a texture, from its loading from file.
*******************************************************************************/
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"




/*******************************************************************************
* Function  : Texture__init__
* Brief     : The object initializer, called by new()
* Parameters:
*    1. self    : The instance of the model.
*    2. filename: The filename of the texture to load.
* Returns   :
*    true : 
*    false: 
*******************************************************************************/
static bool Texture__init__(void *self, va_list *args)
{
  texture       *obj        = self;
  unsigned char *data;
  char           path[1024] = "res/textures/";

  glGenTextures(1, &obj->id);
  glBindTexture(GL_TEXTURE_2D, obj->id);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  strcat(path, va_arg(*args, char *));
  stbi_set_flip_vertically_on_load(true);
  if (NULL != (data = stbi_load(path, &obj->width, &obj->height, &obj->number_channels, 0)))
  {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, obj->width, obj->height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data);
    return true;
  }

  printf("Error, failed to load texture '%s'\n", path);
  return false;
}


/*******************************************************************************
* Class     : _Texture
* Brief     : The class definition and its handlers
*******************************************************************************/
static const class _Texture = {
  .size             = sizeof(texture),
  .__init__         = Texture__init__,
  .__del__          = NULL,
  .__get__          = NULL,
  .__set__          = NULL,
  .__should_close__ = NULL,
  .__refresh__      = NULL,
  .__enable__       = NULL,
};


/*******************************************************************************
* Class     : Texture
* Brief     : Defines a class that will handle our basic functions.
*******************************************************************************/
const void *Texture = & _Texture;
