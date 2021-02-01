#include <iostream>
#include <dlfcn.h>
#include <vector>
#include <stdio.h>
#include <unistd.h>
#include <vector>

#include "../src/PIDController.hpp"


using namespace std;


int main(int argc, char* argv[])
{
	// vector of samples
	vector<double> samples;
	
	// samples per seconds
	int sample_timestep = 10;

	// time at which the jump occurs
	double jump_time = 2;

	// time to simulate
	double overall_time = 10;

	// value to jump to
	double final_value = 1;


	// fill the vector with zeros until jump occurs
	for (int i = 0; i < (int)(jump_time*sample_timestep); i++)
	{
		samples.push_back(0);
	}

	for (int i = 0; i < (int)((overall_time-jump_time)*sample_timestep); i++)
	{
		samples.push_back(final_value);
	}

	cout << "Default Values" << endl;

	// print all values to console
	for (double val : samples)
	{
		cout << val;
	}
	cout << endl;




	cout << "P-Controller" << endl;

	// define PID controller as P controller
	PIDController p = PIDController(5,0,0);

	// print all values to console
	for (double val : samples)
	{
		cout << p.calculate(0-val);
	}
	cout << endl;



	cout << "PID-Controller" << endl;

	// define PID controller with random parameters
	PIDController pid = PIDController(5,4,3);

	// print all values to console
	for (double val : samples)
	{
		cout << pid.calculate(0-val);
	}
	cout << endl;

	// the pid controll loop is not closed
	// values in samples will not be influenced
	
}
