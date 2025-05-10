/*
    MIT License

    Copyright (c) 2025 Harold James Krause

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.
*/

#pragma once

#include "RelinkableHandle.h"
#include "Portfolio.h"
#include "Strategy.h"
#include "Date.h"
#include "YieldCurve.h"

namespace ALM {

    /**
     * @brief Strategy that proportionally scales down all assets to cover a cash shortfall.
     *
     * If cash is negative, the strategy reduces each asset's volume by the same proportion
     * such that the total proceeds match the shortfall. If the shortfall is greater than the
     * total market value, all assets are liquidated.
     */
    class SellProRata : public Strategy {

    public:
        void apply(
            Portfolio& portfolio,
            double& cash,
            Date step_start,
            Date step_end,
            const std::shared_ptr<const YieldCurve>& curve) override
        {
            // No action needed if cash is positive
            if (cash >= 0.0)
                return;

            double need = -cash;
            double total_mv = portfolio.marketValue(curve, step_start);

            if (total_mv <= 0.0)
                return;

            double scalar = std::clamp(1.0 - (need / total_mv), 0.0, 1.0);

            for (auto& asset : portfolio.assets()) {
                double volume = asset.volume();
                asset.setVolume(volume * scalar);
            }

            // Adjust cash depending on whether the shortfall was fully met
            cash = (scalar == 0.0) ? cash + total_mv : 0.0;
        }
    };

}
