#ifndef __ANIMATION_H
#define __ANIMATION_H

class Animation {
protected:
  bool active;
  
public:
  Animation() : active(false) {}
  virtual ~Animation() {}
  
  virtual void init() { active = true; }
  virtual void update() = 0;
  virtual void draw() = 0;
  
  void setActive(bool state) { active = state; }
  bool isActive() const { return active; }
};

#endif