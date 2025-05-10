#pragma once

#include <Eigen/dense>
#define NOMINMAX // windows uses preprocessor min/maxes that blow up algorithm
#include <windows.h>

#include <vector>
#include <memory>
#include <string>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream>
#include <limits>
#include <functional>
#include <cctype>
#include <cmath>
#include <iomanip>

#include "UI.h"
#include "TaskExecutor.h"
#include "SingleThreadedExecutor.h"
#include "MultiThreadedExecutor.h"

#include "Date.h"
#include "DayCounter.h"
#include "Calendar.h"
#include "Schedule.h"

#include "YieldCurve.h"
#include "FlatForward.h"

#include "CashFlow.h"
#include "Asset.h"
#include "Portfolio.h"

#include "Strategy.h"
#include "RebalanceStrategy.h"
#include "BuyBonds.h"
#include "SellProRata.h"

#include "Projection.h"
#include "MultiScenarioProjection.h"
#include "StartingAssetSolver.h"

#include "Constraint.h"
#include "BoxConstraint.h"
#include "SolverXd.h"
#include "BrentSolver.h"
#include "ProjectedGradientSolver.h"
#include "TrustRegionSolver.h"