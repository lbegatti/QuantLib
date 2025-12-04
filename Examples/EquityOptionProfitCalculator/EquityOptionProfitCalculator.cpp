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
        Real underlying = 290;
        Real strike = 330;
        Real boughtOptionPrice = 0.60;
        Real currentOptionPrice = 3.30;
        Spread dividendYield = 0.00;
        Rate riskFreeRate = 0.036;
        Volatility volatility = 0.20;
        Date maturity(20, Mar, 2026);
        DayCounter dayCounter = Actual365Fixed();

        std::cout << "Option type = "  << type << std::endl;
        std::cout << "Maturity = "        << maturity << std::endl;
        std::cout << "Underlying price = "        << underlying << std::endl;
        std::cout << "Strike = "                  << strike << std::endl;
        std::cout << "Option purchase price = " << boughtOptionPrice << std::endl;
        std::cout << "Current option price = " << currentOptionPrice << std::endl;
        std::cout << "Option price change = " << io::rate(((currentOptionPrice/boughtOptionPrice)-1)) << std::endl;
        std::cout << "Risk-free interest rate = " << io::rate(riskFreeRate) << std::endl;
        std::cout << "Dividend yield = " << io::rate(dividendYield) << std::endl;
        std::cout << "Volatility = " << io::volatility(volatility) << std::endl;
        std::cout << std::endl;
        std::cout << std::endl;

        Size widths[] = { 35, 14, 14 };
        std::cout << std::setw(widths[0]) << std::left << "Method"
                  << std::setw(widths[1]) << std::left << "European"
                  << std::setw(widths[2]) << std::left << "American"
                  << std::endl;

        /*********************************************
         * CALCULATING THE THEORETICAL OPTION PROFIT *
         * the objective of the code is to calculate, given some inputs, the value of the option
         * considering that you have already bought it for x and the price is now increased or
         * decreased, especially to have a rough estimate of the potential future value iff the
         * underlying hits the strike.

         * All in all, this is the question we are trying to answer "How much is it theoretically
           worth as of today, my call option if/when it hits the strike price before expiry?"
         *********************************************/

        // European exercise - Black & Scholes

        // America exercise - Binomial Cox-Ross-Rubinstein

    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "unknown error" << std::endl;
        return 1;
    }


}