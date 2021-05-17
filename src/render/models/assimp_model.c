/****************************************************************************
* Title   : Tawy   
* Filename: assimp_model.h
* Author  : Guillaume Mantelet
* Version : 1.0.0
* Brief   : This module manages a model, enabling and disabling buffers, and
*           loading vertices and textures from file.
*******************************************************************************/
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "model.h"


/*******************************************************************************
* Function  : elements_to_buffer
* Brief     : Transfer assimp scene faces to OpenGL element array buffer.
* Parameters:
*    1. path    : The instance of the model
*    2. scene   : The assimp scene from .obj file.
* Returns   :
*    true : Unconditional
*******************************************************************************/
static bool elements_to_buffer(model *obj, const struct aiScene *scene)
{
  const struct aiMesh *mesh;
  const struct aiFace *face;
  unsigned int         faceIndex = 0;

  for (unsigned int n = 0; n < scene->mNumMeshes; n++)
  {
    mesh = scene->mMeshes[n];
    obj->indices = malloc(mesh->mNumFaces * 3 * sizeof(unsigned int));
    faceIndex = 0;

    for (unsigned int t = 0; t < mesh->mNumFaces; t++)
    {
      face = &mesh->mFaces[t];
      memcpy(&obj->indices[faceIndex], face->mIndices, 3 * sizeof(unsigned int));
      faceIndex += 3;
    }

    obj->elements = scene->mMeshes[n]->mNumFaces * 3;

    glGenBuffers(1, &obj->ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, obj->ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, obj->elements * 3 * sizeof(int), obj->indices, GL_STATIC_DRAW);
  }
  return true;
}


/*******************************************************************************
* Function  : normals_to_buffer
* Brief     : Transfer assimp scene normal vectors to OpenGL array buffer.
* Parameters:
*    1. path    : The instance of the model
*    2. scene   : The assimp scene from .obj file.
* Returns   :
*    true : Unconditional
*******************************************************************************/
static bool normals_to_buffer(model *obj, const struct aiScene *scene)
{
  const struct aiMesh *mesh;
  unsigned int buffer;

  for (unsigned int n = 0; n < scene->mNumMeshes; n++)
  {
    mesh = scene->mMeshes[n];

    if (mesh->mNormals)
    {     
      glGenBuffers(1, &buffer);
      glBindBuffer(GL_ARRAY_BUFFER, buffer);
      glBufferData(GL_ARRAY_BUFFER, mesh->mNumVertices * 3 * sizeof(float), mesh->mNormals, GL_STATIC_DRAW);

      glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
      glEnableVertexAttribArray(1);
    }
  }
  return true;
}


/*******************************************************************************
* Function  : textures_to_buffer
* Brief     : Transfer assimp scene texture coordinates to OpenGL array buffer.
* Parameters:
*    1. path    : The instance of the model
*    2. scene   : The assimp scene from .obj file.
* Returns   :
*    true : Unconditional
*******************************************************************************/
static bool textures_to_buffer(model *obj, const struct aiScene *scene)
{
  const struct aiMesh *mesh;
  float               *texCoords;
  unsigned int         buffer;

  for (unsigned int n = 0; n < scene->mNumMeshes; n++)
  {
    mesh = scene->mMeshes[n];

    if (mesh->mTextureCoords[0])
    { 
      texCoords = (float *)malloc(mesh->mNumVertices * 2 * sizeof(float));
      for (unsigned int i = 0; i < mesh->mNumVertices; i++)
      {
        texCoords[i * 2]     = mesh->mTextureCoords[0][i].x;
        texCoords[i * 2 + 1] = mesh->mTextureCoords[0][i].y;
      }

      glGenBuffers(1, &buffer);
      glBindBuffer(GL_ARRAY_BUFFER, buffer);
      glBufferData(GL_ARRAY_BUFFER, mesh->mNumVertices * 2 * sizeof(float), texCoords, GL_STATIC_DRAW);

      glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);
      glEnableVertexAttribArray(2);
    }
  }
  return true;
}


/*******************************************************************************
* Function  : vertices_to_buffer
* Brief     : Transfer assimp scene vertex coordinates to OpenGL array buffer.
* Parameters:
*    1. path    : The instance of the model
*    2. scene   : The assimp scene from .obj file.
* Returns   :
*    true : Unconditional
*******************************************************************************/
static bool vertices_to_buffer(model *obj, const struct aiScene *scene)
{
  const struct aiMesh *mesh;

  for (unsigned int n = 0; n < scene->mNumMeshes; n++)
  {
    mesh = scene->mMeshes[n];

    if (mesh->mVertices)
    {
      obj->vertices = mesh->mNumVertices;
      obj->coordinates = malloc(obj->vertices * 3 * sizeof(float));
      memcpy(obj->coordinates, mesh->mVertices, obj->vertices * 3 * sizeof(float));

      glGenBuffers(1, &obj->vbo);
      glBindBuffer(GL_ARRAY_BUFFER, obj->vbo);
      glBufferData(GL_ARRAY_BUFFER, obj->vertices * 3 * sizeof(float), obj->coordinates, GL_STATIC_DRAW);

      glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
      glEnableVertexAttribArray(0);
    }
  }

  return true;
}


/*******************************************************************************
* Function  : load_model
* Brief     : obj model to OpenGL vertex array object.
* Parameters:
*    1. path    : The instance of the model
*    2. filename: The filename to extract obj model from.
* Returns   :
*    true : Successfully converted obj to vertex array object.
*    false: File could not be located, or file is somehow corrupt.
*******************************************************************************/
static bool load_model(model *obj, const char *filename)
{ 
  bool         ret        = true;
  char         path[1024] = "res/models/";
  strcat(path, filename);

  const struct aiScene *scene = aiImportFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);
  if (!scene) 
  {
    printf("Assimp error: %s\n", aiGetErrorString());
    return false;
  }
  
  if (scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
  {
    printf("Assimp error: %s\n", aiGetErrorString());
    ret = false;
  }

  else
  {
    vertices_to_buffer(obj, scene);
    normals_to_buffer(obj, scene);
    textures_to_buffer(obj, scene);
    elements_to_buffer(obj, scene);
  }

  aiReleaseImport(scene);
  return ret;
}


/*******************************************************************************
* Function  : Model__init__
* Brief     : The object initializer, called by new()
* Parameters:
*    1. self    : The instance of the model.
* Returns   :
*    true : The model is created, ready to be displayed.
*    false: Either the .obj file or the texture file could not been retrieved
*           properly. They may be missing, or have corrupt data, or data written
*           in a way that could not been handled successfully.
*******************************************************************************/
static bool Model__init__(void *self, va_list *args)
{
  model        *obj = self;
  char         *p;
  unsigned int  cnt = 0;

  //
  // 1. Create arrays and buffers: VAO, VBO and EBO
  //
  glGenVertexArrays(1, &obj->vao);
  glBindVertexArray(obj->vao);
  
  //
  // 2. Retrieve .obj file. Build vertices and indices from it. Vertex array and
  //    index array will be sent to VBO and EBO respectively.
  //
  //if (!load_model(obj, va_arg(*args, char *), &vertices, &indices))
  if (!load_model(obj, va_arg(*args, char *)))
    return false;

  //
  // 4. Load textures from file.
  //
  while (true)
  {
    p = va_arg(*args, char *);
    if (!p) break;
    printf("Loading texture: %s\n", p);
    obj->texture[cnt++] = new(Texture, p);
  }  

  obj->texture_cnt = cnt;
  return true;
}


/*******************************************************************************
* Function  : Model__enable__
* Brief     : Enable our buffers before rendering them.
* Parameters:
*    1. self    : The instance of the program.
* Returns   :
*    true : Unconditional
*******************************************************************************/
static bool Model__enable__(void *self)
{
  model *obj = self;

  for (unsigned int i = 0; i < obj->texture_cnt; i++)
  {
    glActiveTexture(GL_TEXTURE0 + i);
    glBindTexture(GL_TEXTURE_2D, obj->texture[i]->id);
  }

  glBindVertexArray(obj->vao);
  //glDrawArrays(GL_TRIANGLES, 0, 3);
  glDrawElements(GL_TRIANGLES, obj->elements, GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
  return true;
}


/*******************************************************************************
* Class     : _Model
* Brief     : The class definition and its handlers
*******************************************************************************/
static const class _AssimpModel = {
  .size             = sizeof(model),
  .__init__         = Model__init__,
  .__del__          = NULL,
  .__get__          = NULL,
  .__set__          = NULL,
  .__should_close__ = NULL,
  .__prepare__      = NULL,
  .__enable__       = Model__enable__,
};


/*******************************************************************************
* Class     : AssimpModel
* Brief     : Defines a class that will handle our basic functions.
*******************************************************************************/
const void *AssimpModel = &_AssimpModel;
