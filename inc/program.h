/****************************************************************************
* Title   : Tawy   
* Filename: program.h
* Author  : Guillaume Mantelet
* Version : 1.0.0
* Brief   : This module compiles shaders and link them to a program.
*******************************************************************************/
#ifndef __TAWY__PROGRAM_H__
#define __TAWY__PROGRAM_H__
#include "object.h"


typedef enum 
{
  UNIFORM_BOOL,
  UNIFORM_INT,
  UNIFORM_FLOAT,
  UNIFORM_MAT4,
} uniform_type;

/*******************************************************************************
* Struct    : program
* Brief     : Defines an instance of a program containing its shaders/
* Attributes:
*    1. id               : The identifier of the linked shaders program
*    2. fragment_shader  : The OpenGL Fragment shader
*    3. vertex_shader    : The OpenGL Vertex shader
* Class     : The structure storing our handlers.
*******************************************************************************/
typedef struct program
{
  const void *__cls__;

  unsigned int id;
  unsigned int fragment_shader;
  unsigned int vertex_shader;

}program;


/*******************************************************************************
* Class     : Program
* Brief     : Defines a class that will handle our basic functions.
*******************************************************************************/
extern const void *Program;
#endif