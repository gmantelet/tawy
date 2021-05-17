/****************************************************************************
* Title   : Tawy   
* Filename: window.c
* Author  : Guillaume Mantelet
* Version : 1.0.0
* Brief   : This module manages the render, its callbacks, its display.
*******************************************************************************/
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <glad/glad.h>

#include "window.h"


/*******************************************************************************
* Function  : framebuffer_size_callback
* Brief     : Callback to change viewport when window is resized by user or os.
* Parameters:
*    1. window  : The window being resized.
*    2. width   : The new width for the window.
*    3. height  : The new height for the window.
*******************************************************************************/
static void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
  glViewport(0, 0, width, height);
}


/******************************************************************************
* Function  : key_callback
* Brief     : Triggers an action when some key is pressed.
* Parameters:
*     1. w: The window object being displayed on the screen.
* Returns   : None.
*******************************************************************************/
static void key_callback(GLFWwindow *w, int k, int scancode, int a, int mods)
{
  switch(k)
  {
    case GLFW_KEY_DOWN:
    case GLFW_KEY_W:
      break;

    case GLFW_KEY_UP:
    case GLFW_KEY_S:
      break;

    case GLFW_KEY_RIGHT:
    case GLFW_KEY_D:
      break;

    case GLFW_KEY_LEFT:
    case GLFW_KEY_A:
      break;

    case GLFW_KEY_ESCAPE:
      glfwSetWindowShouldClose(w, true);
      break;
  }
}


/*******************************************************************************
* Function  : Window__init__
* Brief     : The object initializer, called by new()
* Parameters:
*    1. self    : The instance of the renderer.
*    2. width   : The initial width for the window.
*    3. height  : The initial height for the window.
*    4. title   : The application title transferred to the window.
* Returns   :
*    true : The libraries could be loaded successfully.
*    false: Failed to create a new window on our OS, or OpenGL pointers could
*           not be initialized for our window.
*******************************************************************************/
static bool Window__init__(void *self, va_list *args)
{
  //
  // 1. Instance Initialization. Self has already been malloced.
  //
  window *obj = self;
  obj->width  = va_arg(*args, int);
  obj->height = va_arg(*args, int);

  strncpy(obj->title, va_arg(*args, char*), TAWY_WINDOW_TITLE_LEN - 1);

  //
  // 2. Initializing GLFW. We want OpenGL version 3.3.
  //
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

  //
  // 3. Creating the window.
  //
  obj->display = glfwCreateWindow(obj->width, obj->height, obj->title, NULL, NULL);
  if (!obj->display)
  {
    printf("Failed to create GLFW window\n");
    glfwTerminate();
    return false;
  }
  glfwMakeContextCurrent(obj->display);

  //
  // 4. Loading GLAD. Initializing OpenGL pointers for our window.
  //
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
  {
    printf("Failed to initialize GLAD\n");
    glfwTerminate();
    return false;
  }

  //
  // 5. Initializing display and registering callbacks.
  //
  glViewport(0, 0, obj->width, obj->height);
  glfwSetFramebufferSizeCallback(obj->display, framebuffer_size_callback);
  glfwSetKeyCallback(obj->display, key_callback);

  //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  glEnable(GL_DEPTH_TEST);
  return true;
}


/*******************************************************************************
* Function  : Window__del__
* Brief     : The object destructor, called by delete()
* Parameters:
*    1. self    : The instance of the renderer to delete.
*******************************************************************************/
static void Window__del__(void *self)
{
  //__Window *obj = self;
  printf("Collecting window.\n");
  glfwTerminate();
  free(self);
}


/*******************************************************************************
* Function  : Window__get__
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
static bool Window__get__(void *self, const char *attr, void **value)
{
  window *obj = self;
  if (!strncmp(attr, "height", 6))
  {
    *value = &obj->height;
    return true;
  }

  if (!strncmp(attr, "width", 5))
  {
    *value = &obj->width;
    return true;
  }

  *value = 0;
  return false;
}


/*******************************************************************************
* Function  : Window__should_close__
* Brief     : Provide hints to determine if this instance should be garbage 
*             collected.
* Parameters:
*    1. self    : The instance of the renderer.
* Returns   :
*    true : It is safe to destroy this instance
*    false: This instance is still required, thus application is still allowed
*           to perform operations on this instance.
*******************************************************************************/
static bool Window__should_close__(void *self)
{
  window *obj = self;
  return glfwWindowShouldClose(obj->display);
}


/*******************************************************************************
* Function  : Window__refresh__
* Brief     : Update status of this instance.
* Parameters:
*    1. self    : The instance of the renderer.
* Returns   :
*    true : The instance successfully updated itself.
*    false: One of the operation required by the instance to update, or refresh
*           itself on screen failed.
*******************************************************************************/
static bool Window__prepare__(void *self)
{
  glClearColor(1.0f, 0.95f, 0.75f, 1.0f);  // Trying to mimic bright desert.
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  return true;
}


/*******************************************************************************
* Function  : Window__refresh__
* Brief     : Update status of this instance.
* Parameters:
*    1. self    : The instance of the renderer.
* Returns   :
*    true : The instance successfully updated itself.
*    false: One of the operation required by the instance to update, or refresh
*           itself on screen failed.
*******************************************************************************/
static bool Window__enable__(void *self)
{
  window *obj = self;
  glfwSwapBuffers(obj->display);
  glfwPollEvents();
  return true;
}


/*******************************************************************************
* Class     : _Window
* Brief     : The class definition and its handlers
*******************************************************************************/
static const class _Window = {
  .size             = sizeof(window),
  .__init__         = Window__init__,
  .__del__          = Window__del__,
  .__get__          = Window__get__,
  .__set__          = NULL,
  .__should_close__ = Window__should_close__,
  .__prepare__      = Window__prepare__,
  .__enable__       = Window__enable__
};


/*******************************************************************************
* Class     : Window
* Brief     : Defines a class that will handle our basic functions.
*******************************************************************************/
const void *Window = & _Window;
