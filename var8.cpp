/*$TET$$header*/

#include <templet.hpp>
#include <cmath>
#include <iostream>
#include <ctime>
#include <array>
#include <cstdlib>
#include <cmath>

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

class DataPoints : public templet::message {
public:
	DataPoints(templet::actor*a=0, templet::message_adaptor ma=0) :templet::message(a, ma) {}
	std::array<Point, 4> points;
};

/*$TET$*/

#pragma templet !source(in?DataPoints,out!DataPoints)

struct source :public templet::actor {
	static void on_in_adapter(templet::actor*a, templet::message*m) {
		((source*)a)->on_in(*(DataPoints*)m);}
	static void on_out_adapter(templet::actor*a, templet::message*m) {
		((source*)a)->on_out(*(DataPoints*)m);}

	source(templet::engine&e) :source() {
		source::engines(e);
	}

	source() :templet::actor(true),
		out(this, &on_out_adapter)
	{
/*$TET$source$source*/
/*$TET$*/
	}

	void engines(templet::engine&e) {
		templet::actor::engine(e);
/*$TET$source$engines*/
/*$TET$*/
	}

	void start() {
/*$TET$source$start*/
        on_out(out);
/*$TET$*/
	}

	inline void on_in(DataPoints&m) {
/*$TET$source$in*/
        stop();
/*$TET$*/
	}

	inline void on_out(DataPoints&m) {
/*$TET$source$out*/
        srand(10);
        for(int i = 0; i < 4; i++) {
            m.points[i] = Point(rand() % 100, rand() % 100);
            std::cout << "Point: X = " << m.points[i].X << " Y = " << m.points[i].Y << std::endl;
        }
        m.send();
/*$TET$*/
	}

	void in(DataPoints&m) { m.bind(this, &on_in_adapter); }
	DataPoints out;

/*$TET$source$$footer*/
/*$TET$*/
};

#pragma templet pointHendler(in?DataPoints,out!DataPoints)

struct pointHendler :public templet::actor {
	static void on_in_adapter(templet::actor*a, templet::message*m) {
		((pointHendler*)a)->on_in(*(DataPoints*)m);}
	static void on_out_adapter(templet::actor*a, templet::message*m) {
		((pointHendler*)a)->on_out(*(DataPoints*)m);}

	pointHendler(templet::engine&e) :pointHendler() {
		pointHendler::engines(e);
	}

	pointHendler() :templet::actor(false),
		out(this, &on_out_adapter)
	{
/*$TET$pointHendler$pointHendler*/
/*$TET$*/
	}

	void engines(templet::engine&e) {
		templet::actor::engine(e);
/*$TET$pointHendler$engines*/
/*$TET$*/
	}

	inline void on_in(DataPoints&m) {
/*$TET$pointHendler$in*/
        std::array<Point, 4> points = m.points;
        int x;
        int y;
        int marker = -1;
        for(int i = 0; i < points.size(); i++) {
            if(!points[i].isBusy) {
                x = points[i].X;
                y = points[i].Y;
                points[i].isBusy = true;
                marker = i;
                break;
            }
        }
        if(marker == -1) {
            std::cout << "All points are busy" << std::endl;
            return;
        }
        out.points = points;
        out.send();
        std::cout << "Points sended" << std::endl;
        int resX;
        int resY;
        double res = 100000;
        for(int i = 0; i < points.size(); i++) {
            if(i != marker) {
                double tmp = std::sqrt(std::pow(points[i].X - x, 2) + std::pow(points[i].Y - y, 2));
                if(tmp < res) {
                    res = tmp;
                    resX = points[i].X;
                    resY = points[i].Y;
                }
            }
        }
        std::cout << "The nearest point for X: " << x << " Y: " << y << " is point with X: " << resX << " Y: " << resY << ". The distance is " << res << std::endl;
/*$TET$*/
	}

	inline void on_out(DataPoints&m) {
/*$TET$pointHendler$out*/
/*$TET$*/
	}

	void in(DataPoints&m) { m.bind(this, &on_in_adapter); }
	DataPoints out;

/*$TET$pointHendler$$footer*/
/*$TET$*/
};

/*$TET$$footer*/
int main() 
{
    std::cout << "started" << std::endl;
    templet::engine eng;
    
    source source_w(eng);
    pointHendler a1(eng),a2(eng),a3(eng),a4(eng);
    
    a1.in(source_w.out);
    a2.in(a1.out);
    a3.in(a2.out);
    a4.in(a3.out);
    
    source_w.in(a4.out);
    
    eng.start();
    
    if (eng.stopped()) {
		std::cout << "DONE !!!";
		return EXIT_SUCCESS;
	}

	std::cout << "something broke (((" << std::endl;
	return EXIT_FAILURE;
}
/*$TET$*/
