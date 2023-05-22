#include <sx/allocator.h>
#include <tarch/tarch.h>

void* tarch_malloc(size_t size, void* user_data)
{
  sx_unused(user_data);
  return sx_malloc(sx_alloc_malloc(), size);
}

void* tarch_calloc(size_t n, size_t s, void* user_data)
{
  sx_unused(user_data);
  return sx_calloc(sx_alloc_malloc(), n * s);
}

void* tarch_realloc(void* p, size_t s, void* user_data)
{
  sx_unused(user_data);
  return sx_realloc(sx_alloc_malloc(), p, s);
}

void tarch_free(void* ptr, void* user_data)
{
  sx_unused(user_data);
  sx_free(sx_alloc_malloc(), ptr);
}
