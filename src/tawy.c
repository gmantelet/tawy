/****************************************************************************
* Title   : Tawy   
* Filename: tawy.c
* Author  : Guillaume Mantelet
* Version : 1.0.0
* Brief   : Demonstration of different engine components.
*******************************************************************************/
#include <stdio.h>
#include <cglm/cglm.h>

#include "model.h"
#include "program.h"
#include "window.h"


int main(void)
{
  window *win = new(Window, 800, 600, "tawy");  // The windows creates context. It must come first!
  model *m    = new(Model, "cube.obj", "container.jpg");
  program *p  = new(Program, "vertex_shader.glsl", "fragment_shader.glsl");

  if (!p)
  {
    delete(m, win, NULL);
    return 1;
  }

  printf("%s (%dx%d)\n", win->title, win->width, win->height);

  //unsigned int x = 0;

  while (!should_close(win))
  {
    refresh(win);
    enable(p, NULL);

    set(p, "texture1", &(m->texture->location), UNIFORM_INT);
    enable(m, win, NULL);

    //set(p, "ourColor", &x, UNIFORM_VEC4);
  }

  delete(m, p, win, NULL);
  return 0;
}