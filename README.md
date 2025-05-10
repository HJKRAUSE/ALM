# Simple Asset-Liability Projection

1. Requires Eigen library for dense linear algebra
   
3. Projects a portfolio of assets and liabilities and applies reinvestment and disinvestment
   * Fixed and zero coupon bond cash flow generation is supported
   * Assets are treated as a cash flow stream - custom assets are possible
   * Explicit liability calculations are not supported (should be proxied with zero coupon bonds)

4. Bermuda-style multiple scenario projections are supported
   * Multi-threaded
   * Solves starting asset balance such that ending surplus == 0
  
5. Optimization layer solves for the optimal asset portfolio needed to mature liabilities
   
6. Optional UI functionality included
   * Not used outside of the final optimization layer to reduce message verbosity

# How to use

1. Set a reference date
2. Create yield curves to model
3. Create an asset portfolio
4. Create a liability portfolio
5. Set reinvestment and disinvestment strategies
6. Create a lambda wrapper around projection class that returns some metric
   * In main.cpp, this is the Bermuda reserve
8. Initialize a non-linear solver
9. Apply solver to lambda 
