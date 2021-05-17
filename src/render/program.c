/****************************************************************************
* Title   : Tawy   
* Filename: program.c
* Author  : Guillaume Mantelet
* Version : 1.0.0
* Brief   : This module compiles shaders and link them to a program.
*******************************************************************************/
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "program.h"

#define SHADER_CODE_MAX_LEN 2048


/******************************************************************************
* Function  : read_glsl()
* Brief     : Parse a target file to extract GLSL source code.
* Parameters:
*     1. p: Path to the file to open and read from.
*     2. c: Buffer to store file content to.
* Returns   : 
*     true : The file is copied to a source buffer, ready to be compiled.
*     false: The file could not be open
*******************************************************************************/
static bool read_glsl(const char *p, char *c)
{
  FILE *f;
  char path[1024] = "src/render/glsl/";
  strcat(path, p);

  if (NULL == (f = fopen(path, "r")))
  {
    printf("Error opening file at %s\n", p);
    return false;
  }

  if ((SHADER_CODE_MAX_LEN - 1) == fread(c, sizeof(char), SHADER_CODE_MAX_LEN - 1, f))
  {
    printf("Warning, code at %s may be too long (%d bytes) and may have been truncated\n", path, SHADER_CODE_MAX_LEN - 1);
  }

  fclose(f);  
  return true;
}


/******************************************************************************
* Function  : compile_status()
* Brief     : Validate shader compilation was successful. Print log else.
* Parameters:
*     1. shader: Shader handle
*     2. type  : Type of shader (fragment, vertex...)
* Returns   : 
*     bool : The result of compilation for this shader.
*******************************************************************************/
static bool compile_status(unsigned int *shader, int type)
{
  int success;
  char info[512];
  glGetShaderiv(*shader, GL_COMPILE_STATUS, &success);

  if (!success)
  {
    glGetShaderInfoLog(*shader, 512, NULL, info);
    switch (type)
    {
      case GL_VERTEX_SHADER: 
        printf("Vertex shader compilation error:\n\t%s\n", info);
        break;

      case GL_FRAGMENT_SHADER:
        printf("Fragment shader compilation error:\n\t%s\n", info);
        break;
    }
  }

  return (bool) success;
}


/******************************************************************************
* Function  : link_status()
* Brief     : Validate shaders could be linked to a program.
* Parameters:
*     1. program: Program handle
* Returns   : 
*     bool : The result of linking for this program.
*******************************************************************************/
static bool link_status(unsigned int *program)
{
  int success;
  char info[512];
  glGetProgramiv(*program, GL_LINK_STATUS, &success);

  if (!success)
  {
    glGetShaderInfoLog(*program, 512, NULL, info);
    printf("Program linking error:\n\t%s\n", info);
  }

  return (bool) success;  
}


/******************************************************************************
* Function  : compile_shader()
* Brief     : Compile shader from GLSL source code.
* Parameters:
*     1. shader: Shader handle
*     2. type  : Type of shader (fragment, vertex...)
*     3. source: GLSL source code for this shader.
* Returns   : 
*     bool : The result of compilation for this shader.
*******************************************************************************/
static bool compile_shader(unsigned int *shader, int type, const char *source)
{
  *shader = glCreateShader(type);
  glShaderSource(*shader, 1, &source, NULL);
  glCompileShader(*shader);

  return compile_status(shader, type);
}


/******************************************************************************
* Function  : link_program()
* Brief     : Link shader to a program.
* Parameters:
*     1. program: Program handle
* Returns   : 
*     bool : The result of linking for this program.
*******************************************************************************/
static bool link_program(program *obj)
{
  obj->id = glCreateProgram();
  glAttachShader(obj->id, obj->vertex_shader);
  glAttachShader(obj->id, obj->fragment_shader);
  glLinkProgram(obj->id);

  return link_status(&obj->id);
}


/******************************************************************************
* Function  : delete_shaders()
* Brief     : Delete a set of predefined shaders from OpenGL.
* Parameters:
*     1. obj: A pointer to the program to build
* Returns   : True
*******************************************************************************/
static bool delete_shaders(program *obj)
{
  glDeleteShader(obj->vertex_shader);
  glDeleteShader(obj->fragment_shader);
  return true;
}


/*******************************************************************************
* Function  : Program__init__
* Brief     : The object initializer, called by new()
* Parameters:
*    1. self    : The instance of the model.
* Returns   :
*    true : 
*    false: 
*******************************************************************************/
static bool Program__init__(void *self, va_list *args)
{
  char     vc[SHADER_CODE_MAX_LEN] = {0};
  char     fc[SHADER_CODE_MAX_LEN] = {0};
  program *obj                     = self;        

  return read_glsl(va_arg(*args, char *), vc)                          & 
         read_glsl(va_arg(*args, char *), fc)                          &
         compile_shader(&obj->vertex_shader, GL_VERTEX_SHADER, vc)     &
         compile_shader(&obj->fragment_shader, GL_FRAGMENT_SHADER, fc) &
         link_program(obj)                                             &
         delete_shaders(obj);
}


/*******************************************************************************
* Function  : Program__enable__
* Brief     : Ask OpenGL permission to use our program
* Parameters:
*    1. self    : The instance of the program.
* Returns   :
*    true : Unconditional
*******************************************************************************/
static bool Program__enable__(void *self)
{
  program *obj = self;
  glUseProgram(obj->id);
  return true;
}


/*******************************************************************************
* Function  : Program__get__
* Brief     : Last resort attribute fetcher. Useful if the struct is private.
* Parameters:
*    1. self    : The instance of the renderer.
*    2. attr    : The attribute name to get.
*    3. value   : The pointer to the void pointer to store attribute value.
* Returns   :
*    true : The instance owns the requested attribute.
*    false: The instance does not own the requested attribute, or the value 
*           could not be transferred.
*******************************************************************************/
static bool Program__get__(void *self, const char *attr, void **value)
{
  program *obj = self;
  int location = glGetUniformLocation(obj->id, attr);

  if (location == -1)
  {
    printf("Error, program %u has no attribute named '%s'\n", obj->id, attr);
    *value = 0;
    return false;
  }

  *value = &location;  
  return false;
}


/*******************************************************************************
* Function  : Program__set__
* Brief     : Set a uniform on an instance of a program.
* Parameters:
*    1. self    : The instance of the class.
*    2. attr    : The attribute name to set.
*    3. value   : The pointer to the value to transfer to target attribute.
*    4. args    : The type of uniform to set.
* Returns   :
*    true : The instance owns the requested attribute.
*    false: The instance does not own the requested attribute, or the value 
*           could not be transferred.
*******************************************************************************/
static bool Program__set__(void *self, const char *attr, void *value, va_list *args)
{
  program *obj = self;
  int location = glGetUniformLocation(obj->id, attr);

  if (location == -1)
  {
    printf("Error, program %u has no attribute named '%s'\n", obj->id, attr);
    return false;
  }

  switch (va_arg(*args, int))
  {
    case UNIFORM_BOOL:
    case UNIFORM_INT:
      glUniform1i(location, *(int *)value);
      return true;

    case UNIFORM_FLOAT:
      glUniform1f(location, *(float *)value);
      return true;

    case UNIFORM_MAT4:
      glUniformMatrix4fv(location, 1, GL_FALSE, (float *)value);
      return true;

    default:
      printf("Error, unknown uniform type\n");
      return false;
  }
 
  return false;
}


/*******************************************************************************
* Class     : _Program
* Brief     : The class definition and its handlers
*******************************************************************************/
static const class _Program = {
  .size             = sizeof(program),
  .__init__         = Program__init__,
  .__del__          = NULL,
  .__get__          = Program__get__,
  .__set__          = Program__set__,
  .__should_close__ = NULL,
  .__prepare__      = NULL,
  .__enable__       = Program__enable__
};


/*******************************************************************************
* Class     : Program
* Brief     : Defines a class that will handle our basic functions.
*******************************************************************************/
const void *Program = & _Program;
