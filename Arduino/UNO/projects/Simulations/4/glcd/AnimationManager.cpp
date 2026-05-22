#include "AnimationManager.h"
#include "CubeAnimation.h"
#include "SphereAnimation.h"
#include "PyramidAnimation.h"

AnimationManager::AnimationManager() : currentIndex(0) {
  animations[0] = new CubeAnimation();
  animations[1] = new SphereAnimation();
  animations[2] = new PyramidAnimation();
}

AnimationManager::~AnimationManager() {
  for (int i = 0; i < MAX_ANIMATIONS; i++) {
    delete animations[i];
  }
}

void AnimationManager::switchAnimation() {
  animations[currentIndex]->setActive(false);
  currentIndex = (currentIndex + 1) % MAX_ANIMATIONS;
  animations[currentIndex]->setActive(true);
  animations[currentIndex]->init();
  glcd.clear();
}

void AnimationManager::update() {
  if (animations[currentIndex]->isActive()) {
    glcd.clear();
    animations[currentIndex]->update();
    animations[currentIndex]->draw();
  }
}

int AnimationManager::getCurrentIndex() const {
  return currentIndex;
}