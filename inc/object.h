/****************************************************************************
* Title   : Tawy   
* Filename: object.h
* Author  : Guillaume Mantelet
* Version : 1.0.0
* Brief   : This module manages handlers for our important objects and classes.
*******************************************************************************/
#ifndef __TAWY__OBJECT_H__
#define __TAWY__OBJECT_H__
#include <stdbool.h>


/*******************************************************************************
* Struct    : class
* Brief     : The generic class definition, providing a set of abstract handlers
*             to constrain instances of our application.
* Attributes:
*    1. size            : The size of the class definition
*    2. __init__        : Class constructor.
*    3. __del__         : Class destructor
*    4. __getattr__     : Encapsulator if class definition is private
*    5. __should_close__: Hint to determine if instance is ready for collection.
*    6. __refresh__     : Update status of this instance, refresh on screen.
*    7. __input__       : Forces instance to process external inputs (e.g. from
*                         user).
*    8. __enable__      : Make this instance active. Make OpenGL use its buffer
*                         or the program for instance.
*******************************************************************************/
typedef struct class
{
  size_t size;
  bool  (*__init__)(void *, va_list *);
  void  (*__del__)(void *);
  bool  (*__get__)(void *, const char *, void **);
  bool  (*__set__)(void *, const char *, void *, va_list *);
  bool  (*__should_close__)(void *);
  bool  (*__refresh__)(void *);
  bool  (*__enable__)(void *);
}class;


/*******************************************************************************
* Function  : new
* Brief     : Allocate object from heap and instantiate it.
* Parameters:
*    1. class    : A class definition to retrieve constructor.
*    2. args     : Variadic arguments to pass to class constructor.
* Returns   :
*    obj  : An instance of the class.
*    null : Class constructor returned false for some reason.
*******************************************************************************/
void *new(const void *, ...);


/*******************************************************************************
* Function  : delete
* Brief     : Frees objects from heap and reinitializse them. Any number of
*             instances can be passed to this function, but it MUST be NULL
*             terminated.
* Parameters:
*    1. self     : First instance to free.
*    2. args     : Other instances to free. NUST BE NULL TERMINATED!
*******************************************************************************/
void delete(void *, ...);


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
bool get(void *, const char *, void **);


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
bool set(void *, const char *, void *, ...);


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
bool should_close(void *);


/*******************************************************************************
* Function  : refresh
* Brief     : Update status of this instance.
* Parameters:
*    1. self    : The instance of the class.
* Returns   :
*    true : The instance successfully updated itself.
*    false: One of the operation required by the instance to update, or refresh
*           itself on screen failed.
*******************************************************************************/
bool refresh(void *);


/*******************************************************************************
* Function  : enable
* Brief     : Make this instance active. Make OpenGL use its buffer or the 
*             program for instance.
* Notive    : This function executes its parameters in order. The order of your
*             parameters matters. You may need to enable the program first, and
*             the window last (so that it swaps its buffer).
* Parameters:
*    1. self     : First instance to activate.
*    2. args     : Other instances to activate. NUST BE NULL TERMINATED!
* Returns   :
*    true : The instances could all be enabled, or any instance has no enable 
*           option, meaning it is enabled all the time by default.
*    false: One of the steps to enable one instance failed.
*******************************************************************************/
bool enable(void *, ...);
#endif
