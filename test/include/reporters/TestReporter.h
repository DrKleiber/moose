//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#pragma once

#include "GeneralReporter.h"

class TestDeclareReporter : public GeneralReporter
{
public:
  static InputParameters validParams();
  TestDeclareReporter(const InputParameters & parameters);
  virtual void initialize() override {}
  virtual void finalize() override {}
  virtual void execute() override;

protected:
  int & _int;
  Real & _real;
  std::vector<Real> & _vector;
  std::string & _string;
  Real & _bcast_value;
};

class TestGetReporter : public GeneralReporter
{
public:
  static InputParameters validParams();
  TestGetReporter(const InputParameters & parameters);
  virtual void initialize() override {}
  virtual void finalize() override {}
  virtual void execute() override;

protected:
  const int & _int;
  const int & _int_old;

  const Real & _real;
  const std::vector<Real> & _vector;
  const std::string & _string;
  const Real & _bcast_value;
};
