#include "iostream"
#include "vector"
#include "matrix.cpp"
#include "iomanip"

using namespace std;


class Triangle{
public:
    vector<double> v1;
    vector<double> v2;
    vector<double> v3;
    Triangle(vector<double> v1, vector<double> v2, vector<double> v3){
        this->v1 = v1;
        this->v2 = v2;
        this->v3 = v3;
    }

    Triangle(double x1, double y1, double z1,
             double x2, double y2, double z2,
             double x3, double y3, double z3){
        this->v1 = {x1, y1, z1, 1};
        this->v2 = {x2, y2, z2, 1};
        this->v3 = {x3, y3, z3, 1};
    }

    void applyTransformation(Matrix transformation_matrix){
        this->v1 = transformation_matrix * v1;
        this->v2 = transformation_matrix * v2;
        this->v3 = transformation_matrix * v3;
    }

    // overload the << operator, output 6 decimal places
    friend ostream& operator<<(ostream& os, const Triangle& triangle){
        os << fixed << setprecision(7) << triangle.v1[0] << " " << triangle.v1[1] << " " << triangle.v1[2] << " \n";
        os << fixed << setprecision(7) << triangle.v2[0] << " " << triangle.v2[1] << " " << triangle.v2[2] << " \n";
        os << fixed << setprecision(7) << triangle.v3[0] << " " << triangle.v3[1] << " " << triangle.v3[2] << " \n";
        os << "\n";
        return os;
    }

    
};