#include <fstream>
#include <iostream>
#include <stack>
#include <string>

#include "triangle.cpp"

using namespace std;

vector<Triangle> triangles;
Matrix view_trasnformation_matrix;
Matrix projection_transformation_matrix;

void process_scene(string input_filename, string stage1_out, string stage2_out,
                   string stage3_out) {
  //   redirect cin to input_filename
  ifstream in(input_filename);
  streambuf *cinbuf = cin.rdbuf();
  cin.rdbuf(in.rdbuf());

  // opening the output files
  ofstream stage1_file;
  stage1_file.open(stage1_out);
  ofstream stage2_file;
  stage2_file.open(stage2_out);
  ofstream stage3_file;
  stage3_file.open(stage3_out);

  double eye_x, eye_y, eye_z;
  double look_x, look_y, look_z;
  double up_x, up_y, up_z;
  double fov_y, aspect_ratio, near, far;
  stack<Matrix> model_view_stack;

  cin >> eye_x >> eye_y >> eye_z;
  cin >> look_x >> look_y >> look_z;
  cin >> up_x >> up_y >> up_z;
  cin >> fov_y >> aspect_ratio >> near >> far;

  view_trasnformation_matrix = getViewTransformationMatrix(
      eye_x, eye_y, eye_z, look_x, look_y, look_z, up_x, up_y, up_z);
  projection_transformation_matrix =
      getProjectionTransformationMatrix(fov_y, aspect_ratio, near, far);

  Matrix current = getIdentityMatrix();
  model_view_stack.push(current);

  while (true) {
    string command;
    cin >> command;

    if (command == "triangle") {
      double x1, y1, z1;
      double x2, y2, z2;
      double x3, y3, z3;

      cin >> x1 >> y1 >> z1;
      cin >> x2 >> y2 >> z2;
      cin >> x3 >> y3 >> z3;

      Triangle triangle = Triangle(x1, y1, z1, x2, y2, z2, x3, y3, z3);

      // modeling transformation
      triangle.applyTransformation(current);
      stage1_file << triangle;

      // viewing transformation
      triangle.applyTransformation(view_trasnformation_matrix);
      stage2_file << triangle;

      // projection transformation
      triangle.applyTransformation(projection_transformation_matrix);
      stage3_file << triangle;
    } else if (command == "translate") {
      double tx, ty, tz;
      cin >> tx >> ty >> tz;

      current = current * getTranslationMatrix(tx, ty, tz);
      ;
    } else if (command == "scale") {
      double sx, sy, sz;
      cin >> sx >> sy >> sz;
      current = current * getScalingMatrix(sx, sy, sz);

    } else if (command == "rotate") {
      double angle, rx, ry, rz;
      cin >> angle >> rx >> ry >> rz;

      Matrix rotation_matrix = getRotationMatrix(angle, rx, ry, rz);
      current = current * rotation_matrix;
    } else if (command == "push") {
      model_view_stack.push(current);

    } else if (command == "pop") {
      if (model_view_stack.size() == 1) {
        cout << "Stack size is 1. Cannot pop anymore." << endl;
        continue;
      }
      current = model_view_stack.top();
      model_view_stack.pop();
    } else if (command == "end") {
      break;
    } else {
      cout << "Invalid command: " << command << endl;
    }
  }

  //   closing the output files
  stage1_file.close();
  stage2_file.close();
  stage3_file.close();
  cin.rdbuf(cinbuf);
}

int main() {

  process_scene("scene.txt", "stage1.txt", "stage2.txt", "stage3.txt");
  //   std::ifstream in(".txt");
  //   std::streambuf *cinbuf = std::cin.rdbuf(); // save old buf
  //   std::cin.rdbuf(in.rdbuf());                // redirect std::cin to
  //   in.txt!

  //   std::ofstream out("out.txt");
  //   std::streambuf *coutbuf = std::cout.rdbuf(); // save old buf
  //   std::cout.rdbuf(out.rdbuf());                // redirect std::cout to
  //   out.txt!

  //   std::cin.rdbuf(cinbuf);   // reset to standard input again
  //   std::cout.rdbuf(coutbuf); // reset to standard output again

  return 0;
}