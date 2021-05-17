/****************************************************************************
* Title   : Tawy   
* Filename: object.h
* Author  : Guillaume Mantelet
* Version : 1.0.0
* Brief   : This module manages handlers for our important objects and classes.
*******************************************************************************/
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include "object.h"


/*******************************************************************************
* Function  : new
* Brief     : Allocate object from heap and instantiate it.
* Details   :
* 
* Each instance object is a pointer (obj) to some class definition that derives
* from class. class stores handlers and parameters to help with generic builtin
* functions such as new.
*                     obj            __cls__           class
*                 +---------+      +---------+      +---------+
*                 |         |----->|         |----->|   size  |
*                 +---------+      +---------+      +---------+
*                                  |_ _ _ _ _|      | __init__|
*                                  |         |      | __del__ |
*                                                   +---------+
*
* new allocates a pointer from heap which is the size of its class definition,
* and if successful will call the constructor from the class definition passing
* it the freshly allocated instance parameter.
*
* Parameters:
*    1. cls      : The class definition to retrieve constructor.
*    2. ...      : Variadic arguments to pass to class constructor.
* Returns   :
*    obj  : An instance of the class.
*    null : Class constructor returned false for some reason.
*******************************************************************************/ 
void *new(const void *cls, ...)
{
  va_list args;
  const class *__cls__ = cls;
  
  //
  // 1. New - Obtain a new pointer from heap.
  //
  void *obj = malloc(__cls__->size);

  if (obj == NULL)
  {
    printf("Failed to allocate new object.\n");
    return NULL;
  }

  //
  // 2. Passing class definition to our new instance.
  //
  *(const class **)obj = __cls__;

  //
  // 3. Calling constructor (initializer) on new instance.
  //
  if (__cls__->__init__)
  {   
    va_start(args, cls);
    if (!__cls__->__init__(obj, &args))
      obj = NULL;
    va_end(args);
  }

  return obj;
}


/*******************************************************************************
* Function  : delete
* Brief     : Frees objects from heap and reinitializse them. Any number of
*             instances can be passed to this function, but it MUST be NULL
*             terminated.
* Parameters:
*    1. self     : First instance to free.
*    2. args     : Other instances to free. NUST BE NULL TERMINATED!
*******************************************************************************/
void delete(void *self, ...)
{
  va_list va;
  void *p;
  class **obj;

  va_start(va, self);
  for (p = self; p != NULL; p = va_arg(va, void *))
  {
    obj = p;
    if(p && *obj && (*obj)->__del__)
      (*obj)->__del__(p);
  }
  va_end(va);
}


/*******************************************************************************
* Function  : get
* Brief     : Last resort attribute fetcher. Useful if the struct is private.
* Parameters:
*    1. self    : The instance of the class.
*    2. attr    : The attribute name to get.
*    3. value   : The pointer to the void pointer to store attribute value.
* Returns   :
*    true : The instance owns the requested attribute.
*    false: The instance does not own the requested attribute, or the value 
*           could not be transferred.
*******************************************************************************/
bool get(void *self, const char *attr, void **value)
{
  const class **obj = self;
  if (self && *obj && (*obj)->__get__)
    return (*obj)->__get__(self, attr, value);
  return false;
}


/*******************************************************************************
* Function  : set
* Brief     : Set an attribute. This is useful when for example, you want to 
*             reach a uniform on an instance of a program.
* Parameters:
*    1. self    : The instance of the class.
*    2. attr    : The attribute name to set.
*    3. value   : The pointer to the value to transfer to target attribute.
*    4. args    : Optional parameters.
* Returns   :
*    true : The instance owns the requested attribute.
*    false: The instance does not own the requested attribute, or the value 
*           could not be transferred.
*******************************************************************************/
bool set(void *self, const char *attr, void *value, ...)
{
  va_list args;
  bool ret;
  const class **obj = self;

  if (self && *obj && (*obj)->__set__)
  {
    va_start(args, value);
    ret = (*obj)->__set__(self, attr, value, &args);
    va_end(args);
    return ret;
  }

  return false;  
}


/*******************************************************************************
* Function  : should_close
* Brief     : Provide hints to determine if this instance should be garbage 
*             collected.
* Parameters:
*    1. self    : The instance of the class.
* Returns   :
*    true : It is safe to destroy this instance
*    false: This instance is still required, thus application is still allowed
*           to perform operations on this instance.
*******************************************************************************/
bool should_close(void *self)
{
  const class **obj = self;
  if (self && *obj && (*obj)->__should_close__)
    return (*obj)->__should_close__(self);
  return false; 
}


/*******************************************************************************
* Function  : prepare
* Brief     : Update status of this instance.
* Parameters:
*    1. self    : The instance of the class.
* Returns   :
*    true : The instance successfully updated itself.
*    false: One of the operation required by the instance to update, or refresh
*           itself on screen failed.
*******************************************************************************/
bool prepare(void *self)
{
  const class **obj = self;
  if (self && *obj && (*obj)->__prepare__)
    return (*obj)->__prepare__(self);
  return false; 
}


/*******************************************************************************
* Function  : enable
* Brief     : Make this instance active. Make OpenGL use its buffer or the 
*             program for instance.
* Parameters:
*    1. self     : First instance to activate.
*    2. args     : Other instances to activate. NUST BE NULL TERMINATED!
* Returns   :
*    true : The instances could all be enabled, or any instance has no enable 
*           option, meaning it is enabled all the time by default.
*    false: One of the steps to enable one instance failed.
*******************************************************************************/
bool enable(void *self, ...)
{
  va_list va;
  void *p;
  class **obj;
  bool ret = true;

  va_start(va, self);
  for (p = self; p != NULL; p = va_arg(va, void *))
  {
    obj = p;
    if(p && *obj && (*obj)->__enable__)
      ret &= (*obj)->__enable__(p);
  }
  va_end(va);

  return ret;
}
