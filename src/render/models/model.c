/****************************************************************************
* Title   : Tawy   
* Filename: model.h
* Author  : Guillaume Mantelet
* Version : 1.0.0
* Brief   : This module manages a model, enabling and disabling buffers, and
*           loading vertices and textures from file.
*******************************************************************************/
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "model.h"


/*******************************************************************************
* Function  : textures_to_buffer
* Brief     : Transfer static texture coordinates to OpenGL array buffer.
* Parameters:
*    1. path    : The instance of the model
* Returns   :
*    true : Unconditional
*******************************************************************************/
static bool textures_to_buffer(model *obj)
{
  unsigned int buffer;

  float textures[] = 
  {
    0.0f, 0.0f,
    1.0f, 0.0f,
    1.0f, 1.0f,
    1.0f, 1.0f,
    0.0f, 1.0f,
    0.0f, 0.0f,

    0.0f, 0.0f,
    1.0f, 0.0f,
    1.0f, 1.0f,
    1.0f, 1.0f,
    0.0f, 1.0f,
    0.0f, 0.0f,

    1.0f, 0.0f,
    1.0f, 1.0f,
    0.0f, 1.0f,
    0.0f, 1.0f,
    0.0f, 0.0f,
    1.0f, 0.0f,

    1.0f, 0.0f,
    1.0f, 1.0f,
    0.0f, 1.0f,
    0.0f, 1.0f,
    0.0f, 0.0f,
    1.0f, 0.0f,

    0.0f, 1.0f,
    1.0f, 1.0f,
    1.0f, 0.0f,
    1.0f, 0.0f,
    0.0f, 0.0f,
    0.0f, 1.0f,

    0.0f, 1.0f,
    1.0f, 1.0f,
    1.0f, 0.0f,
    1.0f, 0.0f,
    0.0f, 0.0f,
    0.0f, 1.0f
};

  glGenBuffers(1, &buffer);
  glBindBuffer(GL_ARRAY_BUFFER, buffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(textures), textures, GL_STATIC_DRAW);

  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(2);
  return true;
}


/*******************************************************************************
* Function  : vertices_to_buffer
* Brief     : Transfer static vertex coordinates to OpenGL array buffer.
* Parameters:
*    1. path    : The instance of the model
* Returns   :
*    true : Unconditional
*******************************************************************************/
static bool vertices_to_buffer(model *obj)
{
  float vertices[] = 
  {
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
    -0.5f,  0.5f, -0.5f
  };

  obj->vertices = 36;
  glGenBuffers(1, &obj->vbo);
  glBindBuffer(GL_ARRAY_BUFFER, obj->vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(0);
  return true;
}


/*******************************************************************************
* Function  : load_model
* Brief     : obj model to OpenGL vertex array object.
* Parameters:
*    1. path    : The instance of the model
*    2. filename: The filename to extract obj model from.
* Returns   :
*    true : Successfully converted obj to vertex array object.
*    false: File could not be located, or file is somehow corrupt.
*******************************************************************************/
static bool load_model(model *obj)
{ 
  vertices_to_buffer(obj);
  textures_to_buffer(obj);
  return true;
}


/*******************************************************************************
* Function  : Model__init__
* Brief     : The object initializer, called by new()
* Parameters:
*    1. self    : The instance of the model.
* Returns   :
*    true : The model is created, ready to be displayed.
*    false: Either the .obj file or the texture file could not been retrieved
*           properly. They may be missing, or have corrupt data, or data written
*           in a way that could not been handled successfully.
*******************************************************************************/
static bool Model__init__(void *self, va_list *args)
{
  model        *obj = self;
  char         *p;
  unsigned int  cnt = 0;

  //
  // 1. Create arrays and buffers: VAO, VBO and EBO
  //
  glGenVertexArrays(1, &obj->vao);
  glBindVertexArray(obj->vao);
  
  //
  // 2. Retrieve .obj file. Build vertices and indices from it. Vertex array and
  //    index array will be sent to VBO and EBO respectively.
  //
  load_model(obj);

  //
  // 4. Load textures from file.
  //
  while (true)
  {
    p = va_arg(*args, char *);
    if (!p) break;
    printf("Loading texture: %s\n", p);
    obj->texture[cnt++] = new(Texture, p);
  }  

  obj->texture_cnt = cnt;
  return true;
}


/*******************************************************************************
* Function  : Model__enable__
* Brief     : Enable our buffers before rendering them.
* Parameters:
*    1. self    : The instance of the program.
* Returns   :
*    true : Unconditional
*******************************************************************************/
static bool Model__enable__(void *self)
{
  model *obj = self;

  for (unsigned int i = 0; i < obj->texture_cnt; i++)
  {
    glActiveTexture(GL_TEXTURE0 + i);
    glBindTexture(GL_TEXTURE_2D, obj->texture[i]->id);
  }

  glBindVertexArray(obj->vao);
  glDrawArrays(GL_TRIANGLES, 0, obj->vertices);
  glBindVertexArray(0);
  return true;
}


/*******************************************************************************
* Class     : _Model
* Brief     : The class definition and its handlers
*******************************************************************************/
static const class _Model = {
  .size             = sizeof(model),
  .__init__         = Model__init__,
  .__del__          = NULL,
  .__get__          = NULL,
  .__set__          = NULL,
  .__should_close__ = NULL,
  .__prepare__      = NULL,
  .__enable__       = Model__enable__,
};


/*******************************************************************************
* Class     : Model
* Brief     : Defines a class that will handle our basic functions.
*******************************************************************************/
const void *Model = &_Model;
