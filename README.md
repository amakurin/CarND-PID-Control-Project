# CarND-Controls-PID
Self-Driving Car Engineer Nanodegree Program
---
The goals / steps of this project are the following:

* Apply PID control to steering and throttle of a simulated car.
* Tune by hand or optimize hyperparameters of PID controllers so that car could autonomously drive a lap without living the track. No tire may leave the drivable portion of the track surface. The car may not pop up onto ledges or roll over any surfaces that would otherwise be considered unsafe (if humans were in the vehicle).  

[//]: # (Image References)

[speed_cte]: ./result/speed_cte.png "CTE to Speed"
[twiddle]: ./result/twiddle.png "Twiddle result"


A proportional–integral–derivative controller (PID controller) continuously calculates an error value E as the difference between a desired setpoint and a measured value of process variable and applies a correction based on proportional, integral and derivative terms.

Un = \- Kp \* E(n) \- Ki \* Ei(n) \- Kd \* Ed(n)

where 

E(n) = value \- setpoint  is error between currently measured value and setpoint

Ei(n) - is the sum of the instantaneous error over time till current moment

Ed(n) = (E(n) - E(n-1))/dt - time derivative of error

Kp, Ki, Kd - hyperparameters of PID controller: proportional, integral and derivative gains respectively. 

Kp * E(n) is the proportional term that produce correction signal that is proportional to the current error value. If proportional gain Kp is too high, the system can become unstable. In contrast, a small gain results in a small corrections to a large errors.

Ki * Ei(n) is the integral term that accelerates the movement of the process towards setpoint and eliminates the residual steady-state error that occurs with a pure proportional term. However, since the integral term responds to accumulated errors from the past, it can cause the present value to overshoot the setpoint value and can cause auto-oscillations.

Kd * Ed(n) is the derivative term that predicts error behavior and сounteracts deviation from setpoint in the future. If Kd is too large, the controller can become less sensitive.

In this project two independent PID controllers are used to control steering angle and throttle, depending on cross track error (CTE). 

Here's the [link to video](./result/final.mp4) with one lap autonomous driving car.

## Gains selection

### Throttle 

Throttle control gains were selected by hand: Kp = 0.02, Ki = 0.0, Kd = 0.02. 

Speed setpoint is chosen to be depended on CTE, with quadratic dependecy:

![speed_cte][speed_cte]

speed_setpoint = min_speed + (max_speed - min_speed) * (CTE - CTE_LIMIT)ˆ2 / CTE_LIMITˆ2

where 

CTE_LIMIT - CTE threshold, all values above considered equal to this threshold 

min_speed - speed setpoint corresponding to CTE_LIMIT, 20mph

max_speed - speed setpoint corresponding to zero CTE, 100mph

### Steering angle

Gains optimization for steering PID is done by coordinate descent algorithm aka Twiddle.

Initial gains is chosen by hand to just keep car on track

Kp = 0.1, Ki = 0.0, Kd = 1.5  

Period of one iteration is chosen as 7600 updates, approximately 2 laps on average speed about 40mph.

PID drives car one iteration to collect total and maximum square errors. Then twiddle algorithm is applied to experiment with different deviations of each gain in isolation.

For each experiment total and maximum errors are computed. If car become unstable, stabilization (switching from unstable gains set to stablest set in the moment) is activated and experiment is concidered "bad idea". If car can drive whole iteration, then erorrs of this iteration compared with best in the moment and decision about changing the gain is made. 

Here is plot of total (left vertical) and maximum (right vertical) square errors as well as sum of changing steps.

![twiddle][twiddle]

As at seen on the plot total erorr converge pretty fast.

Final gains for steering PID, chosen after iteration #153:

Kp = 0.203692, Ki = 0.000233967, Kd = 5.12291 

Maximum speed reached by car on a straight segments of lap is about 62mph.

---

## Dependencies

* cmake >= 3.5
 * All OSes: [click here for installation instructions](https://cmake.org/install/)
* make >= 4.1
  * Linux: make is installed by default on most Linux distros
  * Mac: [install Xcode command line tools to get make](https://developer.apple.com/xcode/features/)
  * Windows: [Click here for installation instructions](http://gnuwin32.sourceforge.net/packages/make.htm)
* gcc/g++ >= 5.4
  * Linux: gcc / g++ is installed by default on most Linux distros
  * Mac: same deal as make - [install Xcode command line tools]((https://developer.apple.com/xcode/features/)
  * Windows: recommend using [MinGW](http://www.mingw.org/)
* [uWebSockets](https://github.com/uWebSockets/uWebSockets)
  * Run either `./install-mac.sh` or `./install-ubuntu.sh`.
  * If you install from source, checkout to commit `e94b6e1`, i.e.
    ```
    git clone https://github.com/uWebSockets/uWebSockets 
    cd uWebSockets
    git checkout e94b6e1
    ```
    Some function signatures have changed in v0.14.x. See [this PR](https://github.com/udacity/CarND-MPC-Project/pull/3) for more details.
* Simulator. You can download these from the [project intro page](https://github.com/udacity/self-driving-car-sim/releases) in the classroom.

There's an experimental patch for windows in this [PR](https://github.com/udacity/CarND-PID-Control-Project/pull/3)

## Basic Build Instructions

1. Clone this repo.
2. Make a build directory: `mkdir build && cd build`
3. Compile: `cmake .. && make`
4. Run it: `./pid`. 

## Editor Settings

We've purposefully kept editor configuration files out of this repo in order to
keep it as simple and environment agnostic as possible. However, we recommend
using the following settings:

* indent using spaces
* set tab width to 2 spaces (keeps the matrices in source code aligned)

## Code Style

Please (do your best to) stick to [Google's C++ style guide](https://google.github.io/styleguide/cppguide.html).

## Project Instructions and Rubric

Note: regardless of the changes you make, your project must be buildable using
cmake and make!

More information is only accessible by people who are already enrolled in Term 2
of CarND. If you are enrolled, see [the project page](https://classroom.udacity.com/nanodegrees/nd013/parts/40f38239-66b6-46ec-ae68-03afd8a601c8/modules/f1820894-8322-4bb3-81aa-b26b3c6dcbaf/lessons/e8235395-22dd-4b87-88e0-d108c5e5bbf4/concepts/6a4d8d42-6a04-4aa6-b284-1697c0fd6562)
for instructions and the project rubric.

## Hints!

* You don't have to follow this directory structure, but if you do, your work
  will span all of the .cpp files here. Keep an eye out for TODOs.

## Call for IDE Profiles Pull Requests

Help your fellow students!

We decided to create Makefiles with cmake to keep this project as platform
agnostic as possible. Similarly, we omitted IDE profiles in order to we ensure
that students don't feel pressured to use one IDE or another.

However! I'd love to help people get up and running with their IDEs of choice.
If you've created a profile for an IDE that you think other students would
appreciate, we'd love to have you add the requisite profile files and
instructions to ide_profiles/. For example if you wanted to add a VS Code
profile, you'd add:

* /ide_profiles/vscode/.vscode
* /ide_profiles/vscode/README.md

The README should explain what the profile does, how to take advantage of it,
and how to install it.

Frankly, I've never been involved in a project with multiple IDE profiles
before. I believe the best way to handle this would be to keep them out of the
repo root to avoid clutter. My expectation is that most profiles will include
instructions to copy files to a new location to get picked up by the IDE, but
that's just a guess.

One last note here: regardless of the IDE used, every submitted project must
still be compilable with cmake and make./
