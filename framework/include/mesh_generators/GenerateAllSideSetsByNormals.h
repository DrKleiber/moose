//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#ifndef GENERATEALLSIDESETSBYNORMALS_H
#define GENERATEALLSIDESETSBYNORMALS_H

#include "GenerateSideSetsBase.h"

// Forward declarations
class GenerateAllSideSetsByNormals;

template <>
InputParameters validParams<GenerateAllSideSetsByNormals>();

/**
 * This class will add sidesets to the entire mesh based on unique normals.
 * Note: This algorithm may not work well with meshes containing curved faces.
 * Several sidesets may be created in that case.  Use sensibly!
 */
class GenerateAllSideSetsByNormals : public GenerateSideSetsBase
{
public:
  GenerateAllSideSetsByNormals(const InputParameters & parameters);

  std::unique_ptr<MeshBase> generate();

protected:
  boundary_id_type getNextBoundaryID();

  /**
   * A pointer to the Mesh's boundary set, this datastructure will be modified
   * through this modifier.
   */
  std::set<boundary_id_type> _mesh_boundary_ids;

  std::unique_ptr<MeshBase> & _input;
};

#endif // GENERATEALLSIDESETSBYNORMALS_H
