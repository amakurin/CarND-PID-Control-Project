#ifndef TWIDDLE_H
#define TWIDDLE_H

#include "PID.h"
#include <vector>
#include <string>
using namespace std;

class Twiddle {
public:
  // pid to tune
  PID *pid;
  // stablest parameter set
  vector<double> best_gains;
  // current parameters steps
  vector<double> gain_diffs;

  // errors of current iteration
  double total_error;
  double max_error;

  // with speed regulation we can allow some tolerance in max error 
  // so max_error over iteration should be less then best_max_error*max_error_tolerance
  double max_error_tolerance = 1.5;
  // log status each report_period update
  int report_period = 300;
  // how many updates used for stabilization
  int stabilization_period = 200;

  // best error values from experimental iterations
  double best_total_error;  
  double best_max_error;  

  // main counter of updates
  int update_counter = 0;
  // stabilization counter
  int stabilize_counter = 0;

  // sum(gain_diffs) < tolerance --> completed!
  float tolerance = 0.01;
  // how many updates considered one iteration
  int iteration_len = 7600;

  // iteration counter
  int current_iteration = -1;
  // parameter index
  int current_parameter = -1;
  // case switch (addition\substraction)
  bool addition_case = false;

  // Constructor
  // pid - PID to tune
  // gain_diffs - initial parameters steps
  Twiddle(PID &pid, vector<double> gain_diffs);

  // Destructor
  virtual ~Twiddle();

  // Main algorithm implementation
  void Update(double cte);

private:
  bool is_completed = false;
  // switches gains of training PID with best_gains 
  void switch_gains();

  // switches to addition case of next parameter in cycle (e.g. 0>1>2>0>1...)
  void next_parameter();
  
  // Calculates sum of current parameters steps gain_diffs
  double sum_diff();
  //some log routines to clean up algorithm code
  void log_iteration(bool after_update = false);
  void log_report();
  void log_vector(string name, vector<double> &vector);

};

#endif /* TWIDDLE_H */
