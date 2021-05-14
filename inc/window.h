/****************************************************************************
* Title   : Tawy   
* Filename: window.h
* Author  : Guillaume Mantelet
* Version : 1.0.0
* Brief   : This module manages the render, its callbacks, its display.
*******************************************************************************/
#ifndef __TAWY__WINDOW_H__
#define __TAWY__WINDOW_H__
#include <GLFW/glfw3.h>
#include "object.h"

#define TAWY_WINDOW_TITLE_LEN 32


/*******************************************************************************
* Struct    : window
* Brief     : Defines an instance storing parameters to render objects on screen
* Attributes:
*    1. width  : The application window width.
*    2. height : The application window height.
*    3. title  : The application window title.
*    4. display: The window itself.
*
* Class     : The structure storing our handlers.
*******************************************************************************/
typedef struct window
{
  const void *__cls__;

  int         width;
  int         height;
  char        title[TAWY_WINDOW_TITLE_LEN];
  GLFWwindow *display;

}window;


/*******************************************************************************
* Class     : Window
* Brief     : Defines a class that will handle our basic functions.
*******************************************************************************/
extern const void *Window;

#endif