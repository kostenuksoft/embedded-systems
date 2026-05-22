#ifndef __PYRAMID_ANIMATION_H
#define __PYRAMID_ANIMATION_H

#include "Animation.h"
#include "Geometry.h"
#include "GLCD.h"

extern GLCD glcd;

class PyramidAnimation : public Animation {
private:
  static const int NUM_VERTICES = 5;
  Point3D vertices[NUM_VERTICES];
  float angleY;
  float rotationSpeed;
  int size;
  
public:
  PyramidAnimation();
  void update() override;
  void draw() override;
};

#endif