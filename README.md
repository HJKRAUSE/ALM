# Simple Asset-Liability Projection
* Header-only
* Requires <eigen/dense> for linear algebra

This library provides the functionality to create and optimize asset-liability projections across multiple scenarios.

## Useful, generic features
* Multithreading (TaskExecutor classes)
  * Wraps the threadpool in <windows.h>
  * Simplified API and usage
  
* Calendar dates
  * 1970-01-01 epoch
  * Based on Howard Hinnent's algorithms for serial number calculations

* Day counters and holiday calendars
  * Actual actual
  * Actual 365
  * Thirty 360
  
* Constrained linear/non-linear solvers
  * Brent's method
  * Gradient descent
  * Trust region (dogleg step)

# How to use

1. Include "ALM.h"
2. Set a reference date
3. Create yield curves to model
4. Create an asset portfolio
5. Create a liability portfolio
6. Set reinvestment and disinvestment strategies
7. Create a lambda wrapper around projection class that returns some metric
 * In main.cpp, this is the Bermuda reserve
8. Initialize a non-linear solver
9. Apply solver to lambda 
