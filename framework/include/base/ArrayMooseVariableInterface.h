/****************************************************************/
/*               DO NOT MODIFY THIS HEADER                      */
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*           (c) 2010 Battelle Energy Alliance, LLC             */
/*                   ALL RIGHTS RESERVED                        */
/*                                                              */
/*          Prepared by Battelle Energy Alliance, LLC           */
/*            Under Contract No. DE-AC07-05ID14517              */
/*            With the U. S. Department of Energy               */
/*                                                              */
/*            See COPYRIGHT for full restrictions               */
/****************************************************************/

#ifndef ARRAYMOOSEVARIABLEINTERFACE_H
#define ARRAYMOOSEVARIABLEINTERFACE_H

#include "ArrayMooseVariable.h"
#include "InputParameters.h"
#include "MooseVariableInterfaceBase.h"

/**
 * Interface for objects that need to get values of MooseVariables
 */
class ArrayMooseVariableInterface : public MooseVariableInterfaceBase<ArrayMooseVariable, ArrayVariableValue, ArrayVariableGradient, ArrayVariableSecond>
{
public:
  /**
   * Constructing the object
   * @param parameters Parameters that come from constructing the object
   * @param nodal true if the variable is nodal
   * @param var_param_name the parameter name where we will find the coupled variable name
   */
  ArrayMooseVariableInterface(const MooseObject * moose_object, bool nodal, std::string var_param_name = "variable") :
      MooseVariableInterfaceBase(moose_object, nodal, var_param_name)
  {
  }
};


#endif /* ARRAYMOOSEVARIABLEINTERFACE_H */
