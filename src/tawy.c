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
  model *m    = new(AssimpModel, "cube.obj", "container.jpg", "awesomeface.png", NULL);
  //model *m    = new(Model, "container.jpg", "awesomeface.png", NULL);
  program *p  = new(Program, "vertex_shader.glsl", "fragment_shader.glsl");

  if (!p)
  {
    delete(m, win, NULL);
    return 1;
  }

  printf("%s (%dx%d)\n", win->title, win->width, win->height);

  //unsigned int x = 0;

  enable(p, NULL);
  
  char buf[64];
  for (unsigned int i = 0; i < m->texture_cnt; i++)
  {
    snprintf(buf, 64, "texture%d", i + 1);
    set(p, buf, &i, UNIFORM_INT);
  }


  while (!should_close(win))
  {
    prepare(win);
    enable(p, NULL);

    mat4 projection;
    glm_mat4_identity(projection);
    //glm_perspective(glm_rad(45.0f), win->width / win->height, 0.1f, 100.0f, projection);
    set(p, "projection", &projection, UNIFORM_MAT4);

    mat4 view;
    glm_mat4_identity(view);
    set(p, "view", &view, UNIFORM_MAT4);

    mat4 model;
    glm_mat4_identity(model);
    //glm_translate(model, (vec3){0.5f, 0.0f, 0.0f});
    glm_rotate(model, 50.0f, (vec3){0.5f, 1.0f, 0.0f});
    set(p, "model", &model, UNIFORM_MAT4);


    enable(m, win, NULL);

    //set(p, "ourColor", &x, UNIFORM_VEC4);
  }

  delete(m, p, win, NULL);
  return 0;
}