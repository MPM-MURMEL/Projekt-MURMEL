#ifndef _EXPONENTIALFILTER_HPP_
#define _EXPONENTIALFILTER_HPP_


/**
* @brief A simple exponential Filter class
*
* A low pass filter based on the recusive exponential filter method
* 
*/
class ExponentialFilter
{
	
	private:
		bool initialized; /**< flag for usage of old values */

		double s_out_old; /**< last calculated smooth value */

		double w; /**< weight that controlls the amount of smoothing */

	
	public:
		/**
		* @brief Exponential filter constructor
		*
		* Creates an exponential filter.
		* This filter can be used to smooth incoming signals based on previous signal values. 
		*
		* @param w weight that controlls the amount of smoothing
		*/
		ExponentialFilter(double w);


		/**
		 * @brief Exponential filter no-args constructor
		 * 
		 * This constructor is provided as a default constructor, so that it can be invoked when a 
		 * different class object is created containing ExpoentialFilter objects.
		 */
		ExponentialFilter();


		/**
		* @brief Exponential filter constructor destructor
		*
		* Frees all exponential filter related memory.
		*/
		~ExponentialFilter();

		
		/**
		* @brief Set weight of exponential filter
		*
		* Sets the weight used to calculate smooth values from incoming signal values.
		* @param w weight that controlls the amount of smoothing
		*/
		void setWeight(double w);

		/**
		* @brief Calculates new smooth signal value
		*
		* Calculates the outgoing smooth signal value based on the previously calculated value.
		* s_out = w * s_in + (1 - w) * s_out_old
		*
		* @param s_in input signal
		* @return smooth output signal (s_out)
		*/
		double calculate(double s_in);


};

#endif
