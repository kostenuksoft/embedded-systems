#include "PyramidAnimation.h"

PyramidAnimation::PyramidAnimation() 
  : angleY(0), rotationSpeed(0.06), size(20) {
  vertices[0] = Point3D(-size, size, -size);
  vertices[1] = Point3D(size, size, -size);
  vertices[2] = Point3D(size, size, size);
  vertices[3] = Point3D(-size, size, size);
  vertices[4] = Point3D(0, -size * 1.5, 0);
}

void PyramidAnimation::update() {
  angleY += rotationSpeed;
  if (angleY >= 2 * PI) angleY -= 2 * PI;
}

void PyramidAnimation::draw() {
  Point2D projected[NUM_VERTICES];
  
  for (int i = 0; i < NUM_VERTICES; i++) {
    Point3D rotated = Transform3D::rotateY(vertices[i], angleY);
    projected[i] = Transform3D::project(rotated, 150);
  }
  
  glcd.drawLine(projected[0].x, projected[0].y, projected[1].x, projected[1].y);
  glcd.drawLine(projected[1].x, projected[1].y, projected[2].x, projected[2].y);
  glcd.drawLine(projected[2].x, projected[2].y, projected[3].x, projected[3].y);
  glcd.drawLine(projected[3].x, projected[3].y, projected[0].x, projected[0].y);
  
  glcd.drawLine(projected[0].x, projected[0].y, projected[4].x, projected[4].y);
  glcd.drawLine(projected[1].x, projected[1].y, projected[4].x, projected[4].y);
  glcd.drawLine(projected[2].x, projected[2].y, projected[4].x, projected[4].y);
  glcd.drawLine(projected[3].x, projected[3].y, projected[4].x, projected[4].y);
  
  for (int i = 0; i < NUM_VERTICES; i++) {
    glcd.setPixel(projected[i].x, projected[i].y, 1);
    glcd.setPixel(projected[i].x + 1, projected[i].y, 1);
    glcd.setPixel(projected[i].x, projected[i].y + 1, 1);
  }
}