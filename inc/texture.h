/****************************************************************************
* Title   : Tawy   
* Filename: texture.h
* Author  : Guillaume Mantelet
* Version : 1.0.0
* Brief   : This module manages a texture, from its loading from file.
*******************************************************************************/
#ifndef __TAWY__TEXTURE_H__
#define __TAWY__TEXTURE_H__
#include "object.h"


/*******************************************************************************
* Struct    : texture
* Brief     : Defines an instance of a model that is potentially shared between
*             multiple entities.
* Attributes:

* Class     : The structure storing our handlers.
*******************************************************************************/
typedef struct texture
{
  const void *__cls__;

  unsigned int id;
  unsigned int location;
  int          width;
  int          height;
  int          number_channels;
}texture;


/*******************************************************************************
* Class     : Texture
* Brief     : Defines a class that will handle our basic functions.
*******************************************************************************/
extern const void *Texture;

#endif