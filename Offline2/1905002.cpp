#include <fstream>
#include <iostream>
#include <stack>
#include <string>

#include "bitmap_image.hpp"
#include "triangle.cpp"

#define MAX_Z_BUFFER_VALUE 2
#define Z_LIMIT 1.0

using namespace std;

vector<Triangle> triangles_after_projection;
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

      triangles_after_projection.push_back(triangle);
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
        current = getIdentityMatrix();
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

vector<vector<double>> initialize_2D_double_vector(double value, int row,
                                                   int col) {
  vector<vector<double>> result;
  for (int i = 0; i < row; i++) {
    vector<double> row;
    for (int j = 0; j < col; j++) {
      row.push_back(value);
    }
    result.push_back(row);
  }
  return result;
}

vector<vector<Color>> initialize_frame_buffer(int row, int col) {
  vector<vector<Color>> result;
  for (int i = 0; i < row; i++) {
    vector<Color> row;
    for (int j = 0; j < col; j++) {
      Color color;
      row.push_back(color);
    }
    result.push_back(row);
  }
  return result;
}

int get_min_y_value_index(vector<vector<double>> points) {
  int min_y_index = 0;
  double min_y = points[0][1];
  for (int i = 1; i < points.size(); i++) {
    if (points[i][1] < min_y) {
      min_y = points[i][1];
      min_y_index = i;
    }
  }
  return min_y_index;
}

int get_max_y_value_index(vector<vector<double>> points) {
  int max_y_index = 0;
  double max_y = points[0][1];
  for (int i = 1; i < points.size(); i++) {
    if (points[i][1] > max_y) {
      max_y = points[i][1];
      max_y_index = i;
    }
  }
  return max_y_index;
}

class Line {
public:
  vector<double> point1; // the one with the smaller y value
  vector<double> point2;

  Line(vector<double> point1, vector<double> point2) {
    this->point1 = point1;
    this->point2 = point2;

    if (point1[1] > point2[1]) {
      vector<double> temp = point1;
      point1 = point2;
      point2 = temp;
    }
  }
};

void z_buffer_algorithm(string input_filename, string output_filename) {
  int width, height;
  double left_limit_x, right_limit_x, bottom_limit_y, top_limit_y,
      front_limit_z, back_limit_z;

  ifstream in(input_filename);
  streambuf *cinbuf = cin.rdbuf();
  cin.rdbuf(in.rdbuf());
  cin >> width >> height;
  cin.rdbuf(cinbuf);

  left_limit_x = -1;
  right_limit_x = 1;

  bottom_limit_y = -1;
  top_limit_y = 1;

  front_limit_z = -Z_LIMIT;
  back_limit_z = Z_LIMIT;

  double dx = (right_limit_x - left_limit_x) / width;
  double dy = (top_limit_y - bottom_limit_y) / height;

  double top_y = top_limit_y - dy / 2;
  double left_x = left_limit_x + dx / 2;

  vector<vector<double>> z_buffer =
      initialize_2D_double_vector(MAX_Z_BUFFER_VALUE, height, width);

  vector<vector<Color>> frame_buffer = initialize_frame_buffer(height, width);

  for (int i = 0; i < triangles_after_projection.size(); i++) {
    Triangle triangle = triangles_after_projection[i];

    vector<vector<double>> points_list = {triangle.v1, triangle.v2,
                                          triangle.v3};

    double min_y_value =
        min(min(triangle.v1[1], triangle.v2[1]), triangle.v3[1]);
    double max_y_value =
        max(max(triangle.v1[1], triangle.v2[1]), triangle.v3[1]);

    int start_row = (int)((top_y - max_y_value) / dy);  // top-most point
    int bottom_row = (int)((top_y - min_y_value) / dy); // bottom-most point

    if ((start_row < 0 && bottom_row < 0) ||
        (start_row > (height - 1) &&
         bottom_row > (height - 1))) { // totally outside the screen
      continue;
    }

    // clipping
    if (start_row < 0) {
      start_row = 0;
    }

    if (bottom_row > (height - 1)) {
      bottom_row = height - 1;
    }

    // int min_y_index = get_min_y_value_index(points_list);
    int max_y_index = get_max_y_value_index(points_list);

    // vector<double> min_y_point = points_list[min_y_index];
    vector<double> max_y_point = points_list[max_y_index];

    // delete the max_y_point from the points_list
    points_list.erase(points_list.begin() + max_y_index);

    vector<vector<double>> sorted_x = points_list;
    if (sorted_x[0][0] > sorted_x[1][0]) {
      vector<double> temp = sorted_x[0];
      sorted_x[0] = sorted_x[1];
      sorted_x[1] = temp;
    }

    Line left_line = Line(max_y_point, sorted_x[0]);
    Line right_line = Line(max_y_point, sorted_x[1]);

    for (int row_no = start_row; row_no <= bottom_row; row_no++) {
      double y_s = top_y - row_no * dy;

      // check if left-line needs to updated
      if (y_s < left_line.point2[1]) {
        left_line = Line(left_line.point2, right_line.point2);
      }
      // check if right-line needs to updated
      if (y_s < right_line.point2[1]) {
        right_line = Line(right_line.point2, left_line.point2);
      }

      // calculate x_a, z_a  -- left_line
      double x_a, z_a;
      double x1 = left_line.point1[0];
      double y1 = left_line.point1[1];
      double z1 = left_line.point1[2];

      double x2 = left_line.point2[0];
      double y2 = left_line.point2[1];
      double z2 = left_line.point2[2];

      if (y1 == y2) {
        if (x1 < x2) {
          x_a = x1;
          z_a = z1;
        } else {
          x_a = x2;
          z_a = z2;
        }
      } else {
        x_a = x1 + (y_s - y1) * (x2 - x1) / (y2 - y1);
        z_a = z1 + (y_s - y1) * (z2 - z1) / (y2 - y1);
      }

      // calculate x_b, z_b
      double x_b, z_b;
      x1 = right_line.point1[0];
      y1 = right_line.point1[1];
      z1 = right_line.point1[2];

      x2 = right_line.point2[0];
      y2 = right_line.point2[1];
      z2 = right_line.point2[2];

      if (y1 == y2) {
        if (x1 > x2) {
          x_b = x1;
          z_b = z1;
        } else {
          x_b = x2;
          z_b = z2;
        }
      } else {
        x_b = x1 + (y_s - y1) * (x2 - x1) / (y2 - y1);
        z_b = z1 + (y_s - y1) * (z2 - z1) / (y2 - y1);
      }

      // // in top-most point, sometime may occur
      // if (x_b < x_a) {
      //   std::swap(x_a,x_b);
      //   std::swap(z_a,z_b);
      // }

      // int start_col_index = (int)((x_a - left_x) / dx);
      // int end_col_index = (int)((x_b - left_x) / dx);

      int start_col_index = round((x_a - left_x) / dx);
      int end_col_index = round((x_b - left_x) / dx);

      if ((start_col_index < 0 && end_col_index < 0) ||
          (start_col_index > (width - 1) &&
           end_col_index > (width - 1))) { // totally outside the screen
        continue;
      }

      // clipping
      if (start_col_index < 0) {
        start_col_index = 0;
      }

      if (end_col_index > (width - 1)) {
        end_col_index = width - 1;
      }

      for (int col_no = start_col_index; col_no <= end_col_index; col_no++) {
        double x_p = left_x + col_no * dx;
        double z_p = z_a + (x_p - x_a) * (z_b - z_a) / (x_b - x_a);

        if ((z_p < z_buffer[row_no][col_no]) && (z_p >= front_limit_z) &&
            (z_p <= back_limit_z)) {
          z_buffer[row_no][col_no] = z_p;
          frame_buffer[row_no][col_no] = triangle.color;
        }
      }
    }
  }

  ofstream z_file;
  z_file.open(output_filename);

  for (int i = 0; i < z_buffer.size(); i++) {
    for (int j = 0; j < z_buffer[i].size(); j++) {
      if (z_buffer[i][j] != MAX_Z_BUFFER_VALUE)
        z_file << setprecision(6) << z_buffer[i][j] << "\t";
    }
    z_file << "\n";
  }
  z_file.close();

  bitmap_image image(width, height);
  image.set_all_channels(0, 0, 0);
  for (int i = 0; i < frame_buffer.size(); i++) {
    for (int j = 0; j < frame_buffer[i].size(); j++) {
      image.set_pixel(j, i, frame_buffer[i][j].r, frame_buffer[i][j].g,
                      frame_buffer[i][j].b);
    }
  }

  image.save_image("out.bmp");
}

int main() {

  process_scene("scene.txt", "stage1.txt", "stage2.txt", "stage3.txt");
  z_buffer_algorithm("config.txt", "z_buffer.txt");

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