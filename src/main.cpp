#include <uWS/uWS.h>
#include <iostream>
#include "json.hpp"
#include "PID.h"
#include "twiddle.h"
#include <math.h>

// for convenience
using json = nlohmann::json;

// For converting back and forth between radians and degrees.
constexpr double pi() { return M_PI; }
double deg2rad(double x) { return x * pi() / 180; }
double rad2deg(double x) { return x * 180 / pi(); }

// Checks if the SocketIO event has JSON data.
// If there is data the JSON object in string format will be returned,
// else the empty string "" will be returned.
std::string hasData(std::string s) {
  auto found_null = s.find("null");
  auto b1 = s.find_first_of("[");
  auto b2 = s.find_last_of("]");
  if (found_null != std::string::npos) {
    return "";
  }
  else if (b1 != std::string::npos && b2 != std::string::npos) {
    return s.substr(b1, b2 - b1 + 1);
  }
  return "";
}

int main()
{
  uWS::Hub h;
  // if true, will use widdle to tune steering PID parameters 
  bool twiddle_steering = false;

  // if true, will use PID to control speed
  bool control_speed = true;
  
  // maximum speed - used as speed setpoint when cte is 0
  double max_speed = 100.0;
  // minimum speed - used as speed setpoint when cte is maximum
  double min_speed = 20.0;
  // all values of cte greater are concidered maximum in speed setpoint calculations
  double cte_limit = 3.0;  

  // Steering PID "optimal" parameters found by twiddling with speed regulation 
  auto steering_gains = {0.203692, 0.000233967, 5.12291};
  
  if (twiddle_steering){
    // Steering PID initial nearly random parameters found by hand with speed regulation 
    steering_gains = {0.1, 0.0, 1.5};  
  }
  // Steering PID 
  PID steering_pid(steering_gains, -1.0, 1.0);  

  // Speed PID with parameters found by hand 
  PID speed_pid({0.02, 0.0, 0.02}, -10.0, 10.0); 
  
  // Twiddle for steering PID tuning
  Twiddle twiddle(steering_pid, {0.04,0.0001,0.5});

  if (!control_speed && twiddle_steering){
    // need more careful hand tuning
    steering_pid.gains = {0.1, 0.0, 4.0};   
    // need lesser steps
    twiddle.gain_diffs = {0.02, 0.00001, 0.5};   
    // less tollerant to max deviations
    twiddle.max_error_tolerance = 1.1;
    // need more time to stabilize
    twiddle.stabilization_period = 500;
  }

  h.onMessage([&steering_pid, &twiddle, &speed_pid, twiddle_steering, control_speed, max_speed, min_speed, cte_limit](uWS::WebSocket<uWS::SERVER> ws, char *data, size_t length, uWS::OpCode opCode) {
    // "42" at the start of the message means there's a websocket message event.
    // The 4 signifies a websocket message
    // The 2 signifies a websocket event
    if (length && length > 2 && data[0] == '4' && data[1] == '2')
    {
      auto s = hasData(std::string(data).substr(0, length));
      if (s != "") {
        auto j = json::parse(s);
        std::string event = j[0].get<std::string>();
        if (event == "telemetry") {
          // j[1] is the data JSON object
          double cte = std::stod(j[1]["cte"].get<std::string>());
          double speed = std::stod(j[1]["speed"].get<std::string>());
          //double angle = std::stod(j[1]["steering_angle"].get<std::string>());
          /*
          * TODO: Calcuate steering value here, remember the steering value is
          * [-1, 1].
          * NOTE: Feel free to play around with the throttle and speed. Maybe use
          * another PID controller to control the speed!
          */
          if (twiddle_steering){
            twiddle.Update(cte);
          }
          steering_pid.UpdateError(cte);
          double steer_value = steering_pid.Correction();

          // use default throttle if no speed control
          double throttle_value = 0.5;
          // otherwise
          if (control_speed){
            // get absolute error
            double err = fabs(cte);
            if (err > cte_limit){
              err = cte_limit;
            }
            
            // linear 
            //double speed_setpoint = max_speed + (min_speed-max_speed) * err/cte_limit;
            
            // quadratic 
            double arg =  err - cte_limit;
            double speed_setpoint = min_speed + (max_speed-min_speed) * arg * arg/(cte_limit*cte_limit);
            
            double speed_err = speed - speed_setpoint;
            speed_pid.UpdateError(speed_err);
            throttle_value = speed_pid.Correction();
          }

          // DEBUG
          if (!twiddle_steering)
            std::cout << "CTE: " << cte << " speed: " << speed << std::endl
              << " throttle_value: " << throttle_value 
              << " steer_value: " << steer_value << std::endl;

          json msgJson;
          msgJson["steering_angle"] = steer_value;
          msgJson["throttle"] = throttle_value;
          auto msg = "42[\"steer\"," + msgJson.dump() + "]";
          //std::cout << msg << std::endl;
          ws.send(msg.data(), msg.length(), uWS::OpCode::TEXT);
        }
      } else {
        // Manual driving
        std::string msg = "42[\"manual\",{}]";
        ws.send(msg.data(), msg.length(), uWS::OpCode::TEXT);
      }
    }
  });

  // We don't need this since we're not using HTTP but if it's removed the program
  // doesn't compile :-(
  h.onHttpRequest([](uWS::HttpResponse *res, uWS::HttpRequest req, char *data, size_t, size_t) {
    const std::string s = "<h1>Hello world!</h1>";
    if (req.getUrl().valueLength == 1)
    {
      res->end(s.data(), s.length());
    }
    else
    {
      // i guess this should be done more gracefully?
      res->end(nullptr, 0);
    }
  });

  h.onConnection([&h](uWS::WebSocket<uWS::SERVER> ws, uWS::HttpRequest req) {
    std::cout << "Connected!!!" << std::endl;
  });

  h.onDisconnection([&h](uWS::WebSocket<uWS::SERVER> ws, int code, char *message, size_t length) {
    ws.close();
    std::cout << "Disconnected" << std::endl;
  });

  int port = 4567;
  if (h.listen(port))
  {
    std::cout << "Listening to port " << port << std::endl;
  }
  else
  {
    std::cerr << "Failed to listen to port" << std::endl;
    return -1;
  }
  h.run();
}
