/****************************************************************************
* Title   : Tawy   
* Filename: model.h
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
#include "model.h"


/*******************************************************************************
* Function  : scan_obj_file
* Brief     : Test a file syntax, retrieve require buffer sizes.
* Parameters:
*    1. path    : The path to the file
*    2. vcount  : Stores the number of vertex coordinates in file
*    3. tcount  : Stores the number of texture coordinates in file
*    4. icount  : Stores the number of indices for building mesh
*    5. ncount  : Stores the number of normal vectors in file
* Returns   :
*    true : The file could be open.
*    false: The file does not exist.
*******************************************************************************/
static bool scan_obj_file(const char *path, unsigned int *vcount, unsigned int *tcount, unsigned int *icount, unsigned int *ncount)
{
  FILE         *f;
  char         *line = NULL;
  unsigned int  len  = 0;
  int           r;

  if (NULL == (f = fopen(path, "r")))
  {
    printf("Error opening file at %s\n", path);
    return false;
  }

  while ((r = getline(&line, &len, f)) != -1)
  {
    if      (strncmp(line, "v ",  2) == 0) (*vcount)++;
    else if (strncmp(line, "vt ", 3) == 0) (*tcount)++;
    else if (strncmp(line, "vn ", 3) == 0) (*ncount)++;
    else if (strncmp(line, "f ",  2) == 0) (*icount)++;
  }

  fclose(f);
  if (line) free(line);
  return true; 
}


/*******************************************************************************
* Function  : fill_vertices
* Brief     : Get vertex and index arrays from file.
* Details   : The vertex buffers stores coordinates, normal vectors and texture
*             coordinates. This array is then used to write to openGL buffer 
*             objects.

*             The vertex coordinate is comprised of vlen lines of 8 floats. 
*             Index 0, 1, 2 match coordinates, 3, 4 and 5, normal vectors, and
*             6, 7 texture coordinates.
*
*     ^      +---------------------+---------------------+---------------------+
*     |v     |  Vertex Coordinates |    Normal vector    | Texture coordinates | 
*     |l     +------+------+-------+------+------+-------+----------+----------+
*     |e     |   X  |   Y  |   Z   |   X  |   Y  |   Z   |     X    |     Y    | 
*     |n     +------+------+-------+------+------+-------+----------+----------+
*     |      ...
*     +      +------+------+-------+------+------+-------+----------+----------+
*
*            Indices lead to definition of triangles, they are then made of 
*            three values pointing to the line index of the vertex array. 
*
* Parameters:
*    1. path    : The path to the file
*    2. v       : The target vertex array to populate from file
*    3. i       : The target index array to populate from file
*    4. vlen    : Number of vertices to create
*    5. tlen    : Number of texture coordinates to create
* Returns   :
*    true : The arrays could be populated.
*    false: The file does not exist or contain unrecoverable errors.
*******************************************************************************/
static bool fill_vertices(const char *path, float *v, unsigned int *i, unsigned int vlen, unsigned int tlen)
{
  FILE         *f;
  int           r;
  int           scanned;
  char         *line     = NULL;
  unsigned int  len      = 0;
  float        *textures = calloc(1, tlen * 2 * sizeof(float));
  float        *tptr     = textures;
  float        *vptr     = v;
  int          *table    = calloc(1, vlen * sizeof(int));
  int           t[3]     = {0};
  int           n[3]     = {0};
  bool          ret      = false;

  //
  // 1. Open file.
  //
  if (NULL == (f = fopen(path, "r")))
  {
    printf("Error opening file at %s\n", path);
    goto out;
  }

  // 
  // 2. Scan file
  //
  while ((r = getline(&line, &len, f)) != -1)
  {
    //
    // 3. If line corresponds to a vertex, coordinates are copied to vertex
    //    buffer. The vertex array pointer is incremented by one line after the
    //    three coordinates are gathered.
    //
    if (strncmp(line, "v ", 2) == 0)
    {
      scanned = sscanf(line + 2, "%f %f %f", vptr, vptr + 1, vptr + 2);
      if (scanned != 3)
      {
        printf("Error: parsed only %d on vertices.\n", scanned);
        goto out;
      }  

      vptr += 8;
    }

    //
    // 4. If line corresponds to a texture coordinates, the values are stored in
    //    a temporary array.
    //
    if (strncmp(line, "vt ", 3) == 0)
    {
      scanned = sscanf(line + 3, "%f %f", tptr, tptr + 1);
      if (scanned != 2)
      {
        printf("Error: parsed only %d on textures.\n", scanned);
        goto out;
      }  

      tptr += 2;
    }

    //
    // 5. If line corresponds to a face, the texture index is matched with 
    //    vertex array in a temporary corresponding table.
    //
    if (strncmp(line, "f ", 2) == 0)
    {
      scanned = sscanf(line + 2, "%d/%u/%u %d/%u/%u %d/%u/%u", i, t, n, i + 1, t + 1, n + 1, i + 2, t + 2, n + 2);
      if (scanned != 9)
      {
        printf("Error: parsed only %d on indices.\n", scanned);
        goto out;
      }  

      //
      // 6. Index 0 is used to detect errors. So right now, if our .obj file
      //    has zero index, we raise an error. 
      // 
      if (i[0] == 0 || i[1] == 0 || i[2] == 0 ||
          t[0] == 0 || t[1] == 0 || t[2] == 0   )
      {
        printf("Error: Some of the indices in face equals 0.\n");
        goto out;       
      }

      //
      // 7. Current version makes it clear that to each vertex index corresponds
      //    one and only one texture index. If it turns that there is already a
      //    different texture index for a vertex index, then we raise an error
      //    and we leave, basically telling we cannot build the model properly.
      //
      for (int j = 0; j < 3; j++)
      {
        if (table[i[j] - 1] != 0 && table[i[j] - 1] != t[j] - 1)
        {
          printf("Error: Redefinition of texture for vertex index %u: old %u, now %u.\n", i[j], table[i[j] - 1], t[j] - 1);
          goto out;         
        }
        else
          table[i[j] - 1] = t[j] - 1;
      }

      i += 3;
    }
  }

  //
  // 8. Using matching table, we retrieve the two texture coordinates in 
  //    temporary texture array and we insert them in corresponding line of 
  //    vertex array, at index 6 and 7.
  //
  for (int m = 0; m < vlen; m++)
  {
    v[m * 8 + 6] = textures[table[m] * 2];
    v[m * 8 + 7] = textures[table[m] * 2 + 1];
  }

  ret = true;

out:
  free(textures);
  free(table);
  return ret;
}


static bool load_model(model *obj, const char *filename, float **vertices, unsigned int **indices)
{
  char         path[1024] = "res/models/";
  unsigned int vcount = 0;
  unsigned int tcount = 0;
  unsigned int icount = 0;
  unsigned int ncount = 0;

  strcat(path, filename);
  if (!scan_obj_file(path, &vcount, &tcount, &icount, &ncount))
    return false;

  *vertices = calloc(1, vcount * 8 * sizeof(float));
  *indices  = calloc(1, icount * 3 * sizeof(int));
  if (!fill_vertices(path, *vertices, *indices, vcount, tcount))
  {
    free(*vertices);
    free(*indices);
    return false;
  }  

  for (int i = 0; i < icount * 3; i++)
    --(*indices)[i];

  obj->vertices = vcount;
  obj->elements = icount * 3;
  return true;
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
  model *obj = self;
  float *vertices = NULL;
  unsigned int *indices = NULL;
  
  //
  // 1. Retrieve .obj file. Build vertices and indices from it. Vertex array and
  //    index array will be sent to VBO and EBO respectively.
  //
  if (!load_model(obj, va_arg(*args, char *), &vertices, &indices))
    return false;

  //
  // 2. Create arrays and buffers: VAO, VBO and EBO
  //
  glGenVertexArrays(1, &obj->vao);
  glBindVertexArray(obj->vao);

  glGenBuffers(1, &obj->vbo);
  glBindBuffer(GL_ARRAY_BUFFER, obj->vbo);
  glBufferData(GL_ARRAY_BUFFER, obj->vertices * 8 * sizeof(float), vertices, GL_STATIC_DRAW);

  glGenBuffers(1, &obj->ebo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, obj->ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, obj->elements * sizeof(int), indices, GL_STATIC_DRAW);

  //
  // 3. From vertex array, light corresponding attribute in VAO.
  //
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);

  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(6 * sizeof(float)));
  glEnableVertexAttribArray(2);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  //
  // 4. Load texture from file.
  //
  obj->texture = new(Texture, va_arg(*args, char *));
  free(vertices);
  free(indices);
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

  if (obj->texture)
  {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, obj->texture->id);
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
static const class _Model = {
  .size             = sizeof(model),
  .__init__         = Model__init__,
  .__del__          = NULL,
  .__get__          = NULL,
  .__set__          = NULL,
  .__should_close__ = NULL,
  .__refresh__      = NULL,
  .__enable__       = Model__enable__,
};


/*******************************************************************************
* Class     : Model
* Brief     : Defines a class that will handle our basic functions.
*******************************************************************************/
const void *Model = & _Model;
