#include "PID.h"
#include <iostream>
using namespace std;

/*
* TODO: Complete the PID class.
*/

PID::PID(std::vector<double> gains, double min_correction, double max_correction)
	:gains(gains), min_correction(min_correction), max_correction(max_correction){
	d_error = 0.0;
	i_error = 0.0;
	p_error = 0.0;
	_initialized = false;
}

PID::~PID() {}

void PID::UpdateError(double cte) {
	if (_initialized){
		d_error = cte - p_error;
		i_error += cte;
		p_error = cte;
	} else {
		p_error = i_error = cte;
		_initialized = true;
	}
}

double PID::Correction() {
	double Kp = gains[0];
	double Ki = gains[1];
	double Kd = gains[2];

	double correction = - Kp*p_error - Ki*i_error - Kd*d_error;
	if (correction > max_correction)
		correction = max_correction;
	else if (correction < min_correction)
		correction = min_correction;

	return correction; 
}

