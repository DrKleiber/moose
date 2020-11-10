//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#pragma once

#include "MultiAppTransfer.h"

#include "ReporterData.h"

/*
 * This transfer serves as a base class for transferring reporter values between
 * multiapps. This includes reporters, vector postprocessors, and postprocessors.
 *
 * The undelying purpose fo this class is to avoid using non-const access to ReporterData
 * through FEProblemBase to set reporter data. Instead, we simply have this class
 * as a friend to ReporterData and provide these protected functions for derived
 * classes. This avoids any ol' object modifying reporter data.
 */
class MultiAppReporterTransferBase : public MultiAppTransfer
{
public:
  static InputParameters validParams();
  MultiAppReporterTransferBase(const InputParameters & parameters);

protected:
  /*
   * This function allows derived objects how the "from" reporters should be transferred.
   * I.e. whether we are transferring the entire data or part of it. Without calling this
   * early (constructor is preferred) there could be unintended behaiviour for
   * non-broadcasted data like VPPs.
   *
   * @param ReporterType data type of the reporter
   * @name name reporter name
   * @subapp_index index of the app holds the data (default is main application)
   * @time_index time index of the data (advanced use)
   */
  template <typename ReporterType>
  void addReporterTransferMode(const ReporterName & name,
                               const ReporterMode & mode,
                               unsigned int subapp_index = std::numeric_limits<unsigned int>::max(),
                               unsigned int time_index = 0);

  /*
   * Transferring reporter value to a multiapp
   *
   * @param from_reporter reporter name on main app
   * @param to_reporter reporter name on sub app
   * @param subapp_index the index of the sub app to transfer to
   * @param time_index time index of transfer (default is lastest data)
   */
  template <typename ReporterType>
  void transferToMultiApp(const ReporterName & from_reporter,
                          const ReporterName & to_reporter,
                          unsigned int subapp_index,
                          unsigned int time_index = 0);

  /*
   * Transferring reporter value from a multiapp
   *
   * @param from_reporter reporter name on sub app
   * @param to_reporter reporter name on main app
   * @param subapp_index the index of the sub app to transfer from
   * @param time_index time index of transfer (default is lastest data)
   */
  template <typename ReporterType>
  void transferFromMultiApp(const ReporterName & from_reporter,
                            const ReporterName & to_reporter,
                            unsigned int subapp_index,
                            unsigned int time_index = 0);
};

template <typename ReporterType>
void
MultiAppReporterTransferBase::addReporterTransferMode(const ReporterName & name,
                                                      const ReporterMode & mode,
                                                      unsigned int subapp_index,
                                                      unsigned int time_index)
{
  // For convenience
  FEProblemBase * problem;
  if (subapp_index == std::numeric_limits<unsigned int>::max())
    problem = &_multi_app->problemBase();
  else if (_multi_app->hasLocalApp(subapp_index))
    problem = &_multi_app->appProblemBase(subapp_index);
  else
    return;

  problem->getReporterData(ReporterData::WriteKey())
      .template getReporterValue<ReporterType>(name, this->name(), mode, time_index);
}

template <typename ReporterType>
void
MultiAppReporterTransferBase::transferToMultiApp(const ReporterName & from_reporter,
                                                 const ReporterName & to_reporter,
                                                 unsigned int subapp_index,
                                                 unsigned int time_index)
{
  if (!_multi_app->hasLocalApp(subapp_index))
    return;

  const ReporterType & value =
      _multi_app->problemBase().getReporterData().template getReporterValue<ReporterType>(
          from_reporter, time_index);

  _multi_app->appProblemBase(subapp_index)
      .getReporterData(ReporterData::WriteKey())
      .template setReporterValue<ReporterType>(to_reporter, value, time_index);
}

template <typename ReporterType>
void
MultiAppReporterTransferBase::transferFromMultiApp(const ReporterName & from_reporter,
                                                   const ReporterName & to_reporter,
                                                   unsigned int subapp_index,
                                                   unsigned int time_index)
{
  if (!_multi_app->hasLocalApp(subapp_index))
    return;

  const ReporterType & value =
      _multi_app->appProblemBase(subapp_index)
          .getReporterData()
          .template getReporterValue<ReporterType>(from_reporter, time_index);

  _multi_app->problemBase()
      .getReporterData(ReporterData::WriteKey())
      .template setReporterValue<ReporterType>(to_reporter, value, time_index);
}
