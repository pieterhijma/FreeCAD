/***************************************************************************
 *   Copyright (c) 2005 Jürgen Riegel <juergen.riegel@web.de>              *
 *                                                                         *
 *   This file is part of the FreeCAD CAx development system.              *
 *                                                                         *
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU Library General Public           *
 *   License as published by the Free Software Foundation; either          *
 *   version 2 of the License, or (at your option) any later version.      *
 *                                                                         *
 *   This library  is distributed in the hope that it will be useful,      *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU Library General Public License for more details.                  *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this library; see the file COPYING.LIB. If not,    *
 *   write to the Free Software Foundation, Inc., 59 Temple Place,         *
 *   Suite 330, Boston, MA  02111-1307, USA                                *
 *                                                                         *
 ***************************************************************************/

#include "PreCompiled.h"

#include <Base/Console.h>

#include "FeaturePartCurveNet.h"


using namespace Part;

PROPERTY_SOURCE(Part::CurveNet, Part::Feature)

CurveNet::CurveNet()
{
    ADD_PROPERTY(FileName,(""));
}

short CurveNet::mustExecute() const
{
    if (FileName.isTouched())
        return 1;
    return 0;
}

App::DocumentObjectExecReturn *CurveNet::execute()
{
    Base::FileInfo fi(FileName.getValue());
    if (!fi.isReadable()) {
        Base::Console().log("CurveNet::execute() not able to open %s!\n",FileName.getValue());
        std::string error = std::string("Cannot open file ") + FileName.getValue();
        return new App::DocumentObjectExecReturn(error);
    }

    TopoShape aShape;
    aShape.read(FileName.getValue());
    this->Shape.setValue(aShape);

    return App::DocumentObject::StdReturn;
}
