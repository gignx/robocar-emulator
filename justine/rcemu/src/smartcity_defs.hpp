#ifndef SMARTCITY_DEFS_HPP
#define SMARTCITY_DEFS_HPP

#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/interprocess/containers/map.hpp>
#include <boost/interprocess/containers/vector.hpp>
#include <boost/interprocess/containers/string.hpp>

namespace justine
{

namespace robocar
{
  typedef boost::interprocess::managed_shared_memory::segment_manager
          segment_manager_Type;
  typedef boost::interprocess::allocator<void, segment_manager_Type>
          void_allocator;
  typedef boost::interprocess::allocator<unsigned int, segment_manager_Type>
          uint_allocator;
  typedef boost::interprocess::vector<unsigned int, uint_allocator>
          uint_vector;
  typedef boost::interprocess::allocator<uint_vector, segment_manager_Type>
          uint_vector_allocator;
  typedef boost::interprocess::allocator<char, segment_manager_Type>
          char_allocator;
  typedef boost::interprocess::basic_string<char, std::char_traits<char>, char_allocator>
          char_string;

}

}

#endif
