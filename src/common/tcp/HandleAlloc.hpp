
#ifndef HANDLE_ALLOC_HPP
#define HANDLE_ALLOC_HPP

#include <boost/noncopyable.hpp>
#include <boost/aligned_storage.hpp>

// Class to manage the memory to be used for handler-based custom allocation.
// It contains a single block of memory which may be returned for allocation
// requests. If the memory is in use when an allocation request is made, the
// allocator delegates allocation to the global heap.
class HandleAlloc : private boost::noncopyable
{
 public:
  HandleAlloc() : in_use_(false) {}

  void* allocate(std::size_t size)
  {
    if (!in_use_ && size < storage_.size)
    {
      in_use_ = true;
      return storage_.address();
    }
    else
    {
      return ::operator new(size);
    }
  }

  void deallocate(void* pointer)
  {
    if (pointer == storage_.address())
    {
      in_use_ = false;
    }
    else
    {
      ::operator delete(pointer);
    }
  }

 private:
  boost::aligned_storage<1024> storage_;
  bool in_use_;
};

#endif
