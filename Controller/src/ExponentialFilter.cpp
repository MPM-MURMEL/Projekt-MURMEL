#include "ExponentialFilter.hpp"




ExponentialFilter::ExponentialFilter(double w): initialized(false), s_out_old(0), w(w)
{}


ExponentialFilter::~ExponentialFilter()
{}

		
void ExponentialFilter::setWeight(double w)
{
	this->w = w;
}


double ExponentialFilter::calculate(double s_in)
{
	double s_out = s_in;

	// if the filter is in ints first loop the output will be equal to the input.
	if (initialized)
	{
		// calculate smooth output signal
		s_out = w * s_in + (1-w) * s_out_old;
	}
	else
	{
		// use old filter values from now on
		initialized = true;
	}

	// save as old value for next iteration
	s_out_old = s_out;
	return s_out;
}


