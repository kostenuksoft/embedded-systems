#ifndef __CUBE_ANIMATION_H
#define __CUBE_ANIMATION_H

#include "Animation.h"
#include "Geometry.h"
#include "GLCD.h"

extern GLCD glcd;

class CubeAnimation : public Animation {
private:
  static const int NUM_VERTICES = 8;
  Point3D vertices[NUM_VERTICES];
  float angleX;
  float rotationSpeed;
  int size;
  
public:
  CubeAnimation();
  void update() override;
  void draw() override;
};

#endif