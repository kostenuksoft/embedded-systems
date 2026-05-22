#ifndef ANIMATION_MANAGER_H
#define ANIMATION_MANAGER_H

#include "Animation.h"
#include "GLCD.h"

extern GLCD glcd;

class AnimationManager {
private:
  static const int MAX_ANIMATIONS = 3;
  Animation* animations[MAX_ANIMATIONS];
  int currentIndex;
  
public:
  AnimationManager();
  ~AnimationManager();
  
  void switchAnimation();
  void update();
  int getCurrentIndex() const;
};

#endif