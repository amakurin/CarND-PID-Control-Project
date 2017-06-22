#ifndef PID_H
#define PID_H
#include <vector>

class PID {
public:
  /*
  * Errors
  */
  double p_error;
  double i_error;
  double d_error;

  /*
  * Coefficients
  */ 
  std::vector<double> gains;

  /*
  * Correction limits
  */ 
 double min_correction;
  double max_correction;


  /*
  * Constructor
  */
  PID(std::vector<double> gains, double min_correction, double max_correction);

  /*
  * Destructor.
  */
  virtual ~PID();

  /*
  * Update the PID error variables given cross track error.
  */
  void UpdateError(double cte);

  /*
  * Calculate the total PID correction.
  */
  double Correction();

private:
  bool _initialized;
};

#endif /* PID_H */
