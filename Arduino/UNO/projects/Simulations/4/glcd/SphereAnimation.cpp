#include "SphereAnimation.h"
#include "Config.h"

SphereAnimation::SphereAnimation() 
  : maxRadius(24), minRadius(5), currentRadius(24), radiusChange(-1) {}

void SphereAnimation::update() {
  currentRadius += radiusChange;
  
  if (currentRadius <= minRadius) {
    radiusChange = 1;
  } else if (currentRadius >= maxRadius) {
    radiusChange = -1;
  }
}

void SphereAnimation::draw() {
  for (int i = 0; i < NUM_CIRCLES; i++) {
    float angle = (float)i / NUM_CIRCLES * PI;
    int z = currentRadius * cos(angle);
    int r = currentRadius * sin(angle);
    
    if (r > 0) {
      glcd.drawCircle(CENTER_X, CENTER_Y + z, r);
    }
  }
  
  glcd.drawCircle(CENTER_X, CENTER_Y, currentRadius);
  
  for (int i = 0; i < 4; i++) {
    float angle = (float)i / 4 * PI;
    int x = currentRadius * cos(angle);
    
    for (int y = -currentRadius; y <= currentRadius; y += 2) {
      float scale = sqrt(1.0 - ((float)y * y) / (currentRadius * currentRadius));
      int px = CENTER_X + x * scale;
      glcd.setPixel(px, CENTER_Y + y, 1);
    }
  }
}