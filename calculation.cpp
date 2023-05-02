#include <iostream>
#include <cstdlib>
#include <array>
#include <cmath>
#include <string>
#include <cstdlib>

class Point {
    public:
        Point(){}
        Point(int x, int y) {
            X = x;
            Y = y;
        }
    int X;
    int Y;
    bool isBusy = false;
};

std::array<Point,4> convertStrtoArr(std::string str) {
    int str_length = str.length();
    int arr[str_length] = { 0 };
    int j = 0, i = 0;
    while (str[i] != ';') {
 
        if (str[i] == ','){
            j++;
        }
        else {
            arr[j] = arr[j] * 10 + (str[i] - 48);
        }
        i++;
    }
    
    std::array<Point,4> res;
    for (i = 0; i <= j + 1; i = i+2) {
       res[i/2] = Point(arr[i], arr[i+1]);
        // std::string s = std::to_string(res[i/2].Y);
        // std::cout << s  << "\n" << std::endl;
    }
    return res;
}

int main(int argc, char* argv[]) {
    std::string str = argv[1];
    std::array<Point,4> arr = convertStrtoArr(str);

    int index = atoi(argv[2]);

    int resX;
    int resY;
    double res = 100000;
    Point p = arr[index];
    for(int i = 0; i < 4; i++) {
        if(arr[i].X != p.X && arr[i].Y != p.Y) {
            double tmp = std::sqrt(std::pow(arr[i].X - p.X, 2) + std::pow(arr[i].Y - p.Y, 2));
            if(tmp < res) {
                res = tmp;
                resX = arr[i].X;
                resY = arr[i].Y;
            }
        }
    }
    std::cout << "The nearest point for X: " << p.X << " Y: " << p.Y << " is point with X: " << resX << " Y: " << resY << ". The distance is " << res << 
            std::endl;
}