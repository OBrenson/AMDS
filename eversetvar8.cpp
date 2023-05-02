/*$TET$$header*/

#define _CRT_SECURE_NO_DEPRECATE

#include <templet.hpp>
#include <everest.hpp>
#include <string>
#include <cmath>
#include <iostream>
#include <ctime>
#include <array>
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

class DataPoints : public templet::message {
public:
	DataPoints(templet::actor*a=0, templet::message_adaptor ma=0) :templet::message(a, ma) {}
	std::array<Point, 4> points;
    Point p;
    std::string mes;
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
            m.points[i] = Point(i, i + 1);
            std::cout << "Point: X = " << m.points[i].X << " Y = " << m.points[i].Y << std::endl;
        }
        std::string mes = "";
        for(int i = 0; i < m.points.size(); i++) {
            mes = mes + std::to_string(m.points[i].X) + "," + std::to_string(m.points[i].Y) + ",";
        }
        mes[mes.length() - 1] = ';';
        m.mes = mes;
        m.send();
/*$TET$*/
	}

	void in(DataPoints&m) { m.bind(this, &on_in_adapter); }
	DataPoints out;

/*$TET$source$$footer*/
/*$TET$*/
};

#pragma templet pointHendler(in?DataPoints,out!DataPoints,t:everest)

struct pointHendler :public templet::actor {
	static void on_in_adapter(templet::actor*a, templet::message*m) {
		((pointHendler*)a)->on_in(*(DataPoints*)m);}
	static void on_out_adapter(templet::actor*a, templet::message*m) {
		((pointHendler*)a)->on_out(*(DataPoints*)m);}
	static void on_t_adapter(templet::actor*a, templet::task*t) {
		((pointHendler*)a)->on_t(*(templet::everest_task*)t);}

	pointHendler(templet::engine&e,templet::everest_engine&te_everest) :pointHendler() {
		pointHendler::engines(e,te_everest);
	}

	pointHendler() :templet::actor(false),
		out(this, &on_out_adapter),
		t(this, &on_t_adapter)
	{
/*$TET$pointHendler$pointHendler*/
        _in = 0;
        t.app_id("6450a3d5140000e70825d1c6");
/*$TET$*/
	}

	void engines(templet::engine&e,templet::everest_engine&te_everest) {
		templet::actor::engine(e);
		t.engine(te_everest);
/*$TET$pointHendler$engines*/
/*$TET$*/
	}

	inline void on_in(DataPoints&m) {
/*$TET$pointHendler$in*/
        _in = &m;
        
        std::array<Point, 4> points = m.points;
		int index = 0;
        for(int i = 0; i < points.size(); i++) {
            if(!points[i].isBusy) {
				index = i;
                points[i].isBusy = true;
                _in -> p = points[i];
                break;
            }
        }
          
		json in;
		in["name"] = "var8";
        in["inputs"]["input-string"] = m.mes;
        std::cout<<m.mes<<"\n"<<std::to_string(index)<<"\n";
        in["inputs"]["input-index"] = std::to_string(index);
		out.points = points;
        out.mes = m.mes;
        // std::cout<<in;
         out.send();
		if (t.submit(in)) {
            std::cout << "task submit succeeded" << std::endl;
        }
		else std::cout << "task submit failed" << std::endl;
/*$TET$*/
	}

	inline void on_out(DataPoints&m) {
/*$TET$pointHendler$out*/
/*$TET$*/
	}

	inline void on_t(templet::everest_task&t) {
/*$TET$pointHendler$t*/
        json res = t.result();
        std::string str = res["output-string"];
        
		std::cout << str << std::endl;
		// _in->send();
/*$TET$*/
	}

	void in(DataPoints&m) { m.bind(this, &on_in_adapter); }
	DataPoints out;
	templet::everest_task t;

/*$TET$pointHendler$$footer*/
std::array<Point,4> convertStrtoArr(std::string str)
{
    int str_length = str.length();
    int arr[str_length] = { 0 };
    int j = 0, i = 0;
    for (i = 0; str[i] != ';'; i++) {
 
        if (str[i] == ',')
            continue;
         if (str[i] == ' '){
            j++;
        }
        else {
            arr[j] = arr[j] * 10 + (str[i] - 48);
        }
    }
    
    std::array<Point,4> res;
    for (i = 0; i <= j; i = i+2) {
       res[i/2] = Point(arr[i], arr[i+1]);
    }
     return res;
}
    
    DataPoints* _in;
/*$TET$*/
};

/*$TET$$footer*/
int main() 
{
    std::cout << "started" << std::endl;
    
    templet::engine eng;
	templet::everest_engine teng("Oleg-brenyov@yandex.ru", "Qwerty12345");
    
    if (!teng) {
		std::cout << "task engine is not connected to the Everest server..." << std::endl;
		return EXIT_FAILURE;
	}
    
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

    eng.start();
    
try_continue:
    teng.run();

	if (eng.stopped()) {
		std::cout << "DONE !!!";
		return EXIT_SUCCESS;
	}

	static int recovery_tries = 3;
	templet::everest_error cntxt;

	if (teng.error(&cntxt)) {
		std::cout << "...task engine error..." << std::endl;

		if (recovery_tries--) {
			std::cout << "error information:" << std::endl;

			std::cout << "type ID : " << *cntxt._type << std::endl;
			std::cout << "HTML response code : " << cntxt._code << std::endl;
			std::cout << "HTML response : " << cntxt._response << std::endl;
			std::cout << "task input : " << cntxt._task_input << std::endl;

			// trying to fix an error
			switch (*cntxt._type) {
				case templet::everest_error::NOT_CONNECTED:
				{
					std::cout << "error string : NOT_CONNECTED" << std::endl;
					std::cout << "the task engine is not initialized properly -- fatal error, exiting" << std::endl;
					return EXIT_FAILURE;
				}
				case templet::everest_error::SUBMIT_FAILED:
				{
					std::cout << "error string : SUBMIT_FAILED" << std::endl;
					std::cout << "resubmitting the task" << std::endl;
					json input = json::parse(cntxt._task_input);
					// here you may fix something in the input 
					cntxt._task->resubmit(input);
					break;
				}
				case templet::everest_error::TASK_CHECK_FAILED:
				{
					std::cout << "error string : TASK_CHECK_FAILED" << std::endl;
					std::cout << "trying to check the task status again" << std::endl;
					*cntxt._type = templet::everest_error::NOT_ERROR;
					break;
				}
				case templet::everest_error::TASK_FAILED_OR_CANCELLED:
				{
					std::cout << "error string : TASK_FAILED_OR_CANCELLED" << std::endl;
					std::cout << "resubmitting the task" << std::endl;
					json input = json::parse(cntxt._task_input);
					// here you may fix something in the input 
					cntxt._task->resubmit(input);
				}
			}
	
			goto try_continue;
		}
	}
	else 
		std::cout << "logical error" << std::endl;
	return EXIT_FAILURE;
}
/*$TET$*/
