#include "twiddle.h"
#include <iostream>

Twiddle::Twiddle(PID &pid, vector<double> gain_diffs)
:pid(&pid), gain_diffs(gain_diffs) {
	best_gains = pid.gains;
	total_error = 0.0;
	max_error = 0.0;
	best_total_error = 0.0;
	best_max_error = 0.0;
	is_completed = false;
}

Twiddle::~Twiddle() {}
  
void Twiddle::Update(double cte) {
	if (!is_completed){
		// stabilization phase
		if (stabilize_counter > 0){
			--stabilize_counter;
			// if stabilization period ended - switch unstable gains back
			if (stabilize_counter ==0){
				switch_gains();
				cout<<"===========STABILIZATION COMPLETED============"<<endl<<endl;
			}
		}
		else{
			// on the end of each iteration
			if (update_counter % iteration_len == 0){
				// We can do as many iterations as we want before experiments to analize error statiscs.
				// Zero iteration is the first one, when we use this statiscs.
				if (current_iteration > -1){
					log_iteration();
					// if initial iteration - just update best errors and
					if (current_iteration == 0){
						best_total_error = total_error;
						best_max_error = max_error;
			            next_parameter();
			        } 
			        // if experimental iteration
			        else {
			        	// if better results
			        	if ((total_error < best_total_error)
			        		&& (max_error < (best_max_error * max_error_tolerance))){
							// update best errors
							best_total_error = total_error;
							best_max_error = max_error;
							best_gains = pid->gains;
							// increase parameter step
							gain_diffs[current_parameter] *= 1.1;
			                // switch to next parameter
			                next_parameter();
						}
						// if results not better and it is additional case
						else if (addition_case)
							// switch to substraction 
							addition_case = false;
						// if no improvement after addition and substraction experiments 
						else {
							// restore parameter
							pid->gains[current_parameter] += gain_diffs[current_parameter];
			                // decrease parameter step for further experiments
			                gain_diffs[current_parameter] *= 0.9;
			                // switch to next parameter
			                next_parameter();
						}
					}
					// if parameter steps are still big enough
					if (sum_diff() > tolerance){
						if (addition_case){
							// go with addition experiment
							pid->gains[current_parameter] += gain_diffs[current_parameter];
						}
						else{
							// go with substraction experiment
							pid->gains[current_parameter] -= 2*gain_diffs[current_parameter];
						}
					}
					// if parameters steps are small - algorithm completed
					else{
						cout<<"---------TOLERANCE LIMIT REACHED---------"<<endl;
						is_completed = true;
					}
					log_iteration(true);
				}
				// zero errors accumulators
				total_error = 0.0;
				max_error = 0.0;
				pid->i_error = 0.0;
				// go with next iteration
				++current_iteration;
			}
			// on each update calculate current errors 
			double err = cte*cte;
			total_error += err;
			if (err > max_error){
				max_error = err;
			} 
			log_report();
			// if iteration is not yet ended, but we got big errors
			if ((current_iteration > 0)
				&& ((max_error > (best_max_error * max_error_tolerance)) 
					|| (total_error > best_total_error))){
				cout<<"===========STABILIZATION STARTED==============="<<endl;
				update_counter = 0;
				// start stabilization period 
				stabilize_counter = stabilization_period;
				// by switching gains to most stable set, found at the moment
				switch_gains();
			}
			else	
				++update_counter;
		}
	}
}

void Twiddle::switch_gains(){
	vector<double> current_gains = pid->gains;
	pid->gains = best_gains;
	best_gains = current_gains;	
}

void Twiddle::next_parameter(){
	current_parameter = (current_parameter + 1) % gain_diffs.size();               
    addition_case = true;
}

double Twiddle::sum_diff(){
	double sum = 0.0;
	for (auto diff : gain_diffs)
		sum += diff;
	return sum;
}

void Twiddle::log_vector(string name, vector<double> &vector){
	cout<<name<<": ";
	for (double val : vector){
		cout<<val<<" ";
	}
	cout<<endl;	
}

void Twiddle::log_iteration(bool after_update){
	if (!after_update){
		cout<<"=========================="<<endl;
		cout<<"completing iteration "<<current_iteration<<endl;
		cout<<"total_error = "<<total_error<<endl;
		cout<<"best_total_error = "<<best_total_error<<endl;
		cout<<"max_error = "<<max_error<<endl;
		cout<<"best_max_error = "<<best_max_error<<endl;
		cout<<"current_parameter = "<<current_parameter<<endl;
		cout<<"addition_case = "<<addition_case<<endl;
		cout<<"--------------------------"<<endl;

		log_vector("Diffs", gain_diffs);
		log_vector("GAINS", pid->gains);
	}
	else {
		cout<<endl;
		cout<<"---------NEW VALS---------"<<endl;
		cout<<"best_total_error = "<<best_total_error<<endl;
		cout<<"best_max_error = "<<best_max_error<<endl;
		cout<<"current_parameter = "<<current_parameter<<endl;
		cout<<"addition_case = "<<addition_case<<endl;
		double sum = sum_diff();
		cout<<"sum_diff = "<<sum<<endl;
		log_vector("New Diffs", gain_diffs);
		log_vector("NEW GAINS", pid->gains);
		cout<<"=========================="<<endl<<endl;
	}
}

void Twiddle::log_report(){
	if(update_counter % report_period == 0){
		cout<<"---cnt = "<<update_counter<<endl;
		cout<<"---total err = "<<total_error<<endl;
		cout<<"---max err = "<<max_error<<endl;
		cout<<"---int err = "<<pid->i_error<<endl<<endl;
	}
}
