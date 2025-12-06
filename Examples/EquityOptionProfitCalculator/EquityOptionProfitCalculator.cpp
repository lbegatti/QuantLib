//
// Created by Luca Begatti on 03/12/2025.
//

/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*!
 Copyright (C) 2005, 2006, 2007, 2009 StatPro Italia srl

 This file is part of QuantLib, a free-software/open-source library
 for financial quantitative analysts and developers - http://quantlib.org/

 QuantLib is free software: you can redistribute it and/or modify it
 under the terms of the QuantLib license.  You should have received a
 copy of the license along with this program; if not, please email
 <quantlib-dev@lists.sf.net>. The license is also available online at
 <https://www.quantlib.org/license.shtml>.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the license for more details.
*/

#include <ql/qldefines.hpp>
#if !defined(BOOST_ALL_NO_LIB) && defined(BOOST_MSVC)
#  include <ql/auto_link.hpp>
#endif
#include <ql/instruments/vanillaoption.hpp>
#include <ql/math/integrals/tanhsinhintegral.hpp>
#include <ql/pricingengines/vanilla/analyticeuropeanengine.hpp>
#include <ql/pricingengines/vanilla/analyticeuropeanvasicekengine.hpp>
#include <ql/pricingengines/vanilla/analytichestonengine.hpp>
#include <ql/pricingengines/vanilla/baroneadesiwhaleyengine.hpp>
#include <ql/pricingengines/vanilla/batesengine.hpp>
#include <ql/pricingengines/vanilla/binomialengine.hpp>
#include <ql/pricingengines/vanilla/bjerksundstenslandengine.hpp>
#include <ql/pricingengines/vanilla/fdblackscholesvanillaengine.hpp>
#include <ql/pricingengines/vanilla/integralengine.hpp>
#include <ql/pricingengines/vanilla/mcamericanengine.hpp>
#include <ql/pricingengines/vanilla/mceuropeanengine.hpp>
#include <ql/pricingengines/vanilla/qdfpamericanengine.hpp>
#include <ql/time/calendars/target.hpp>
#include <ql/utilities/dataformatters.hpp>

#include <iostream>
#include <iomanip>

using namespace QuantLib;

int main(int, char* []) {
    try {
        std::cout << std::endl;

        // set up dates
        Calendar calendar = TARGET();
        Date todaysDate(3, Dec, 2025);
        Date optionPurchaseDate(15,Oct,2025);
        Date settlementDate(16,Oct,2025);
        Settings::instance().evaluationDate() = todaysDate;

        // option details
        Option::Type type(Option::Call);
        Integer contractSize = 100;
        Real underlying = 290;
        Real strike = 330;
        Real purchasePremium = 0.60;
        Real currentOptionPremium = 3.30;
        Spread dividendYield = 0.00;
        Rate riskFreeRate = 0.036;
        Volatility volatility = 0.20;
        Date maturity(20, Mar, 2026);
        DayCounter dayCounter = Actual365Fixed();

        std::cout << "Option type = "  << type << std::endl;
        std::cout << "Maturity = "        << maturity << std::endl;
        std::cout << "Underlying price = "        << underlying << std::endl;
        std::cout << "Strike = "                  << strike << std::endl;
        std::cout << "Option purchase price = " << purchasePremium << std::endl;
        std::cout << "Current option price = " << currentOptionPremium << std::endl;
        std::cout << "Option price change = " << io::rate(((currentOptionPremium/purchasePremium)-1)) << std::endl;
        std::cout << "Risk-free interest rate = " << io::rate(riskFreeRate) << std::endl;
        std::cout << "Dividend yield = " << io::rate(dividendYield) << std::endl;
        std::cout << "Volatility = " << io::volatility(volatility) << std::endl;
        std::cout << std::endl;
        std::string method;
        std::cout << std::endl;



        /*********************************************
         * CALCULATING THE THEORETICAL OPTION PROFIT *
         * the objective of the code is to calculate, given some inputs, the value of the option
         * considering that you have already bought it for x and the price is now increased or
         * decreased, especially to have a rough estimate of the potential future value iff the
         * underlying hits the strike.

         * All in all, this is the question we are trying to answer "How much is it theoretically
           worth as of today, my call option if/when it hits the strike price before expiry?"
         *********************************************/

        std::vector<Date> exerciseDates;
        for (Integer i =1; i<=4; i++)
            exerciseDates.push_back(settlementDate + 3*i*Months);
        auto europeanExercise = ext::make_shared<EuropeanExercise>(maturity);
        auto americanExercise = ext::make_shared<AmericanExercise>(settlementDate, maturity);
        auto underlyingH = makeQuoteHandle(underlying);

        // bootstrap the yield/dividend/vol curves
        Handle<YieldTermStructure> flatTermStructure(
            ext::make_shared<FlatForward>(settlementDate, riskFreeRate, dayCounter));
        Handle<YieldTermStructure> flatDividendTS(
            ext::make_shared<FlatForward>(settlementDate, dividendYield, dayCounter));
        Handle<BlackVolTermStructure> flatVolTS(
            ext::make_shared<BlackConstantVol>(settlementDate, calendar, volatility,
                dayCounter));
        auto payoff = ext::make_shared<PlainVanillaPayoff>(type, strike);
        auto bsmProcess = ext::make_shared<BlackScholesMertonProcess>(
                underlyingH, flatDividendTS, flatTermStructure, flatVolTS);

        //options
        VanillaOption europeanOption(payoff, europeanExercise);
        VanillaOption americanOption(payoff, americanExercise);

        std::cout << "-------------EUROPEAN OPTION PRICING (PROXY METHOD)---------------------------" << std::endl << std::endl;

        // European exercise - Black & Scholes -- Profit calculation
        // the first step calculates the ROI on one unit of the option.
        // option premium paid is 0.60$ and this is compared to the NPV calculated via Black-Scholes
        // formula and its parameters/assumptions.
        Size widths[] = { 35, 14, 14, 14};
        std::cout << std::setw(widths[0]) << std::left << "Method"
                  << std::fixed
                  << std::setw(widths[1]) << std::left << "European ($)"
                  << std::setw(widths[2]) << std::left << "ROI (%)"
                  << std::setw(widths[3]) << std::left << "Potential Value ($)"
                  << std::endl;

        method = "Black-Scholes";
        europeanOption.setPricingEngine(ext::make_shared<AnalyticEuropeanEngine>(bsmProcess));
        Real currentTheoreticalValue = europeanOption.NPV();
        Real profit = currentTheoreticalValue - purchasePremium;
        Real profitPercentEu = (profit / purchasePremium) * 100.0;

        // Now we want to calculate the potential value of the option, with the strike being the
        // current underlying price of 290$, always expiring in March 2026, i.e. no intrinsic value
        // but max extrinsic value. Essentially, we would like to know what a March 2026 call is
        // worth when the stock matches the strike. You can use a trading platform to have a second
        // estimate - Proxy Method - upper bound (time decay)

        auto payoffProxyMethod = ext::make_shared<PlainVanillaPayoff>(type, underlying);
        VanillaOption europeanOptionProxyMethod(payoffProxyMethod, europeanExercise);
        europeanOptionProxyMethod.setPricingEngine(ext::make_shared<AnalyticEuropeanEngine>(bsmProcess));

        std::cout << std::setw(widths[0]) << std::left << method
                  << std::fixed
                  << std::setw(widths[1]) << std::left << currentTheoreticalValue
                  << std::setw(widths[2]) << std::left << profitPercentEu
                  << std::setw(widths[3]) << std::left << europeanOptionProxyMethod.NPV()*contractSize
                  << std::endl << std::endl;

        std::cout << "-------------EUROPEAN OPTION PRICING (PROFIT CALCULATOR)----------------------" << std::endl << std::endl;

        // European Option profit calculator (more precise, sticky strike)
        VanillaOption europeanOptionProfitCalculatorMethod(payoff,europeanExercise);
        Volatility impliedVol = europeanOptionProfitCalculatorMethod.impliedVolatility(purchasePremium, bsmProcess);
        std::cout << "Implied Volatility at purchase price " << purchasePremium << "$ : "
                  << io::volatility(impliedVol) << std::endl << std::endl;
        // Price the option in the future scenario, i.e. we assume that for instance in 2 months
        // the underlying hits the strike price of 330$, and we want to know what is the value at
        // that future point in time.
        Date futureDate = todaysDate + Period(1, Weeks);
        Real futureUnderlying = strike; // we assume it hits the strike

        // Yield, dividend, vol term structures are rebuilt at future date
        Settings::instance().evaluationDate() = futureDate;
        auto futureUnderlyingH = makeQuoteHandle(futureUnderlying);
        Handle<YieldTermStructure> futureflatTermStructure(
            ext::make_shared<FlatForward>(futureDate, riskFreeRate, dayCounter));
        Handle<YieldTermStructure> futureflatDividendTS(
            ext::make_shared<FlatForward>(futureDate, dividendYield, dayCounter));
        Handle<BlackVolTermStructure> futureflatVolTS(
            ext::make_shared<BlackConstantVol>(futureDate, calendar, impliedVol,
                dayCounter));
        // new BSM process at future date
        auto futureBsmProcess = ext::make_shared<BlackScholesMertonProcess>(
                futureUnderlyingH, futureflatDividendTS, futureflatTermStructure, futureflatVolTS);
        europeanOptionProfitCalculatorMethod.setPricingEngine(ext::make_shared<AnalyticEuropeanEngine>(futureBsmProcess));

        // Real future value
        Real futureValueEuropean = europeanOptionProfitCalculatorMethod.NPV();
        std::cout << "\n--- FUTURE SCENARIO ---" << std::endl;
        std::cout << "Future Date: " << futureDate << std::endl;
        std::cout << "Target Spot: " << futureUnderlying << std::endl;
        std::cout << "Est. Option Value: $" << futureValueEuropean << std::endl << std::endl;
        std::cout << "Total future value of the option contract: " << futureValueEuropean * contractSize << '$' << std::endl << std::endl;

        std::cout << "--------------------AMERICAN OPTION PRICING (PROXY METHOD)--------------------" << std::endl << std::endl;
        // American exercise - Binomial Cox-Ross-Rubinstein
        method = "Binomial Cox-Ross-Rubinstein";
        Size timeSteps = 801;
        auto crrEngine = ext::make_shared<BinomialVanillaEngine<CoxRossRubinstein>>(bsmProcess, timeSteps);
        americanOption.setPricingEngine(crrEngine);
        Real americanCurrentTheoreticalValue = americanOption.NPV();
        Real americanProfit = americanCurrentTheoreticalValue - purchasePremium;
        Real americanProfitPercentEu = (americanProfit / purchasePremium) * 100.0;
        VanillaOption americanOptionProxyMethod(payoffProxyMethod, americanExercise);
        americanOptionProxyMethod.setPricingEngine(crrEngine);

        std::cout << std::setw(widths[0]) << std::left << "Method"
                  << std::fixed
                  << std::setw(widths[1]) << std::left << "American ($)"
                  << std::setw(widths[2]) << std::left << "ROI (%)"
                  << std::setw(widths[3]) << std::left << "Potential Value ($)"
                  << std::endl;
        std::cout << std::setw(widths[0]) << std::left << method
                  << std::fixed
                  << std::setw(widths[1]) << std::left << americanCurrentTheoreticalValue
                  << std::setw(widths[2]) << std::left << americanProfitPercentEu
                  << std::setw(widths[3]) << std::left << americanOptionProxyMethod.NPV()*contractSize
                  << std::endl;
        return 0;

    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "unknown error" << std::endl;
        return 1;
    }


}