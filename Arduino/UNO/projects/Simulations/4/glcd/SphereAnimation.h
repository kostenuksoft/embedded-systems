#ifndef __SPHERE_ANIMATION_H
#define __SPHERE_ANIMATION_H

#include "Animation.h"
#include "GLCD.h"

extern GLCD glcd;

class SphereAnimation : public Animation {
private:
  int maxRadius, minRadius, currentRadius;
  int radiusChange;
  static const int NUM_CIRCLES = 5;
  
public:
  SphereAnimation();
  void update() override;
  void draw() override;
};

#endif