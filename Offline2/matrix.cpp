#pragma once
#include "vec.cpp"
#include "vector"

using namespace std;

// this class is implemented for 4x4 matrices only
class Matrix {

public:
  double mat[4][4];
  Matrix() {
    for (int i = 0; i < 4; i++) {
      for (int j = 0; j < 4; j++) {
        mat[i][j] = 0.0;
      }
    }
  }

  
  Matrix operator*(Matrix m);
  vector<double> operator*(vector<double> v);
};

Matrix Matrix::operator*(Matrix m) {
  Matrix result;
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      for (int k = 0; k < 4; k++) {
        result.mat[i][j] += mat[i][k] * m.mat[k][j];
      }
    }
  }
  return result;
}

vector<double> Matrix::operator*(vector<double> v) {
  // v is a 4x1 matrix
  vector<double> result;
  for (int i = 0; i < 4; i++) {
    double sum = 0;
    for (int j = 0; j < 4; j++) {
      sum += mat[i][j] * v[j];
    }
    result.push_back(sum);
  }

  // divide all the elements by the last element
  for (int i = 0; i < 4; i++) {
    result[i] /= result[3];
  }

  return result;
}

Matrix getIdentityMatrix() {
  Matrix result;
  result.mat[0][0] = 1;
  result.mat[1][1] = 1;
  result.mat[2][2] = 1;
  result.mat[3][3] = 1;

  return result;
}

Matrix getTranslationMatrix(double tx, double ty, double tz) {
  Matrix result;
  result.mat[0][0] = 1;
  result.mat[1][1] = 1;
  result.mat[2][2] = 1;
  result.mat[3][3] = 1;
  result.mat[0][3] = tx;
  result.mat[1][3] = ty;
  result.mat[2][3] = tz;

  return result;
}

Matrix getScalingMatrix(double sx, double sy, double sz) {
  Matrix result;
  result.mat[0][0] = sx;
  result.mat[1][1] = sy;
  result.mat[2][2] = sz;
  result.mat[3][3] = 1;

  return result;
}

Matrix getRotationMatrix(double angel_in_degree, double ax, double ay,
                         double az) {
    Vec axis = Vec(ax, ay, az);
    axis = axis.getNormalizedResult();

    double theta = angel_in_degree * M_PI / 180;

    Vec c1 = Vec(1,0,0).rotateAroundAxis(axis, theta);
    Vec c2 = Vec(0,1,0).rotateAroundAxis(axis, theta);
    Vec c3 = Vec(0,0,1).rotateAroundAxis(axis, theta);

    Matrix result;
    result.mat[0][0] = c1.x;
    result.mat[1][0] = c1.y;
    result.mat[2][0] = c1.z;

    result.mat[0][1] = c2.x;
    result.mat[1][1] = c2.y;
    result.mat[2][1] = c2.z;

    result.mat[0][2] = c3.x;
    result.mat[1][2] = c3.y;
    result.mat[2][2] = c3.z;

    result.mat[3][3] = 1;

    return result;
}

Matrix getViewTransformationMatrix(double ex, double ey, double ez, double lx, double ly, double lz, double upx, double upy, double upz) {
    Vec eye = Vec(ex, ey, ez);
    Vec look_at = Vec(lx, ly, lz);
    Vec up = Vec(upx, upy, upz);

    Vec l = (look_at - eye).getNormalizedResult();
    Vec r = (l.getCrossProduct(up)).getNormalizedResult();
    Vec u = (r.getCrossProduct(l)).getNormalizedResult();

    Matrix T = getTranslationMatrix(-ex, -ey, -ez);
    Matrix R = Matrix();

    R.mat[0][0] = r.x;
    R.mat[0][1] = r.y;
    R.mat[0][2] = r.z;

    R.mat[1][0] = u.x;
    R.mat[1][1] = u.y;
    R.mat[1][2] = u.z;

    R.mat[2][0] = -l.x;
    R.mat[2][1] = -l.y;
    R.mat[2][2] = -l.z;

    R.mat[3][3] = 1;

    return R * T;
}

Matrix getProjectionTransformationMatrix(double fovY, double aspectRatio, double near, double far) {
    Matrix result;
    double fovX = fovY * aspectRatio;

    double t = near * tan((fovY / 2) * M_PI / 180);
    double r = near * tan((fovX / 2) * M_PI / 180);

    result.mat[0][0] = near / r;
    result.mat[1][1] = near / t;
    result.mat[2][2] = -(far + near) / (far - near);
    result.mat[2][3] = -(2 * far * near) / (far - near);
    result.mat[3][2] = -1;

    return result;
}