/****************************************************************/
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*          All contents are licensed under LGPL V2.1           */
/*             See LICENSE for full restrictions                */
/****************************************************************/
#ifndef SINGLEGRAINRIGIDBODYMOTION_H
#define SINGLEGRAINRIGIDBODYMOTION_H

#include "Kernel.h"

//Forward Declarations
class SingleGrainRigidBodyMotion;

template<>
InputParameters validParams<SingleGrainRigidBodyMotion>();

class SingleGrainRigidBodyMotion : public Kernel
{
public:
  SingleGrainRigidBodyMotion(const InputParameters & parameters);

protected:
  virtual Real computeQpResidual();
  virtual Real computeQpJacobian();
  virtual Real computeQpOffDiagJacobian(unsigned int jvar);
  virtual Real computeQpCJacobian();

  /// int label for the Concentration
  unsigned int _c_var;

  /// Variable value for the concentration
  VariableValue & _c;

  /// Variable gradient for the concentration
  VariableGradient & _grad_c;

  /// Grain number for the kernel to be applied
  unsigned int _op_index;

  /// Material property giving the advection velocity of grains
  const MaterialProperty<std::vector<RealGradient> > & _velocity_advection;

  /// Material property for divergence of advection velocities
  const MaterialProperty<std::vector<Real> > & _div_velocity_advection;

  /// Material property for  dervative of advection velocities
  const MaterialProperty<std::vector<RealGradient> > & _velocity_advection_derivative;

  /// Material property for dirivative of divergence of advection velocities
  const MaterialProperty<std::vector<Real> > & _div_velocity_advection_derivative;
};

#endif //SINGLEGRAINRIGIDBODYMOTION_H
