//
// Copyright 2018 Sepehr Taghdisian (septag@github). All rights reserved.
// License: https://github.com/septag/sx#license-bsd-2-clause
//
// array.h - v1.0 - Dynamic arrays
// Source: https://github.com/nothings/stb/blob/master/stb_ds.h
//
// clang-format off
//    Unlike C++ vector<>, the sx_array has the same
//    semantics as an object that you manually malloc and realloc.
//    The pointer may relocate every time you add a new object
//    to it, so you:
//
//         1. can't take long-term pointers to elements of the array
//         2. have to return the pointer from functions which might expand it
//            (either as a return value or by storing it to a ptr-to-ptr)
//
//    Non-function interface:
//
//      Declare an empty dynamic array of type T
//        T* foo = NULL;
//
//      Access the i'th item of a dynamic array 'foo' of type T, T* foo:
//        foo[i]
//
//    Functions (actually macros):
//
//      sx_array_free:
//        void sx_array_free(sx_alloc* alloc, T*);
//          Frees the array.
//
//      sx_array_count:
//        ptrdiff_t sx_array_count(T*);
//          Returns the number of elements in the array.
//
//      sx_array_countu:
//        size_t sx_array_countu(T*);
//          Returns the number of elements in the array as an unsigned type.
//
//      sx_array_pop:
//        T sx_array_pop(T* a)
//          Removes the final element of the array and returns it.
//
//      sx_array_push:
//        T sx_array_push(sx_alloc* alloc, T* a, T b);
//          Appends the item b to the end of array a. Returns b.
//
//      sx_array_ins:
//        T sx_array_ins(T* a, int p, T b);
//          Inserts the item b into the middle of array a, into a[p],
//          moving the rest of the array over. Returns b.
//
//      sx_array_insn:
//        void sx_array_ins(T* a, int p, int n);
//          Inserts n uninitialized items into array a starting at a[p],
//          moving the rest of the array over.
//
//      sx_array_addnptr:
//        T* sx_array_addnptr(sx_alloc* alloc, T* a, int n)
//          Appends n uninitialized items onto array at the end.
//          Returns a pointer to the first uninitialized item added.
//
//      sx_array_addnindex:
//        size_t sx_array_addnindex(sx_alloc* alloc, T* a, int n)
//          Appends n uninitialized items onto array at the end.
//          Returns the index of the first uninitialized item added.
//
//      sx_array_del:
//        void sx_array_del(T* a, int p);
//          Deletes the element at a[p], moving the rest of the array over.
//
//      sx_array_deln:
//        void sx_array_del(T* a, int p, int n);
//          Deletes n elements starting at a[p], moving the rest of the array over.
//
//      sx_array_delswap:
//        void sx_array_delswap(T* a, int p);
//          Deletes the element at a[p], replacing it with the element from
//          the end of the array. O(1) performance.
//
//      sx_array_setlen:
//        void sx_array_setlen(sx_alloc* alloc, T* a, int n);
//          Changes the length of the array to n. Allocates uninitialized
//          slots at the end if necessary.
//
//      sx_array_reserve:
//        size_t sx_array_reserve(sx_alloc* alloc, T* a, int n);
//          Sets the length of allocated storage to at least n. It will not
//          change the length of the array.
//
//      sx_array_cap:
//        size_t sx_array_cap(T* a);
//          Returns the number of total elements the array can contain without
//          needing to be reallocated.
//
//      sx_array_clear:
//        void sx_array_clear(T* a);
//          Resets the array count to zero, but does not resize memory.
//
// Usage:
//       NOTE: include "allocator.h" before array.h to prevent warnings and errors
//       sx_array_declare(SomeStruct) = NULL; // SomeStruct *arr = NULL;
//       while (something)
//       {
//          SomeStruct new_one;
//          new_one.whatever = whatever;
//          new_one.whatup   = whatup;
//          new_one.foobar   = barfoo;
//          sx_array_push(arr, new_one);
//       }
//
//     Note that these are all macros and many of them evaluate
//     their arguments more than once (except for 'v' in sx_array_push), so the arguments should
//     be side-effect-free.
//
//     Note that 'TYPE *a' in sx_array_push and sx_array_add* must be lvalues
//     so that the library can overwrite the existing pointer if the object has to be reallocated.
//
// NOTE for C++ users:
//     take a look at `sx_array` struct and it's members at the end of this file. It's a thin template
//     wrapper over array macros for more convenient C++ usage
//
#pragma once

typedef struct sx_alloc sx_alloc;

#define sx_array_setcap(_alloc,a,n)    (sx__arrgrow(_alloc,a,0,n))
#define sx_array_reserve               sx_array_setcap // synonym
#define sx_array_setlen(_alloc,a,n)    ((sx_array_cap(a) < (size_t) (n) ? sx_array_setcap((_alloc),(a),(size_t)(n)),0 : 0), (a) ? sx__header(a)->length = (size_t) (n) : 0)
#define sx_array_cap(a)                ((a) ? sx__header(a)->capacity : 0)
#define sx_array_len(a)                ((a) ? (ptrdiff_t) sx__header(a)->length : 0)
#define sx_array_lenu(a)               ((a) ?             sx__header(a)->length : 0)
#define sx_array_count                 sx_array_len // synonym
#define sx_array_countu                sx_array_lenu // synonym
#define sx_array_put(_alloc,a,v)       (sx__arrmaybegrow(_alloc,a,1), (a)[sx__header(a)->length++] = (v))
#define sx_array_push                  sx_array_put // synonym
#define sx_array_pop(a)                (sx__header(a)->length--, (a)[sx__header(a)->length])
#define sx_array_addnptr(_alloc,a,n)   (sx__arrmaybegrow(_alloc,a,n), sx__header(a)->length += (n), &(a)[sx__header(a)->length-(n)])
#define sx_array_addnindex(_alloc,a,n) (sx__arrmaybegrow(_alloc,a,n), sx__header(a)->length += (n), sx__header(a)->length-(n))
#define sx_array_add                   sx_array_addnptr // synonym
#define sx_array_last(a)               ((a)[sx__header(a)->length-1])
#define sx_array_free(_alloc, a)       ((void) ((a) ? sx_free(_alloc, sx__header(a)) : (void)0), (a)=NULL)
#define sx_array_del(a,i)              sx_array_deln(a,i,1)
#define sx_array_deln(a,i,n)           (sx_memmove(&(a)[i], &(a)[(i)+(n)], sizeof *(a) * (sx__header(a)->length-(n)-(i))), sx__header(a)->length -= (n))
#define sx_array_delswap(a,i)          ((a)[i] = sx_array_last(a), sx__header(a)->length -= 1)
#define sx_array_insn(_alloc,a,i,n)    ((void) (sx_array_addnptr(_alloc,(a),(n)), sx_memmove(&(a)[(i)+(n)], &(a)[i], sizeof *(a) * (sx__header(a)->length-(n)-(i)))))
#define sx_array_ins(a,i,v)            (sx_array_insn((a),(i),1), (a)[i]=(v))
#define sx_array_clear(a)              ((void) ((a) ? (sx__header(a)->length = 0) : 0))

////////////////////////////////////////////////////////////////////////////////////////////////////
// Internal

typedef struct {
  size_t      length;
  size_t      capacity;
  void      * hash_table;
  ptrdiff_t   temp;
} sx__array_header;

#define sx__header(t)                ((sx__array_header *) (t) - 1)
#define sx__arrmaybegrow(_alloc,a,n) ((!(a) || sx__header(a)->length + (n) > sx__header(a)->capacity) \
                                         ? (sx__arrgrow(_alloc,a,n,0),0) : 0)
#define sx__arrgrow(_alloc,a,b,c)    ((a) = sx__arrgrowf((a), sizeof *(a), (b), (c), (_alloc), __FILE__, __FUNCTION__, __LINE__))

// clang-format on
SX_INLINE void* sx__arrgrowf(void* a, size_t elemsize, size_t addlen, size_t min_cap,
                             const sx_alloc* alloc, const char* file, const char* func, int line)
{
    void* b;
    size_t min_len = sx_array_len(a) + addlen;

    // compute the minimum capacity needed
    if (min_len > min_cap)
        min_cap = min_len;

    if (min_cap <= sx_array_cap(a))
        return a;

    // increase needed capacity to guarantee O(1) amortized
    if (min_cap < 2 * sx_array_cap(a))
        min_cap = 2 * sx_array_cap(a);
    else if (min_cap < 4)
        min_cap = 4;

    // if (num_prev < 65536) if (a) prev_allocs[num_prev++] = (int *) ((char *) a+1);
    // if (num_prev == 2201)
    //  num_prev = num_prev;
    b = sx__realloc(alloc, (a) ? sx__header(a) : 0, elemsize * min_cap + sizeof(sx__array_header), 0,
                    file, func, line);

    if (!b) {
        sx_out_of_memory();
        return 0x0;    // NULL
    }

    // if (num_prev < 65536) prev_allocs[num_prev++] = (int *) (char *) b;
    b = (char*)b + sizeof(sx__array_header);
    if (a == NULL) {
        sx__header(b)->length = 0;
        sx__header(b)->hash_table = 0;
    }
    sx__header(b)->capacity = min_cap;

    return b;
}

// cpp wrapper (minimal template)
#ifdef __cplusplus
template <typename _T>
struct sx_array {
    _T* p;
    const sx_alloc* alloc;

    sx_array()
    {
        p = nullptr;
        alloc = nullptr;
    }
    explicit sx_array(const sx_alloc* _alloc) : alloc(_alloc), p(nullptr) {}

    ~sx_array()
    {
        if (alloc) {
            sx_array_free(alloc, p);
            alloc = nullptr;
        }
        p = nullptr;
    }

    void init(const sx_alloc* _alloc, int init_count = 0)
    {
        sx_assert(_alloc);
        this->alloc = _alloc;
        if (init_count > 0) {
            sx_array_reserve(_alloc, p, init_count);
        }
    }

    void release()
    {
        sx_assert(alloc);
        sx_array_free(alloc, p);
        p = nullptr;
        alloc = nullptr;
    }

    void push(const _T& _value)
    {
        sx_assert(alloc);
        sx_array_push(alloc, p, _value);
    }

    void del(int _index)
    {
        sx_assert(alloc);
        sx_assert(_index < sx_array_count(p));
        sx_array_del(alloc, _index);
    }

    void pop()
    {
        sx_assert(alloc);
        sx_assert(sx_array_count(p));
        sx_array_pop(p);
    }

    void clear()
    {
        sx_assert(alloc);
        sx_array_clear(p);
    }

    int count() const
    {
        sx_assert(alloc);
        return sx_array_count(p);
    }

    _T* expand(int _count)
    {
        sx_assert(alloc);
        return sx_array_add(alloc, p, _count);
    }

    void reserve(int _count)
    {
        sx_assert(alloc);
        sx_array_reserve(alloc, p, _count);
    }

    _T& operator[](int _index)
    {
        sx_assert(p);
        return this->p[_index];
    }

    const _T& operator[](int index) const
    {
        sx_assert(p);
        return this->p[index];
    }
};
#endif    // __cplusplus
