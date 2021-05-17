/****************************************************************************
* Title   : Tawy   
* Filename: model.h
* Author  : Guillaume Mantelet
* Version : 1.0.0
* Brief   : This module manages a model, enabling and disabling buffers, and
*           loading vertices and textures from file.
*******************************************************************************/
#ifndef __TAWY__MODEL_H__
#define __TAWY__MODEL_H__
#include "texture.h"

/*******************************************************************************
* Struct    : model
* Brief     : Defines an instance of a model that is potentially shared between
*             multiple entities.
* Attributes:
*    1. vao  : The OpenGL Vertex Array Object for this instance.
*    2. vbo  : The OpenGL Vertex Buffer Object for this instance.
*    3. ebo  : The OpenGL Element Buffer Object for this instance.
* Class     : The structure storing our handlers.
*******************************************************************************/
typedef struct model
{
  const void *__cls__;

  unsigned int vao;
  unsigned int vbo;
  unsigned int ebo;

  unsigned int vertices;
  unsigned int elements;

  float        *coordinates;
  unsigned int *indices;  // Temporary. But could be useful if I want to reshape later.


  texture      *texture[16]; // Allowing max 16 textures per model.
  unsigned int  texture_cnt;
}model;


/*******************************************************************************
* Class     : Model
* Brief     : Defines a class that will handle our basic functions.
*******************************************************************************/
extern const void *Model;

extern const void *AssimpModel;

#endif