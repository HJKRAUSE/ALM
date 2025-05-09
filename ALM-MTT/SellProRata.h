#pragma once

#include "RelinkableHandle.h"
#include "Portfolio.h"
#include "Strategy.h"
#include "Date.h"
#include "Curve.h"

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
            const std::shared_ptr<const Curve>& curve,
            const std::shared_ptr<TaskExecutor>& executor) override
        {
            // No action needed if cash is positive
            if (cash >= 0.0)
                return;

            double need = -cash;
            double total_mv = portfolio.marketValue(curve, step_start, executor);

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
