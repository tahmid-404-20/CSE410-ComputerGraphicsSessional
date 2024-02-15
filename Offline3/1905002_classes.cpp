#ifdef __linux__
#include <GL/glut.h>
#elif WIN32
#include <glut.h>
#include <windows.h>
#endif

#include "1905002_vec.cpp"
#include <iostream>
#include <vector>

#define AMB 0
#define DIFF 1
#define SPEC 2
#define REFL 3

// contains color, from 0 to 1
class Color {
public:
  double r, g, b;

  Color() {
    r = 0;
    g = 0;
    b = 0;
  }

  Color(double r, double g, double b) {
    this->r = r;
    this->g = g;
    this->b = b;
  }

  // overloaded operators

  // overload * with double
  Color operator*(double scalar) {
    Color result;
    result.r = r * scalar;
    result.g = g * scalar;
    result.b = b * scalar;

    return result;
  }

  // overload * with Color
  Color operator*(Color c) {
    Color result;
    result.r = r * c.r;
    result.g = g * c.g;
    result.b = b * c.b;

    return result;
  }

  // overload + with Color
  Color operator+(Color c) {
    Color result;
    result.r = r + c.r;
    result.g = g + c.g;
    result.b = b + c.b;

    return result;
  }
};

class PointLight {
public:
  Vec position;
  Color color;

  PointLight() {
    position = Vec(0, 0, 0);
    color = Color(1, 1, 1);
  }

  PointLight(Vec position, Color color) {
    this->position = position;
    this->color = color;
  }

  void draw() {
    glPushMatrix();
    glTranslated(position.x, position.y, position.z);
    glColor3f(color.r, color.g, color.b);
    glutSolidSphere(2, 10, 10);
    glPopMatrix();
  }
};

class SpotLight {
public:
  PointLight pointLight;
  Vec direction;
  double cutOffAngle;

  SpotLight() {
    pointLight = PointLight();
    direction = Vec(0, 0, 0);
    cutOffAngle = 0;
  }

  SpotLight(Vec position, Color color, Vec direction, double cutOffAngle) {
    pointLight = PointLight(position, color);
    this->direction = direction;
    this->cutOffAngle = cutOffAngle;
  }

  void draw() { pointLight.draw(); }
};

class Ray {
public:
  Vec start, dir;

  Ray(Vec start, Vec dir) {
    this->start = start;
    this->dir = dir.getNormalizedResult();
  }
};

class Object {
public:
  Vec reference_point;
  double height, width, length;
  Color color;
  double coEfficients[4]; // ambient,diffuse,reflection coefficients
  int shine;              // exponent term of specular component

  Object() {
    reference_point = Vec(0, 0, 0);
    height = 0;
    width = 0;
    length = 0;
    color = Color(0, 0, 0);
    coEfficients[0] = 0;
    coEfficients[1] = 0;
    coEfficients[2] = 0;
    coEfficients[3] = 0;
    shine = 0;
  }

  virtual void draw() = 0;
  virtual double intersect(Ray *ray, Color *color, int level) = 0;

  void setColor(double r, double g, double b) { color = Color(r, g, b); }
  void setShine(int shine) { this->shine = shine; }
  void setCoEfficients(double a, double d, double s, double r) {
    coEfficients[0] = a;
    coEfficients[1] = d;
    coEfficients[2] = s;
    coEfficients[3] = r;
  }
};

extern std::vector<Object *> objects;
extern std::vector<PointLight> pointLights;
extern std::vector<SpotLight> spotLights;

class Sphere : public Object {
  int sectorsCount, stacksCount;

public:
  Sphere(Vec center, double radius) {
    this->reference_point = center;
    this->height = radius;
    this->width = radius;
    this->length = radius;
    this->sectorsCount = 50;
    this->stacksCount = 100;
  }

  void draw() {
    // openGL code to draw sphere
    glPushMatrix();
    glTranslated(reference_point.x, reference_point.y, reference_point.z);
    glColor3f(color.r, color.g, color.b);
    glutSolidSphere(height, sectorsCount, stacksCount);
    glPopMatrix();
  }

  double intersect(Ray *ray, Color *color, int level) {

    double t = this->getTmin(ray);

    if (level == 0) {
      return t;
    }

    Vec intersectionPoint = ray->start + ray->dir * t;
    Color intersectionPointColor = getColorAt(intersectionPoint);

    *color = intersectionPointColor * coEfficients[AMB];

    // iterate through all the point lights
    for (auto pointLight : pointLights) {
      Vec L = pointLight.position - intersectionPoint;
      Vec N = this->getNormalAtIntersectionPoint(intersectionPoint);
      Vec V = ray->start - intersectionPoint;

      L = L.getNormalizedResult();
      V = V.getNormalizedResult();

      double NL = N.getDotProduct(L);
      Vec R = N * (2 * NL) - L;
      R = R.getNormalizedResult();

      double RV = R.getDotProduct(V);

      // I have doubts about this part, so commenting it out and adding a new
      //   if (NL > 0) {
      //     *color = *color + (pointLight.color * coEfficients[DIFF] * NL *
      //                        intersectionPointColor); // diffuse component
      //   }

      //   if (RV > 0) {
      //     *color =
      //         *color + (pointLight.color * coEfficients[SPEC] * pow(RV,
      //         shine) *
      //                   intersectionPointColor); // specular component
      //   }

      if (NL > 0) {
        *color = *color + (intersectionPointColor * coEfficients[DIFF] *
                           NL); // diffuse component
      }

      if (RV > 0) {
        *color = *color + (pointLight.color * coEfficients[SPEC] *
                           pow(RV, shine)); // specular component
      }
    }

    return t;
  }

  double getTmin(Ray *ray) {
    Vec R_0 = ray->start - reference_point; // move the sphere to the origin,
                                            // rthis formula for -R_0
    double tp = -ray->dir.getDotProduct(R_0);

    if (tp < 0) {
      return -1.0; // ray is in opposite direction
    }

    double d2 = R_0.getDotProduct(R_0) - tp * tp;

    double radius2 = height * height;

    if (d2 > radius2) {
      return -1.0; // ray misses the sphere
    }

    // find whether the origin is inside or outside the sphere
    bool outside = true;

    if (R_0.getDotProduct(R_0) < radius2) // origin is inside the sphere
      outside = false;

    double t1 = sqrt(radius2 - d2);

    double t;
    if (outside) {
      t = tp - t1;
    } else {
      t = tp + t1;
    }

    return t;
  }

  Color getColorAt(Vec intersectionPoint) { return color; }

  Vec getNormalAtIntersectionPoint(Vec intersectionPoint) {
    Vec normal = intersectionPoint - reference_point;
    return normal.getNormalizedResult();
  }
};

class Floor : public Object {

public:
  int tesselation;
  double tileWidth, floorWidth;
  Floor(double floorWidth, double tileWidth) {
    this->floorWidth = floorWidth;
    this->tileWidth = tileWidth;
    this->tesselation = floorWidth / tileWidth;
  }

  void draw() {
    // openGL code to draw floor
    glPushMatrix();
    glBegin(GL_QUADS);
    for (int i = -tesselation; i < tesselation; i++) {
      for (int j = -tesselation; j < tesselation; j++) {
        if ((i + j) % 2 == 0) {
          glColor3f(1.0f, 1.0f, 1.0f); // White
        } else {
          glColor3f(0.0f, 0.0f, 0.0f); // Black
        }
        glVertex3f(i * tileWidth, j * tileWidth, 0);
        glVertex3f(i * tileWidth, (j + 1) * tileWidth, 0);
        glVertex3f((i + 1) * tileWidth, (j + 1) * tileWidth, 0);
        glVertex3f((i + 1) * tileWidth, j * tileWidth, 0);
      }
    }
    glEnd();
    glPopMatrix();
  }

  double intersect(Ray *ray, Color *color, int level) { return 0; }
};

class Triangle : public Object {
  Vec a, b, c;

public:
  Triangle(Vec a, Vec b, Vec c) {
    this->a = a;
    this->b = b;
    this->c = c;
  }

  void draw() {
    // openGL code to draw triangle
    glPushMatrix();
    glBegin(GL_TRIANGLES);
    glColor3f(color.r, color.g, color.b);
    glVertex3f(a.x, a.y, a.z);
    glVertex3f(b.x, b.y, b.z);
    glVertex3f(c.x, c.y, c.z);
    glEnd();
    glPopMatrix();
  }

  double intersect(Ray *ray, Color *color, int level) { return 0; }
};
