// SPDX-License-Identifier: LGPL-2.1-or-later

/***************************************************************************
 *   Copyright (c) 2024 Eric Price (CorvusCorax)                           *
 *                      <eric.price[at]tuebingen.mpg.de>                   *
 *                                                                         *
 *   This file is part of FreeCAD.                                         *
 *                                                                         *
 *   FreeCAD is free software: you can redistribute it and/or modify it    *
 *   under the terms of the GNU Lesser General Public License as           *
 *   published by the Free Software Foundation, either version 2.1 of the  *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   FreeCAD is distributed in the hope that it will be useful, but        *
 *   WITHOUT ANY WARRANTY; without even the implied warranty of            *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU      *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with FreeCAD. If not, see                               *
 *   <https://www.gnu.org/licenses/>.                                      *
 *                                                                         *
 **************************************************************************/

#include "PreCompiled.h"
#include <FuzzyHelper.h>

using namespace Part;

namespace {
    double BooleanFuzzy = 1.0;
}

double FuzzyHelper::getBooleanFuzzy() {
    return BooleanFuzzy;
}

void FuzzyHelper::setBooleanFuzzy(const double base) {
    BooleanFuzzy = base;
}

void FuzzyHelper::withBooleanFuzzy(double base, std::function<void()> func) {
    double oldValue = getBooleanFuzzy();
    setBooleanFuzzy(base);
    func();
    setBooleanFuzzy(oldValue);
}
