/***************************************************************************
 *   Copyright (c) Jürgen Riegel          (juergen.riegel@web.de) 2002     *
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
#ifndef _PreComp_
# include <Python.h>
#endif

#include <CXX/Extensions.hxx>
#include <CXX/Objects.hxx>

#include <Mod/Part/App/TopoShapePy.h>
#include "ProjectionAlgos.h"
#include <Base/Console.h>
#include <Base/VectorPy.h>
#include <boost/regex.hpp>

#include <Mod/Part/App/OCCError.h>

using namespace std;
using Part::TopoShapePy;
using Part::TopoShape;

namespace Projection {

  /** Copies a Python dictionary of Python strings to a C++ container.
   * 
   * After the function call, the key-value pairs of the Python
   * dictionary are copied into the target buffer as C++ pairs
   * (pair<string, string>).
   *
   * @param sourceRange is a Python dictionary (Py::Dict). Both, the
   * keys and the values must be Python strings.
   *
   * @param targetIt refers to where the data should be inserted. Must
   * be of concept output iterator.
   */
  template<typename OutputIt>
  void copy(Py::Dict sourceRange, OutputIt targetIt)
  {
    string key;
    string value;

    for (auto keyPy : sourceRange.keys()) {
      key = Py::String(keyPy);
      value = Py::String(sourceRange[keyPy]);
      *targetIt = {key, value};
      ++targetIt;
    }
  }


class Module : public Py::ExtensionModule<Module>
{
public:
    Module() : Py::ExtensionModule<Module>("Projection")
    {
        add_keyword_method("projectToSVG2",&Module::projectToSVG2,
            "string = projectToSVG2(TopoShapes[, App.Vector origin, App.Vector direction, string type, float tolerance, list of dict vStyles, list of dict v0Style, dict v1Style, dict hStyle, dict h0Style, dict h1Style])\n"
            " -- Project a shape and return the SVG representation as string."
        );
        initialize("This module is the Drawing module."); // register with Python
    }

    virtual ~Module() {}

private:
    // virtual Py::Object invoke_method_varargs(void *method_def, const Py::Tuple &args)
    // {
    //     try {
    //         return Py::ExtensionModule<Module>::invoke_method_varargs(method_def, args);
    //     }
    //     catch (const Standard_Failure &e) {
    //         std::string str;
    //         Standard_CString msg = e.GetMessageString();
    //         str += typeid(e).name();
    //         str += " ";
    //         if (msg) {str += msg;}
    //         else     {str += "No OCCT Exception Message";}
    //         Base::Console().Error("%s\n", str.c_str());
    //         throw Py::Exception(Part::PartExceptionOCCError, str);
    //     }
    //     catch (const Base::Exception &e) {
    //         std::string str;
    //         str += "FreeCAD exception thrown (";
    //         str += e.what();
    //         str += ")";
    //         e.ReportException();
    //         throw Py::RuntimeError(str);
    //     }
    //     catch (const std::exception &e) {
    //         std::string str;
    //         str += "C++ exception thrown (";
    //         str += e.what();
    //         str += ")";
    //         Base::Console().Error("%s\n", str.c_str());
    //         throw Py::RuntimeError(str);
    //     }
    // }

    Py::Object projectToSVG2(const Py::Tuple& args, const Py::Dict& keys)
        {
	  static char* argNames[] = {"topoShape", "origin", "direction", "xDirection", "type", "tolerance", "vStyles", "v0Styles", "v1Style", "hStyle", "h0Style", "h1Style", NULL};
            PyObject *pcObjShapes = 0;
            PyObject *pcObjOrigin = 0;
            PyObject *pcObjDir = 0;
            PyObject *pcObjXDir = 0;
            const char *extractionTypePy = 0;
            ProjectionAlgos::ExtractionType extractionType = ProjectionAlgos::Plain;
            const float tol = 0.1f;
            PyObject* vStylesPy = 0;
            vector<ProjectionAlgos::XmlAttributes> vStyles;
            PyObject* v0StylesPy = 0;
            vector<ProjectionAlgos::XmlAttributes> v0Styles;
            PyObject* v1StylePy = 0;
            ProjectionAlgos::XmlAttributes v1Style;
            PyObject* hStylePy = 0;
            ProjectionAlgos::XmlAttributes hStyle;
            PyObject* h0StylePy = 0;
            ProjectionAlgos::XmlAttributes h0Style;
            PyObject* h1StylePy = 0;
            ProjectionAlgos::XmlAttributes h1Style;
        
            // Get the arguments

            if (!PyArg_ParseTupleAndKeywords(
                    args.ptr(), keys.ptr(), 
                    "O|O!O!O!sfOOOOOO", 
                    argNames,
                    &pcObjShapes,
                    &(Base::VectorPy::Type), &pcObjOrigin,
                    &(Base::VectorPy::Type), &pcObjDir,
                    &(Base::VectorPy::Type), &pcObjXDir,
                    &extractionTypePy, &tol, 
                    &vStylesPy, &v0StylesPy, &v1StylePy, 
                    &hStylePy, &h0StylePy, &h1StylePy))
          
                throw Py::Exception();

            // Convert all arguments into the right format

	    // Construct list of objects to export before making the Exporter, so
	    // we don't get empty exports if the list can't be constructed.
	    Py::Sequence list(pcObjShapes);
	    // if (list.length() == 0) {
	    //   return Py::None();
	    // }

	    // collect all object types that can be exported as mesh
	    std::vector<TopoDS_Shape> objectList;
	    for (auto it : list) {
	      PyObject *item = it.ptr();
	      if (PyObject_TypeCheck(item, &(TopoShapePy::Type))) {
                TopoShapePy* obj = static_cast<TopoShapePy *>(item);
                objectList.push_back(obj->getTopoShapePtr()->getShape());
	      }
	    }

	    if (objectList.empty()) {
	      throw Py::TypeError("No elements");
	    }

	    
	    

            //TopoShapePy* pShape = static_cast<TopoShapePy*>(pcObjShape);

            Base::Vector3d originVector(0,0,0);
            if (pcObjOrigin)
                originVector = static_cast<Base::VectorPy*>(pcObjOrigin)->value();
	    
            Base::Vector3d directionVector(0,0,1);
            if (pcObjDir)
                directionVector = static_cast<Base::VectorPy*>(pcObjDir)->value();

            Base::Vector3d xDirectionVector(1,0,0);
            if (pcObjXDir)
                xDirectionVector = static_cast<Base::VectorPy*>(pcObjXDir)->value();

            if (extractionTypePy && string(extractionTypePy) == "ShowHiddenLines")
                extractionType = ProjectionAlgos::WithHidden;

            if (vStylesPy) {
	      Py::Sequence styles(vStylesPy);
	      for (auto style : styles) {
		ProjectionAlgos::XmlAttributes vStyle;
                copy(Py::Dict(style), inserter(vStyle, vStyle.begin()));
		vStyles.push_back(vStyle);
	      }
	    }
            if (v0StylesPy) {
	      Py::Sequence styles(v0StylesPy);
	      for (auto style : styles) {
		ProjectionAlgos::XmlAttributes v0Style;
                copy(Py::Dict(style), inserter(v0Style, v0Style.begin()));
		v0Styles.push_back(v0Style);
	      }
	    }
            if (v1StylePy)
                copy(Py::Dict(v1StylePy), inserter(v1Style, v1Style.begin()));
            if (hStylePy)
                copy(Py::Dict(hStylePy), inserter(hStyle, hStyle.begin()));
            if (h0StylePy)
                copy(Py::Dict(h0StylePy), inserter(h0Style, h0Style.begin()));
            if (h1StylePy)
                copy(Py::Dict(h1StylePy), inserter(h1Style, h1Style.begin()));
        
            // Execute the SVG generation

            ProjectionAlgos Alg(objectList,
                                originVector, directionVector, xDirectionVector);

	    string shapestring("");
	    for (int i = 0; i < objectList.size(); i++) {
	      shapestring += Alg.getSVG2(i, extractionType, vStyles, v0Styles,
					 tol, v1Style, 
                                         hStyle, h0Style, h1Style);
	    }
	    
            Py::String result(shapestring);
            return result;
        }
  
};

PyObject* initModule()
{
    return (new Module)->module().ptr();
}

} // namespace Projection
