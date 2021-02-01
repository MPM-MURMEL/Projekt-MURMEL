#include "PIDController.hpp"

PIDController::PIDController()
	: Kp(0), KTi(0), KTd(0), e_sum(0), e_old(0), calculation_method(true)
{}

PIDController::PIDController(double Kp, double KTi, double KTd)
: Kp(Kp), KTi(KTi), KTd(KTd), e_sum(0), e_old(0), calculation_method(true) {}

PIDController::~PIDController(){}

void PIDController::setTimeMode()
{
	calculation_method = false;
}

void PIDController::setGainMode()
{
	calculation_method = true;
}

double PIDController::calculate(double e)
{
	// add error to the error sum
	e_sum += e;

	// control value
	double u = e;

	// check which calculation method
	if(calculation_method)
	{
		u = Kp*e + KTi*e_sum + KTd*(e-e_old); 
	}
	else
	{
		u = Kp * ( e + 1/KTi *(e_sum) + KTd * (e-e_old));
	}
	

	// set old error for next calculation
	e_old = e;

	return u;
}
