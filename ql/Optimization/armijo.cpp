
/*
 Copyright (C) 2001, 2002, 2003 Nicolas Di C�sar�

 This file is part of QuantLib, a free-software/open-source library
 for financial quantitative analysts and developers - http://quantlib.org/

 QuantLib is free software: you can redistribute it and/or modify it under the
 terms of the QuantLib license.  You should have received a copy of the
 license along with this program; if not, please email quantlib-dev@lists.sf.net
 The license is also available online at http://quantlib.org/html/license.html

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the license for more details.
*/

/*! \file armijo.cpp
    \brief Armijo line-search class
*/

#include <ql/Optimization/armijo.hpp>

namespace QuantLib {

    double ArmijoLineSearch::operator()(
            const Problem& P,       // Optimization problem
            double t_ini)           // initial value of line-search step
    {
        Method& method = P.method();
        Constraint& constraint = P.constraint();

        bool maxIter = false;
        double q0 = method.functionValue();
        double qp0 = method.gradientNormValue();
        qt_ = q0;
        qpt_ = qp0;
        double qtold, t = t_ini;
        int loopNumber = 0;

        Array& x = method.x();
        Array& d = method.searchDirection();

        // Initialize gradient
        gradient_ = Array(x.size());
        // Compute new point
        xtd_ = x;
        t = update(xtd_, d, t, constraint);
        // Compute function value at the new point
        qt_ = P.value (xtd_);

        // Enter in the loop if the criterion is not satisfied
        if ((qt_-q0) > -alpha_*t*qpt_) {
            do {
                loopNumber++;
                // Decrease step
                t *= beta_;
                // Store old value of the function
                qtold = qt_;
                // New point value
                xtd_ = x;
                t = update(xtd_, d, t, constraint);

                // Compute function value at the new point
                qt_ = P.value (xtd_);
                P.gradient (gradient_, xtd_);
                // and it squared norm
                maxIter = P.method().endCriteria().
                    checkIterationNumber(loopNumber);
            } while (
                     (((qt_ - q0) > (-alpha_ * t * qpt_)) ||
                      ((qtold - q0) <= (-alpha_ * t * qpt_ / beta_))) &&
                     (!maxIter));
        }

        if (maxIter)
            succeed_ = false;

        // Compute new gradient
        P.gradient(gradient_, xtd_);
        // and it squared norm
        qpt_ = DotProduct(gradient_, gradient_);

        // Return new step value
        return t;
    }

}
