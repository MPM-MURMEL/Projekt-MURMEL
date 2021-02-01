# Header files

The main part of the documentation takes place in the header files.
This why the source implementation can be changed independently and will not impair the documentation itself. 


```cpp
#ifndef _DEMOCLASS_HPP_
#define _DEMOCLASS_HPP_

/**
* Description of example that uses demo_class.
*
* @example <example-name>.cpp
*/

/**
* @brief A short description of demo_class
*
* A more detailed description of demo_class and space to elaborate on.
*/
public class demo_class
{
	private: 
	
	int demo_int; /**< Discription of demo_int */
	double demo_double; /**< Discription of demo_double */

	public:
	
	/**
	* @brief demo_class constructor
	*
	* Creates and initializes demo_class. 
	*
	* @param parameter1 Discription of parameter1 
	* @param parameter2 Discription of parameter2
	*/
	demo_class(int parameter1, double parameter2);

	/**
	* @brief demo_class destructor
	*
	* Frees all demo_class related memory.
	*/
	~demo_class();

	/**
	* @brief A brief discription foo
	*
	* A more detailed discription of foo.
	*
	* @param parameter Discription of parameter
	*
	* @return Discription of return value
	*/
	bool foo(int parameter);
	
}
```


# Source files

Within the source file all additional information can be added by comments.
These comments are not used by doxygen and therefore do not require a specific style.


```cpp
#include "demo_class.hpp"

demo_class::demo_class(int parameter1, double parameter2)
{
	// set demo_int to parameter1
	demo_int = parameter1;

	// set demo_double to sum of parameter1 and parameter2
 	demo_double =  parameter1 + parameter2;
}

demo_class::~demo_class()
{
	// 
}

bool demo_class::foo(int parameter)
{
	// return if parameter value is larger then zero
	return parameter>0;
}

```
