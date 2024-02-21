#ifdef __linux__
#include <GL/glut.h>
#elif WIN32
#include <glut.h>
#include <windows.h>
#endif

#include "1905002_vec.cpp"
#include <iostream>
#include <vector>

#define TEST 1

#define AMB 0
#define DIFF 1
#define SPEC 2
#define REFL 3

#define EPSILON 1e-5

extern int recursionLevel;
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

class BoundingBox {
public:
  Vec cubeReferencePoint;
  double cubeHeight, cubeWidth, cubeLength;

  BoundingBox(Vec reference_point, double length, double width,
              double height) { // length along x, width along y, height along z
    this->cubeReferencePoint = reference_point;
    this->cubeHeight = height;
    this->cubeWidth = width;
    this->cubeLength = length;
  }

  BoundingBox() {
    cubeReferencePoint = Vec(0, 0, 0);
    cubeHeight = 0;
    cubeWidth = 0;
    cubeLength = 0;
  }

  bool isInside(Vec point) {
    // if length is zero, no need to check x(the condition is true), similarly
    // for y and z
    if (cubeLength > EPSILON && (point.x < cubeReferencePoint.x ||
                                 point.x > cubeReferencePoint.x + cubeLength)) {
      return false;
    }
    if (cubeWidth > EPSILON && (point.y < cubeReferencePoint.y ||
                                point.y > cubeReferencePoint.y + cubeWidth)) {
      return false;
    }
    if (cubeHeight > EPSILON && (point.z < cubeReferencePoint.z ||
                                 point.z > cubeReferencePoint.z + cubeHeight)) {
      return false;
    }

    return true;
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
    this->direction = direction.getNormalizedResult();
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

  double intersect(Ray *ray, Color *color, int level);

  virtual void draw() = 0;
  virtual double getTmin(Ray *ray) = 0;
  virtual Color getColorAt(Vec intersectionPoint) = 0;
  virtual Vec getNormalAtIntersectionPoint(Vec intersectionPoint) = 0;

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

double Object::intersect(Ray *ray, Color *color, int level) {
  double t = this->getTmin(ray);

  if (level == 0) {
    return t;
  }

  if (level > recursionLevel) {
    // std::cout << "level: " << level << std::endl;
    return t;
  }

  Vec intersectionPoint = ray->start + ray->dir * t;
  Color intersectionPointColor = this->getColorAt(intersectionPoint);

  *color = intersectionPointColor * coEfficients[AMB];

  // iterate through all the point lights
  for (auto pointLight : pointLights) {

    bool obscured = false;

    Vec dir = intersectionPoint - pointLight.position;
    Ray shadowRay(pointLight.position, dir);

    double distance = dir.getMagnitude();

    if (distance < EPSILON) {
      continue;
    }

    double tMin = distance;
    for (auto object : objects) {
      double temp = object->getTmin(&shadowRay);
      if (temp > 0 && temp + EPSILON < tMin) {
        obscured = true;
        break;
      }
    }

    if (!obscured) {
      Vec L = pointLight.position - intersectionPoint;
      Vec N = this->getNormalAtIntersectionPoint(intersectionPoint);

      // including it on test
      if (TEST && N.getDotProduct(ray->dir) > 0) {
        N = N * -1;
      }

      Vec V = ray->start - intersectionPoint;

      L = L.getNormalizedResult();
      V = V.getNormalizedResult();

      double NL = N.getDotProduct(L);
      Vec R = N * (2 * NL) - L;
      R = R.getNormalizedResult();

      double RV = R.getDotProduct(V);

      NL = std::max(0.0, NL);
      RV = std::max(0.0, RV);

      *color = *color + (intersectionPointColor * coEfficients[DIFF] * NL *
                         pointLight.color); // diffuse component

      *color =
          *color + (pointLight.color * coEfficients[SPEC] * pow(RV, shine) *
                    intersectionPointColor); // specular component
    }
  }

  // iterate through all the spot lights
  for (auto spotLight : spotLights) {
    bool obscured = false;

    Vec dir = intersectionPoint - spotLight.pointLight.position;
    Ray shadowRay(spotLight.pointLight.position, dir);

    double distance = dir.getMagnitude();

    if (distance < EPSILON) {
      continue;
    }

    double tMin = distance;
    for (auto object : objects) {
      double temp = object->getTmin(&shadowRay);
      if (temp > 0 && temp + EPSILON < tMin) {
        obscured = true;
        break;
      }
    }

    if (!obscured) {
      Vec L = spotLight.pointLight.position - intersectionPoint;

      Vec lightRay = -L;
      double angle =
          lightRay.getAngleBetweenVector(spotLight.direction) * 180 / M_PI;

      if (angle > spotLight.cutOffAngle)
        continue;

      Vec N = this->getNormalAtIntersectionPoint(intersectionPoint);

      if (TEST && N.getDotProduct(ray->dir) > 0) {
        N = N * -1;
      }

      Vec V = ray->start - intersectionPoint;

      L = L.getNormalizedResult();
      V = V.getNormalizedResult();

      double NL = N.getDotProduct(L);
      Vec R = N * (2 * NL) - L;
      R = R.getNormalizedResult();

      double RV = R.getDotProduct(V);

      NL = std::max(0.0, NL);
      RV = std::max(0.0, RV);

      *color =
          *color + (intersectionPointColor * coEfficients[DIFF] * NL *
                    spotLight.pointLight.color *
                    pow(cos(angle * M_PI / 180.0), 4.4)); // diffuse component

      *color = *color + (spotLight.pointLight.color * coEfficients[SPEC] *
                         pow(RV, shine) * intersectionPointColor *
                         pow(cos(angle * M_PI / 180.0), 4.4)); // specular
    }
  }

  Vec normal = this->getNormalAtIntersectionPoint(intersectionPoint)
                   .getNormalizedResult();

  // including it on test
  if (TEST && normal.getDotProduct(ray->dir) > 0) {
    normal = normal * -1;
  }

  Vec m = normal * ray->dir.getDotProduct(normal);
  Vec r = ray->dir - m * 2;

  Ray reflectedRay(intersectionPoint + r * EPSILON, r);

  double tMin = 1e9;
  int nearestObjectIndex = -1;

  for (int k = 0; k < objects.size(); k++) {
    Color dummyColor;
    double t = objects[k]->getTmin(&reflectedRay);
    if (t > 0 && t < tMin) {
      tMin = t;
      nearestObjectIndex = k;
    }
  }

  if (nearestObjectIndex != -1) {
    Color reflectedColor;
    tMin = objects[nearestObjectIndex]->intersect(&reflectedRay,
                                                  &reflectedColor, level + 1);
    *color = *color + (reflectedColor * coEfficients[REFL]);
  }

  return t;
}

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
    glColor3f(color.r, color.g, color.b);
    glBegin(GL_TRIANGLES);
    glVertex3f(a.x, a.y, a.z);
    glVertex3f(b.x, b.y, b.z);
    glVertex3f(c.x, c.y, c.z);
    glEnd();
    glPopMatrix();
  }

  double getTmin(Ray *ray) {
    Vec R_0 = ray->start;
    Vec Rd = ray->dir;

    // construct matrix A
    Matrix A;
    A.matrix[0][0] = a.x - b.x;
    A.matrix[0][1] = a.x - c.x;
    A.matrix[0][2] = Rd.x;
    A.matrix[1][0] = a.y - b.y;
    A.matrix[1][1] = a.y - c.y;
    A.matrix[1][2] = Rd.y;
    A.matrix[2][0] = a.z - b.z;
    A.matrix[2][1] = a.z - c.z;
    A.matrix[2][2] = Rd.z;

    // construct matrix beta
    Matrix beta;
    beta.matrix[0][0] = a.x - R_0.x;
    beta.matrix[0][1] = a.x - c.x;
    beta.matrix[0][2] = Rd.x;
    beta.matrix[1][0] = a.y - R_0.y;
    beta.matrix[1][1] = a.y - c.y;
    beta.matrix[1][2] = Rd.y;
    beta.matrix[2][0] = a.z - R_0.z;
    beta.matrix[2][1] = a.z - c.z;
    beta.matrix[2][2] = Rd.z;

    // construct matrix gamma
    Matrix gamma;
    gamma.matrix[0][0] = a.x - b.x;
    gamma.matrix[0][1] = a.x - R_0.x;
    gamma.matrix[0][2] = Rd.x;
    gamma.matrix[1][0] = a.y - b.y;
    gamma.matrix[1][1] = a.y - R_0.y;
    gamma.matrix[1][2] = Rd.y;
    gamma.matrix[2][0] = a.z - b.z;
    gamma.matrix[2][1] = a.z - R_0.z;
    gamma.matrix[2][2] = Rd.z;

    // construct matrix t
    Matrix t;
    t.matrix[0][0] = a.x - b.x;
    t.matrix[0][1] = a.x - c.x;
    t.matrix[0][2] = a.x - R_0.x;
    t.matrix[1][0] = a.y - b.y;
    t.matrix[1][1] = a.y - c.y;
    t.matrix[1][2] = a.y - R_0.y;
    t.matrix[2][0] = a.z - b.z;
    t.matrix[2][1] = a.z - c.z;
    t.matrix[2][2] = a.z - R_0.z;

    double detA = A.determinant();

    if (fabs(detA - 0.0) < EPSILON / 10) {
      return -1;
    }

    double betaVal = beta.determinant() / detA;
    double gammaVal = gamma.determinant() / detA;

    if (betaVal < 0 || gammaVal < 0 || betaVal + gammaVal > 1) {
      return -1;
    }

    double tVal = t.determinant() / detA;

    return tVal;
  }

  Color getColorAt(Vec intersectionPoint) { return color; }

  Vec getNormalAtIntersectionPoint(Vec intersectionPoint) {
    Vec normal = (b - a).getCrossProduct(c - a);
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
    this->reference_point = Vec(0, 0, 0);
  }

  void draw() {
    // openGL code to draw floor
    glPushMatrix();
    glTranslated(-floorWidth / 2, -floorWidth / 2, 0);
    glBegin(GL_QUADS);
    for (int i = 0; i < tesselation; i++) {
      for (int j = 0; j < tesselation; j++) {
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

  double getTmin(Ray *ray) {
    // basically ray-plane intersection
    Vec normal = Vec(0, 0, 1);

    if (ray->dir.getDotProduct(normal) == 0) {
      return -1;
    }

    double t = (reference_point - ray->start).getDotProduct(normal) /
               (ray->dir.getDotProduct(normal));

    return t;
  }
  Color getColorAt(Vec intersectionPoint) {
    // floor spans from -floorWidth/2 to floorWidth/2, alternate tiles are black
    int i = (int)((intersectionPoint.x + floorWidth / 2) / tileWidth);
    int j = (int)((intersectionPoint.y + floorWidth / 2) / tileWidth);

    // if outside the floor, return black
    if (i < 0 || j < 0 || i >= tesselation || j >= tesselation) {
      return Color(0, 0, 0);
    }

    if ((i + j) % 2 == 0) {
      return Color(1, 1, 1);
    } else {
      return Color(0, 0, 0);
    }
  }
  Vec getNormalAtIntersectionPoint(Vec intersectionPoint) {
    return Vec(0, 0, 1);
  }
};

class General : public Object {
  double A, B, C, D, E, F, G, H, I, J;
  // the equation is Ax^2 + By^2 + Cz^2 + Dxy + Ezx + Fyz + Gx + Hy + Iz + J = 0
  BoundingBox boundingBox;

private:
  double delX(Vec intersectionPoint) {
    return 2 * A * intersectionPoint.x + D * intersectionPoint.y +
           E * intersectionPoint.z + G;
  }

  double delY(Vec intersectionPoint) {
    return 2 * B * intersectionPoint.y + D * intersectionPoint.x +
           F * intersectionPoint.z + H;
  }

  double delZ(Vec intersectionPoint) {
    return 2 * C * intersectionPoint.z + E * intersectionPoint.x +
           F * intersectionPoint.y + I;
  }

public:
  General(std::vector<double> parameters, BoundingBox boundingBox) {
    this->A = parameters[0];
    this->B = parameters[1];
    this->C = parameters[2];
    this->D = parameters[3];
    this->E = parameters[4];
    this->F = parameters[5];
    this->G = parameters[6];
    this->H = parameters[7];
    this->I = parameters[8];
    this->J = parameters[9];
    this->boundingBox = boundingBox;
  }

  void draw() {
    // openGL code to draw general object
    // avoid drawing the object
  }

  double getTmin(Ray *ray) {
    double a = A * ray->dir.x * ray->dir.x + B * ray->dir.y * ray->dir.y +
               C * ray->dir.z * ray->dir.z + D * ray->dir.x * ray->dir.y +
               E * ray->dir.x * ray->dir.z + F * ray->dir.y * ray->dir.z;

    double b =
        2 *
        (A * ray->start.x * ray->dir.x + B * ray->start.y * ray->dir.y +
         C * ray->start.z * ray->dir.z +
         0.5 * (D * (ray->start.x * ray->dir.y + ray->start.y * ray->dir.x) +
                E * (ray->start.x * ray->dir.z + ray->start.z * ray->dir.x) +
                F * (ray->start.y * ray->dir.z + ray->start.z * ray->dir.y)) +
         G * ray->dir.x + H * ray->dir.y + I * ray->dir.z);

    double c =
        A * ray->start.x * ray->start.x + B * ray->start.y * ray->start.y +
        C * ray->start.z * ray->start.z + D * ray->start.x * ray->start.y +
        E * ray->start.x * ray->start.z + F * ray->start.y * ray->start.z +
        G * ray->start.x + H * ray->start.y + I * ray->start.z + J;

    double discriminant = b * b - 4 * a * c;

    if (discriminant < 0) {
      return -1;
    }

    if (discriminant < EPSILON) {
      double t = -b / (2 * a);
      if (t < 0 || !boundingBox.isInside(ray->start + ray->dir * t)) {
        return -1;
      }
      return t;
    }

    double t1 = (-b + sqrt(discriminant)) / (2 * a);
    double t2 = (-b - sqrt(discriminant)) / (2 * a);

    if (t1 < 0 && t2 < 0) {
      return -1;
    }

    if (t1 < 0) {
      Vec intersectionPoint = ray->start + ray->dir * t2;
      if (!boundingBox.isInside(intersectionPoint)) {
        return -1;
      }
      return t2;
    }

    if (t2 < 0) {
      Vec intersectionPoint = ray->start + ray->dir * t1;
      if (!boundingBox.isInside(intersectionPoint)) {
        return -1;
      }
      return t1;
    }

    double t = std::min(t1, t2);

    Vec intersectionPoint = ray->start + ray->dir * t;
    if (!boundingBox.isInside(intersectionPoint)) {
      t = std::max(t1, t2);
      intersectionPoint = ray->start + ray->dir * t;
      if (!boundingBox.isInside(intersectionPoint)) {
        return -1;
      }
    }

    return t;
  }

  Color getColorAt(Vec intersectionPoint) { return color; }

  Vec getNormalAtIntersectionPoint(Vec intersectionPoint) {
    double x = delX(intersectionPoint);
    double y = delY(intersectionPoint);
    double z = delZ(intersectionPoint);

    Vec normal = Vec(x, y, z);
    return normal.getNormalizedResult();
  }
};