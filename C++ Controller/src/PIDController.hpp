#ifndef _PIDCONTROLLER_HPP_
#define _PIDCONTROLLER_HPP_


/**
* A simple main function to demonstrate the creation and usage of the PID Controller
* A set of samples will be used to illustrate the PID results.
*
* @example TEST_PIDController.cpp
*/

/**
* @brief A simple PID controller class
*
* A PID controller that only provides simple functions.
*/
class PIDController
{
	
	private:

		double e_sum; /**< the sum of all errors */
		double e_old; /**< the previous error */

		double Kp; /**< proportional gain */
		double KTi; /**< integral gain */
		double KTd; /**< derivation gain */

		bool calculation_method; /**< flag that defines the calculation method used (true gain base, false time based) */

	
	public:
		/**
		* @brief PID Constructor
		*
		* Creates a PID controller for universal usage
		* Depending on the controller mode (time base or gain based) the second and third parameter very in function.
		*
		* @param Kp proportional gain
		* @param KTi integral gain / integral time
		* @param KTd derivation gain / derivation time
		*/
		PIDController(double Kp, double KTi, double KTd);

		/**
		* @brief PID destructor
		*
		* Frees all PID related memory.
		*/
		~PIDController();

		/**
		* @brief Set time based calculation 
		*
		* Sets the calculation mode to time based.
		* u = Kp( e + 1/Ti (e_sum) + Td (e-e_old))
		*
		*/
		void setTimeMode();

		/**
		* @brief Set time based calculation 
		*
		* Sets the calculation mode to gain based.
		* u = Kp e + Ki (e_sum) + Kd (e-e_old)
		* Gain based by default
		*
		*/
		void setGainMode();

		/**
		* @brief Calculates controll unit value
		*
		* Calculates the controll value u depending on the error e.
		* Gain based: u = Kp e + Ki T (e_sum) + Kd/T (e-e_old)
		* Time based: u = Kp e + Ki (e_sum) + Kd (e-e_old)
		* Gain based by default
		*
		* @param e error value
		* @return controll value
		*/
		double calculate(double e);


};

#endif
