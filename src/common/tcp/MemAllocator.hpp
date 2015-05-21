
#ifndef MEM_ALLOCATOR_HPP
#define MEM_ALLOCATOR_HPP

#include "HandleAlloc.hpp"

// Wrapper class template for handler objects to allow handler memory
// allocation to be customised. Calls to operator() are forwarded to the
// encapsulated handler.
template <typename Handler>
class MemAllocator
{
 public:
  MemAllocator(HandleAlloc& a, Handler h) : allocator_(a), handler_(h) {}

  template <typename Arg1>
  void operator()(Arg1 arg1)
  {
    handler_(arg1);
  }

  template <typename Arg1, typename Arg2>
  void operator()(Arg1 arg1, Arg2 arg2)
  {
    handler_(arg1, arg2);
  }

  friend void* asio_handler_allocate(std::size_t size,
                                     MemAllocator<Handler>* this_handler)
  {
    return this_handler->allocator_.allocate(size);
  }

  friend void asio_handler_deallocate(void* pointer,
                                      std::size_t,
                                      MemAllocator<Handler>* this_handler)
  {
    this_handler->allocator_.deallocate(pointer);
  }

 private:
  HandleAlloc& allocator_;
  Handler handler_;
};

#endif
