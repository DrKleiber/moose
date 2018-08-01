#ifndef RAY_H
#define RAY_H

// MOOSE Includes
#include "Moose.h"

// libMesh Includes
#include "libmesh/point.h"
#include "libmesh/parallel.h"
#include "libmesh/elem.h"
#include "libmesh/mesh_base.h"
#include "libmesh/stored_range.h"

// System Includes
#include <vector>

// Forward Declarations
class Ray;

namespace libMesh
{
namespace Parallel
{
template <>
class Packing<std::shared_ptr<Ray>>;
}
}

// Forward declaration
class Ray;

// Range for threaded execution
typedef StoredRange<std::vector<std::shared_ptr<Ray>>::iterator, std::shared_ptr<Ray>> RayRange;

/**
 * Basic datastructure for a ray that will traverse the mesh.
 */
class Ray
{
public:
  Ray() {}

  virtual ~Ray() {}

  Ray(const Point & start,
      const Point & end,
      unsigned int data_size,
      const Elem * starting_elem = NULL,
      unsigned int incoming_side = -1)
    : _data(data_size, 0),
      _start(start),
      _end(end),
      _starting_elem(starting_elem),
      _incoming_side(incoming_side)
  {
  }

  Ray(const Point & start,
      const Point & end,
      const std::vector<Real> & data,
      const Elem * starting_elem = NULL,
      unsigned int incoming_side = -1)
    : _data(data),
      _start(start),
      _end(end),
      _starting_elem(starting_elem),
      _incoming_side(incoming_side)
  {
  }

  bool operator==(const Ray & other);

  /**
   * Reset a ray with a given data size and constant data.
   * Useful for reusing a ray acquired from the SharedPool.
   */
  void reset(const Point & start,
             const Point & end,
             unsigned int data_size,
             Real constant_data = 0,
             const Elem * starting_elem = NULL,
             unsigned int incoming_side = -1)
  {
    _should_continue = true;
    _is_reverse = false;
    _ends_within_mesh = false;
    resetCounters();
    _data.resize(data_size);
    std::fill(_data.begin(), _data.end(), constant_data);
    _start = start;
    _end = end;
    _starting_elem = starting_elem;
    _incoming_side = incoming_side;
  }

  /**
   * Reset a ray with a given data vector.
   * Useful for reusing a ray acquired from the SharedPool.
   */
  void reset(const Point & start,
             const Point & end,
             const std::vector<Real> & data,
             const Elem * starting_elem = NULL,
             unsigned int incoming_side = -1)
  {
    _should_continue = true;
    _is_reverse = false;
    _ends_within_mesh = false;
    resetCounters();
    _data.resize(data.size());
    _data.assign(data.begin(), data.end());
    _start = start;
    _end = end;
    _starting_elem = starting_elem;
    _incoming_side = incoming_side;
  }

  /**
   * Reset any of the internal counters
   */
  void resetCounters()
  {
    _processor_crossings = 0;
    _intersections = 0;
    _distance = 0;
    _integrated_distance = 0;
  }

  void setID(unsigned int id) { _id = id; }
  unsigned int id() { return _id; }

  void setStart(const Point & start) { _start = start; }
  const Point & start() const { return _start; }

  void setEnd(const Point & end) { _end = end; }
  const Point & end() const { return _end; }

  std::vector<Real> & data() { return _data; }

  void setStartingElem(const Elem * starting_elem) { _starting_elem = starting_elem; }
  const Elem * startingElem() const { return _starting_elem; }

  void setEndsWithinMesh(bool ends_within_mesh = true) { _ends_within_mesh = ends_within_mesh; }
  bool endsWithinMesh() { return _ends_within_mesh; }

  void setIncomingSide(unsigned int incoming_side) { _incoming_side = incoming_side; }
  unsigned int incomingSide() const { return _incoming_side; }

  Real & processorCrossings() { return _processor_crossings; }

  Real & intersections() { return _intersections; }

  Real & distance() { return _distance; }

  Real & integratedDistance() { return _integrated_distance; }

  Real azimuthalAngle() { return _azimuthal_angle; }
  void setAzimuthalAngle(Real azimuthal_angle) { _azimuthal_angle = azimuthal_angle; }

  Real azimuthalSpacing() { return _azimuthal_spacing; }
  void setAzimuthalSpacing(Real azimuthal_spacing) { _azimuthal_spacing = azimuthal_spacing; }

  Real azimuthalWeight() { return _azimuthal_weight; }
  void setAzimuthalWeight(Real azimuthal_weight) { _azimuthal_weight = azimuthal_weight; }

  Real polarSpacing() { return _polar_spacing; }
  void setPolarSpacing(Real polar_spacing) { _polar_spacing = polar_spacing; }

  std::vector<Real> & polarSins() { return _polar_sins; }
  void setPolarSins(const std::vector<Real> & polar_sins) { _polar_sins = polar_sins; }

  std::vector<Real> & polarWeights() { return _polar_weights; }
  void setPolarWeights(const std::vector<Real> & polar_weights) { _polar_weights = polar_weights; }

  bool shouldContinue() { return _should_continue; }
  void setShouldContinue(bool should_continue) { _should_continue = should_continue; }

  bool isReverse() { return _is_reverse; }
  void setIsReverse(bool is_reverse) { _is_reverse = is_reverse; }

  /**
   * Creates a new Ray that is the reverse of this one
   */
  std::shared_ptr<Ray> reverse()
  {
    auto reverse_ray = std::make_shared<Ray>(_end, _start, _data.size());

    reverse_ray->_is_reverse = true;
    reverse_ray->_azimuthal_angle = _azimuthal_angle;
    reverse_ray->_azimuthal_spacing = _azimuthal_spacing;
    reverse_ray->_azimuthal_weight = _azimuthal_weight;
    reverse_ray->_polar_spacing = _polar_spacing;
    reverse_ray->_polar_sins = _polar_sins;
    reverse_ray->_polar_weights = _polar_weights;

    return reverse_ray;
  }

  /// The data that is carried with the Ray
  std::vector<Real> _data;

protected:
  /// A unique ID for this Ray.
  unsigned int _id;

  /// Start of the Ray
  Point _start;
  Point _end;

  /// The element the Ray begins in
  const Elem * _starting_elem;

  /// Whether or not the Ray ends within the Mesh, when this is false it means the ray ends on the boundary (or ends at a distance)
  bool _ends_within_mesh = false;

  /// The side of the the _starting element the ray is incoming on.  -1 if the Ray is starting _in_ the element
  unsigned int _incoming_side;

  /// Number of times this Ray has needed to be communicated
  Real _processor_crossings = 0;

  /// Number of intersections done for this Ray
  Real _intersections = 0;

  /// Total distance this Ray has traveled
  Real _distance = 0;

  /// Total distance this Ray has been integrated over
  Real _integrated_distance = 0;

  /// The azimuthal angle.  This is NOT sent in parallel because it's only used during generation!
  Real _azimuthal_angle = 0;

  /// The azimuthal spacing... just 1.0 for TRRM
  Real _azimuthal_spacing = 1.0;

  /// The azimuthal weight... just 1.0 for TRRM
  Real _azimuthal_weight = 1.0;

  /// The polar spacing... just 1.0 for TRRM
  Real _polar_spacing = 1.0;

  /// The sin of the polar angle for this Ray (just 1.0 for 3D)
  std::vector<Real> _polar_sins;

  /// The weight for each polar angle (just 1.0 for 3D)
  std::vector<Real> _polar_weights;

  /// True if this Ray was created as the reverse of another ray
  bool _is_reverse = false;

  /// Wether or not the Ray should continue to be traced
  bool _should_continue = true;

  // Extra padding to avoid false sharing
  long padding[8];

  friend class Parallel::Packing<std::shared_ptr<Ray>>;
};

/**
 * The following methods are specializations for using the Parallel::packed_range_* routines
 * for a vector of Rays
 */
namespace libMesh
{
namespace Parallel
{
template <>
class Packing<std::shared_ptr<Ray>>
{
public:
  typedef Real buffer_type;

  static unsigned int packed_size(typename std::vector<Real>::const_iterator in);

  static unsigned int packable_size(const std::shared_ptr<Ray> & ray, const void *);

  template <typename Iter>
  static void pack(const std::shared_ptr<Ray> & b, Iter data_out, const void *)
  {
    //    auto packing_start_time = std::chrono::steady_clock::now();

    // Storing the data size first makes it easy to verify and reserve space
    data_out = b->_data.size();

    // Storing the polar sins size
    data_out = b->_polar_sins.size();

    // Storing the polar weights size
    data_out = b->_polar_weights.size();

    // Start point
    data_out = b->_start(0);
    data_out = b->_start(1);
    data_out = b->_start(2);

    // End point
    data_out = b->_end(0);
    data_out = b->_end(1);
    data_out = b->_end(2);

    // Starting element
    data_out = b->startingElem()->id();

    // Ends within mesh
    data_out = b->endsWithinMesh();

    // Incoming side
    data_out = b->incomingSide();

    // Processor Crossings
    data_out = b->processorCrossings();

    // Intersections
    data_out = b->intersections();

    // Distance
    data_out = b->distance();

    // Integrated Distance
    data_out = b->integratedDistance();

    // Azimuthal Spacing
    data_out = b->azimuthalSpacing();

    // Azimuthal Weight
    data_out = b->azimuthalWeight();

    // Polar Spacing
    data_out = b->polarSpacing();

    // ID
    data_out = b->id();

    // is_reverse
    data_out = b->isReverse();

    // Should continue
    data_out = b->shouldContinue();

    // Copy out data
    std::copy(b->_data.begin(), b->_data.end(), data_out);

    // Copy out Polar Sins
    std::copy(b->_polar_sins.begin(), b->_polar_sins.end(), data_out);

    // Copy out Polar Weights
    std::copy(b->_polar_weights.begin(), b->_polar_weights.end(), data_out);

    //    global_packing_time += std::chrono::steady_clock::now() - packing_start_time;
  }

  static std::shared_ptr<Ray> unpack(typename std::vector<Real>::const_iterator in,
                                     void * ray_problem_ptr);
};

} // namespace Parallel

} // namespace libMesh

#endif
