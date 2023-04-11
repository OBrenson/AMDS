/*$TET$$header*/

#include <templet.hpp>
#include <cmath>
#include <iostream>
#include <ctime>
#include <array>
#include <cstdlib>
#include <cmath>
#include <basesim.hpp>
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
    Point p;
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
        
        for(int i = 0; i < 4; i++) {
            m.points[i] = Point(i, i);
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

#pragma templet pointHendler(in?DataPoints,out!DataPoints,t:basesim)

struct pointHendler :public templet::actor {
	static void on_in_adapter(templet::actor*a, templet::message*m) {
		((pointHendler*)a)->on_in(*(DataPoints*)m);}
	static void on_out_adapter(templet::actor*a, templet::message*m) {
		((pointHendler*)a)->on_out(*(DataPoints*)m);}
	static void on_t_adapter(templet::actor*a, templet::task*t) {
		((pointHendler*)a)->on_t(*(templet::basesim_task*)t);}

	pointHendler(templet::engine&e,templet::basesim_engine&te_basesim) :pointHendler() {
		pointHendler::engines(e,te_basesim);
	}

	pointHendler() :templet::actor(false),
		out(this, &on_out_adapter),
		t(this, &on_t_adapter)
	{
/*$TET$pointHendler$pointHendler*/
            _in = 0;
/*$TET$*/
	}

	void engines(templet::engine&e,templet::basesim_engine&te_basesim) {
		templet::actor::engine(e);
		t.engine(te_basesim);
/*$TET$pointHendler$engines*/
/*$TET$*/
	}

	inline void on_in(DataPoints&m) {
/*$TET$pointHendler$in*/
        _in = &m;
        std::array<Point, 4> points = m.points;
        for(int i = 0; i < points.size(); i++) {
            if(!points[i].isBusy) {
                points[i].isBusy = true;
                _in -> p = points[i];
                break;
            }
        }
        out.points = points;
        out.send();
        t.submit();
/*$TET$*/
	}

	inline void on_out(DataPoints&m) {
/*$TET$pointHendler$out*/
/*$TET$*/
	}

	inline void on_t(templet::basesim_task&t) {
/*$TET$pointHendler$t*/
        calculate();
        t.delay(1);
/*$TET$*/
	}

	void in(DataPoints&m) { m.bind(this, &on_in_adapter); }
	DataPoints out;
	templet::basesim_task t;

/*$TET$pointHendler$$footer*/
    void calculate() {
        int resX;
        int resY;
        double res = 100000;
        if (access(_in)) {
            Point p = _in->p;
        	for(int i = 0; i < _in->points.size(); i++) {
            	if(*(&_in->points[i].X) != p.X && *(&_in->points[i].Y) != p.Y) {
                	double tmp = std::sqrt(std::pow(_in -> points[i].X - p.X, 2) + std::pow(_in ->points[i].Y - p.Y, 2));
                	if(tmp < res) {
                    	res = tmp;
                    	resX = _in->points[i].X;
                    	resY = _in->points[i].Y;
                	}
            	}
        	}
        	std::cout << "The nearest point for X: " << p.X << " Y: " << p.Y << " is point with X: " << resX << " Y: " << resY << ". The distance is " << res << 
            	std::endl;
        }
	}
    
    DataPoints* _in;
/*$TET$*/
};

/*$TET$$footer*/
int main() 
{
    std::cout << "started" << std::endl;
    
    templet::engine eng;
    templet::basesim_engine teng;
    
    int actors_num = 4;
    
	source       source_w(eng);
	pointHendler     actors[actors_num];

    actors[0].in(source_w.out);
    
    for(int i=1; i < actors_num; i++ )       
        actors[i].in(actors[i-1].out);

    source_w.in(actors[actors_num - 1].out);
    
    for(int i=0;i<actors_num;i++){
        actors[i].engines(eng,teng);
    }
    srand(time(NULL));

    eng.start();
    teng.run();

	if (eng.stopped()) {
		std::cout << "DONE !!!";
        std::cout << "Maximum number of tasks executed in parallel : " << teng.Pmax() << std::endl;
		std::cout << "Time of sequential execution of all tasks    : " << teng.T1() << std::endl;
		std::cout << "Time of parallel   execution of all tasks    : " << teng.Tp() << std::endl;
		return EXIT_SUCCESS;
	}

	std::cout << "something broke (((" << std::endl;
	return EXIT_FAILURE;
}
/*$TET$*/
