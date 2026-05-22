#include "Geometry.h"

Point3D Transform3D::rotateX(const Point3D& p, float angle) {
  float cosA = cos(angle);
  float sinA = sin(angle);
  return Point3D(
    p.x,
    p.y * cosA - p.z * sinA,
    p.y * sinA + p.z * cosA
  );
}

Point3D Transform3D::rotateY(const Point3D& p, float angle) {
  float cosA = cos(angle);
  float sinA = sin(angle);
  return Point3D(
    p.x * cosA + p.z * sinA,
    p.y,
    -p.x * sinA + p.z * cosA
  );
}

Point3D Transform3D::rotateZ(const Point3D& p, float angle) {
  float cosA = cos(angle);
  float sinA = sin(angle);
  return Point3D(
    p.x * cosA - p.y * sinA,
    p.x * sinA + p.y * cosA,
    p.z
  );
}

Point2D Transform3D::project(const Point3D& p, float distance) {
  float scale = distance / (distance + p.z);
  return Point2D(
    CENTER_X + (int)(p.x * scale),
    CENTER_Y + (int)(p.y * scale)
  );
}