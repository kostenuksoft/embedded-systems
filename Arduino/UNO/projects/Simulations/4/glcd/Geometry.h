#ifndef __GEOMETRY_H
#define __GEOMETRY_H

#include <Arduino.h>
#include "Config.h"

struct Point3D {
  float x, y, z;
  Point3D() : x(0), y(0), z(0) {}
  Point3D(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}
};

struct Point2D {
  int x, y;
  Point2D() : x(0), y(0) {}
  Point2D(int _x, int _y) : x(_x), y(_y) {}
};

class Transform3D {
public:
  static Point3D rotateX(const Point3D& p, float angle);
  static Point3D rotateY(const Point3D& p, float angle);
  static Point3D rotateZ(const Point3D& p, float angle);
  static Point2D project(const Point3D& p, float distance = 100);
};

#endif