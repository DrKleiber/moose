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

#ifndef RAYPROBLEM_H
#define RAYPROBLEM_H

// Local Includes
#include "RaySystem.h"

// MOOSE Includes
#include "FEProblem.h"
#include "MooseObjectWarehouse.h"
#include "SharedPool.h"

// libMesh Includes
#include "libmesh/petsc_vector.h"

class RayProblem;

class RayTracingStudy;

template <>
InputParameters validParams<RayProblem>();

/**
 * FEProblem derived class for customization of callbacks. In this instance we only print out
 * something in the c-tor and d-tor, so we know the class was build and used properly.
 */
class RayProblemBase : public FEProblem
{
public:
  RayProblemBase(const InputParameters & params);
  virtual ~RayProblemBase() override;

  virtual void initialSetup() override;
  virtual void timestepSetup() override;

  virtual void solve() override;

  virtual bool converged() override;

  virtual void subdomainSetup(SubdomainID subdomain, THREAD_ID tid) override;

  bool rayKernelCoverageCheck() { return _ray_kernel_coverage_check; }

  bool rayMaterialCoverageCheck() { return _ray_material_coverage_check; }

  /**
   * Run subdomain setup on just the Ray systems
   */
  virtual void RaySubdomainSetup(SubdomainID subdomain, THREAD_ID tid);

  virtual void reinitElem(const Elem * elem, THREAD_ID tid) override;
  virtual void reinitElemPhys(const Elem * elem,
                              const std::vector<Point> & phys_points_in_elem,
                              THREAD_ID tid) override;

  virtual void reinitNeighbor(const Elem * elem, unsigned int side, THREAD_ID tid) override;

  /**
   * Run reinitElem() on just the Ray systems
   */
  virtual void RayReinitElem(const Elem * elem, THREAD_ID tid);

  virtual void prepareMaterials(SubdomainID blk_id, THREAD_ID tid) override;
  virtual void
  reinitMaterials(SubdomainID blk_id, THREAD_ID tid, bool swap_stateful = true) override;

  virtual std::vector<VariableName> getVariableNames() override;

  virtual bool hasVariable(const std::string & var_name) const override;

  virtual MooseVariableFEBase & getVariable(
      THREAD_ID tid,
      const std::string & var_name,
      Moose::VarKindType expected_var_type = Moose::VarKindType::VAR_ANY,
      Moose::VarFieldType expected_var_field_type = Moose::VarFieldType::VAR_FIELD_ANY) override;

  /**
   * Get the RaySystem for this Problem
   */
  virtual RaySystem & raySystem() { return *_ray_system; }

  /**
   * Called during the ray tracing process when the subdomain changes
   */
  void subdomainChanged(SubdomainID current_subdomain, THREAD_ID tid);

  /**
   * The number of energy groups currently being utilized.
   */
  unsigned long int numGroups() { return _num_groups; }

  /**
   * The number of polar angles currently being used
   */
  unsigned long int numPolar() { return _num_polar; }

  /**
   * Maximum length through the domain
   */
  Real domainMaxLength() { return _domain_max_length; }

  /**
   * Get the RayTracingStudy
   */
  template <typename T = RayTracingStudy>
  T & rayTracingStudy(THREAD_ID tid = 0)
  {
    const std::string & name = _pars.get<UserObjectName>("study");
    std::shared_ptr<T> ptr =
        std::dynamic_pointer_cast<T>(_all_user_objects.getActiveObject(name, tid));
    if (!ptr)
      mooseError("A RayTracingStudy study object of the given type does not exist.");
    return *ptr;
  }

  /**
   * Get the bounding box for the domain
   */
  const MeshTools::BoundingBox & boundingBox() { return _b_box; }

  /**
   * Get the outward facing normals for each element side
   */
  const std::map<const Elem *, std::vector<Point>> & elemNormals() { return _elem_normals; }

  /**
   * A pool of Rays to be reused
   */
  MooseUtils::SharedPool<Ray> _ray_pool;

protected:
  unsigned long int _num_groups;
  unsigned long int _num_polar;

  bool _solve_ray;

  bool _solve_fe;

  // Whether or not to check for RayKernel coverage
  bool _ray_kernel_coverage_check;

  // Whether or not to check for RayKernel coverage
  bool _ray_material_coverage_check;

  /// The System that holds all of data for Ray
  std::shared_ptr<RaySystem> _ray_system;

  Real _domain_max_length;

  /// Bounding box for the domain
  MeshTools::BoundingBox _b_box;

  /// Outward facing normals for each element side
  std::map<const Elem *, std::vector<Point>> _elem_normals;

  friend class RaySystem;
  friend class RayAuxSystem;
};

class RayProblem : public RayProblemBase
{
public:
  RayProblem(const InputParameters & params);
};

#endif /* RAYPROBLEM_H */
