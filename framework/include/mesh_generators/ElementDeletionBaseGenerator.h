//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#ifndef ELEMENTDELETIONBASEGENERATOR_H
#define ELEMENTDELETIONBASEGENERATOR_H

#include "MeshGenerator.h"

// Forward declarations
class ElementDeletionBaseGenerator;

template <>
InputParameters validParams<ElementDeletionBaseGenerator>();

/**
 * This class deletes elements from the mesh data structure
 * after it has been generated or read but before any FEM
 * data structures are initialized. Users are free to implement
 * their own derived classes by providing an implementation
 * for "shouldDelete".
 */
class ElementDeletionBaseGenerator : public MeshGenerator
{
public:
  ElementDeletionBaseGenerator(const InputParameters & parameters);

  std::unique_ptr<MeshBase> generate();

protected:
  std::unique_ptr<MeshBase> & _input;

  /**
   * Method that returns a Boolean indicating whether an
   * element should be removed from the mesh.
   */
  virtual bool shouldDelete(const Elem * elem) = 0;

  ///  Assign a boundary name to the cut surface?
  const bool _assign_boundary;

  /// Name of the boundary name to assign to the cut surface
  const BoundaryName _boundary_name;
};

#endif // ELEMENTDELETIONBASEGENERATOR_H
