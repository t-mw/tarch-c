# Best Practices

## Style

- Upper snake case for constants, including enum members.
- Lower snake case for everything else.
- Fully qualify structs and enums (no typedefs).
- Indent with four spaces.
- Free function suffixes:
  - `*_new`: allocate with automatic storage
  - `*_create`: allocate with allocated storage and initialize
    - `*_alloc`: allocate memory for struct
    - `*_init`: initialize struct fields
  - `*_destroy`: discard and free
    - `*_discard`: deallocate struct fields
    - `*_free`: deallocate struct
  - `*_get_*`: getter function
  - `*_set_*`: setter function
- Write types so that they can be read right-to-left:
  - `int const` instead of `const int`
  - `int * const` instead of `int const *`

## Memory Management

- Have a single owner and know what it is.
- When moving or freeing a pointer, null the source pointer.
- Use `const` to enforce ownership when passing pointers (both `X *const` and `X const *` disable freeing).
- When using `malloc`, use `sizeof *var_name` rather than `sizeof(type)`.
- Use `memmove` instead of `memcpy` if the source and destination regions may overlap.
- Take advantage of `goto` and `free(NULL)` for clarity during cleanup, e.g:

  ```
    void *p = NULL;
    void *q = NULL;
    void *r = NULL;

    p = a();
    if (!p) goto cleanup;
    q = b();
    if (!q) goto cleanup;
    r = c();
    if (!r) goto cleanup;

    // Success case.  Give r to the caller.
    *callerPointer = r;
    r = NULL;

    // Clean everything up at once.
    // r is null now so free(r) is harmless.
  cleanup:
    free(p);
    free(q);
    free(r);
  ```

## Headers

- Enforce encapsulation with:
  - [opaque pointers](https://stackoverflow.com/questions/7553750/what-is-an-opaque-pointer-in-c)
  - `static` functions in source files

## Strings

- Use `malloc`, `free`, `strlen`, `memcpy`, `snprintf` and `asprintf` for processing.
- Use `strchr`, `strstr`, `strspn` and `strcspn` for searching.
- Use `strtoi` and `strtof` for parsing integers and floats.
- Use `strtok_r` and `strsep` for splitting.
