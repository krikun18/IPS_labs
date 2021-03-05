#include <iostream>
#include <cmath>
#include <time.h>
#include <chrono>
#include <thread>

#define PI 3.14159265358979323846

struct {
	double sum[8];
	double start;
	double end;
	double step;
} Data;

void threadFun(int arg)
{
	int num = arg;
	double sum = 0;

	double start = Data.start;
	double end = Data.end;
	double step = Data.step;
	double i = start + (num + 1) * step;

	while (i<end) {
		sum += 5 / sqrt(8 - 4 * i*i);
		i += step*2;
	}

	Data.sum[num] = sum;
}

void int_threads(double* sum_vect, int num_of_part, double start, double end, int mode_fl = 0)
{
	double sum0 = 0;
	double sum1 = 0;
	double step = (end - start) / num_of_part;

	Data.step = step;	

	std::thread t1(threadFun, 0);
	std::thread t2(threadFun, 1);

	t1.join();
	t2.join();

	step = (end - start) / num_of_part;
	sum0 = 5 / sqrt(8 - 4 * start*start);
	sum1 = 5 / sqrt(8 - 4 * end*end);

	for(int i=0; i<2; i++)
	{
		sum0 += Data.sum[i];
		sum1 += Data.sum[i];
	}
	sum_vect[0] = sum0*step;
	sum_vect[1] = sum1*step;
}

void integral(double* sum_vect, int num_of_part, double start, double end, int mode_fl = 0)
{

	double sum0 = 0;
	double sum1 = 0;

	double i = start;
	double step = (end - start) / num_of_part;

	i = start;
	i += step;

	step = (end - start) / num_of_part;

	sum0 = 5 / sqrt(8 - 4 * start*start);
	sum1 = 5 / sqrt(8 - 4 * end*end);
	double sum = 0;
	
	#pragma loop(hint_parallel(2))
	while (i<end) {
		sum = 5 / sqrt(8 - 4 * i*i);
		sum0 += sum;
		sum1 += sum;
		i += step;
	}
	sum_vect[0] = sum0*step;
	sum_vect[1] = sum1*step;
}

void main()
{
	double start = -1;
	double end = 1;

	int int_step = 100;
	double sum_vect[2] = {0,0};

	Data.start = start;
	Data.end = end;

	for(int j=0; j<=4; j++)
	{
		std::chrono::high_resolution_clock::time_point beg_clk = std::chrono::high_resolution_clock::now();
		integral(sum_vect, int_step, start, end);
		//int_threads(sum_vect, int_step, start, end);
		std::chrono::high_resolution_clock::time_point end_clk = std::chrono::high_resolution_clock::now();
		std::cout << "Number of inervals: " << int_step << "\n";
		std::cout<<"\n"<< sum_vect[0]<< "  ad  "<< sum_vect[1]<<"\n";	
		std::cout << "Difference:\nLeft rectangles:" << sum_vect[0] - PI / 2 * 2.5 
		<< "\nRight rectangles:" << sum_vect[1] - PI / 2 * 2.5;
		std::cout << "\n" << sum_vect[0] << "\n" << sum_vect[1] << "\n";
		auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(end_clk - beg_clk);
		std::cout << "Execution time: " << elapsed.count()* 1e-9 << " seconds\n\n";

		int_step *= 10;
	}


}

