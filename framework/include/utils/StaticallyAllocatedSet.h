#ifndef STATICALLYALLOCATEDSET_H
#define STATICALLYALLOCATEDSET_H

#include "MooseError.h"

#include <array>

namespace MooseUtils
{

/**
 * Optimized Set
 *
 * For when you need a small sized, really fast set that is small and doesn't do any allocation
 *
 * *Small* is important here!  The search routine is _linear_!
 *
 * It is templated on the type the set will hold and the maximum size of the set (N)
 */
template <typename T, std::size_t N>
class StaticallyAllocatedSet
{
public:
  typedef typename std::vector<T>::iterator iterator;
  typedef typename std::vector<T>::const_iterator const_iterator;

  /**
   * Create a set
   */
  StaticallyAllocatedSet() : _end_pos(0) {}

  /**
   * Number of entries
   */
  std::size_t size() { return _end_pos; }

  /**
   * Empty?
   */
  bool empty() { return !_end_pos; }

  /**
   * Add a new entry
   */
  void insert(const T && value)
  {
    if (!contains(value))
    {
      mooseAssert(_end_pos < N, "Out of space in StaticallyAllocatedSet!");

      _data[_end_pos] = std::forward<const T>(value);
      _end_pos++;
    }
  }

  /**
   * Remove all entries
   *
   * Note: this does NOT at all free any entries
   */
  void clear() { _end_pos = 0; }

  /**
   * Iterator for the first entry
   */
  iterator begin() { return _data.begin(); }
  const_iterator begin() const { return _data.begin(); }

  /**
   * Iterator for the last entry
   */
  iterator end() { return _data.begin() + _end_pos; }
  const_iterator end() const { return _data.begin() + _end_pos; }

  /**
   * Whether or not the set contains the given item
   */
  bool contains(const T & value)
  {
    for (unsigned int i = 0; i < _end_pos; i++)
      if (_data[i] == value)
        return true;

    return false;
  }

  /**
   * Expert interface: the current ending position
   */
  size_t endPos() { return _end_pos; }

protected:
  std::array<T, N> _data;

  /// Save the ending as positions internally
  size_t _end_pos;
};

} // namespace MooseUtils

#endif
