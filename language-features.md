# Language Features

<!--- Copied from annex/Documents/Learning/Notes/C -->

## Storage Duration

- **External** (`extern`) linkage means that symbols that match across files should be treated as the same thing by the linker.
- **Internal** (`static`) linkage indicates that a file’s instance of a variable x or a function f() is its own and matches only other instances of x or f() in the same scope (which for things declared outside of any functions would be file scope).
- In **file scope** the default linkage for variables and functions is **external**, adding static switches it to internal.
- Static can also refer to the **storage duration** of a variable. The main types of storage duration are **static**, **automatic** and **allocated**.
- Variables and functions in **file scope** always have a **static storage duration**, regardless of their linkage.
- Variables in **block scope** have an **automatic or allocated storage duration**, and can be changed to have a static storage duration by adding static.

Properties of the storage durations:

|                                    | Static | Automatic | Allocated |
| ---------------------------------- | ------ | --------- | --------- |
| Set to zero on startup             | x      |           |           |
| Scope-limited                      | x      | x         |           |
| Can set values on init             | x      | x         |           |
| Can set nonconstant values on init |        | x         |           |
| sizeofmeasures array size          | x      | x         |           |
| Persists across function calls     | x      |           | x         |
| Can be global                      | x      |           | x         |
| Array size can be set at runtime   |        | x         | x         |
| Can be resized                     |        |           | x         |

More details at https://en.cppreference.com/w/c/language/storage_duration

## Inlining

To write an inline function in c99, write its definition in a header file:

```
inline void func(...) { ... }
```

And then in exactly one compilation unit (.c file) write:

```
extern inline void func(...);
```

## Arrays

### Designated initialisers

Can be initialised using designated initialisers:

`int a[6] = { [4] = 29, [2] = 15 };`

`int widths[] = { [0 ... 9] = 1, [10 ... 99] = 2, [100] = 3 };`

which can be combined with characters or enum values:

`int whitespace[256] = { [' '] = 1, ['\t'] = 1, ['\n'] = 1, ['\r'] = 1 };`

### Compound Literal

Here `list` is an automatically allocated array:

`double list[] = {1.1, 2.2, 3.3, NAN};`

Like an array, a compound literal is a list of elements. However, a compound literal is an anonymous list which is cast to a pointer:

`double *list = (double[]){1.1, 2.2, 3.3, NAN};`

`sizeof(list)` will therefore return the size of the pointer, rather than the size of the list itself. Unlike an array, the compound literal can be used directly as an argument to a function.

## Strings

Typical usage:

    #include <string.h>
    char *str1 = "hello", str2[100];
    strlen(str1); //get the length up to but excluding the ’\0’
    strncpy(str2, 100, str1); //copy at most 100 bytes from str1 to str2
    strncat(str2, 100, str1); //append at most 100 bytes from str1 onto str2
    strcmp(str1, str2); //are str1 and str2 different? zero=no, nonzero=yes
    snprintf(str2, 100, "str1 says: %s", str1); //write to a string, as above.

Follow [this](http://stackoverflow.com/a/4580503) convention when writing string handling functions.

One [opinion](https://stackoverflow.com/questions/4688041/good-c-string-library):

> I would suggest not using any library aside from `malloc`, `free`, `strlen`, `memcpy`, and `snprintf`. These functions give you all of the tools for powerful, safe, and efficient string processing in C. Just stay away from `strcpy`, `strcat`, `strncpy`, and `strncat`, all of which tend to lead to inefficiency and exploitable bugs.
>
> \[for searching\], `strchr` and `strstr` are almost certainly going to be what you want to use. `strspn` and `strcspn` can also be useful.

### asprintf

`asprintf` can be used to perform operations with strings safely:

    int size = asprintf(&dest, "concatenate %s", "this");
    if (size == -1) { /* handle error*/ }
    free(dest);

If the function isn't available on a platform, it can be manually defined (see 21st Century C, Example 9-3).

Calling `asprintf` with the same pointer multiple times and without calling `free` will cause a memory leak. The following macro can be used to reuse the same pointer and safely build a string incrementally:

    #define Sasprintf(write_to,  ...) {           \
        char *tmp_string_for_extend = (write_to); \
        asprintf(&(write_to), __VA_ARGS__);       \
        free(tmp_string_for_extend);              \
    }

    Sasprintf(q, "select * from tab");
    Sasprintf(q, "%s where col%i is not null", q, i);

### Constant strings

Strings embedded in the executable cannot be modified in any way. The following will segfault:

    char *s = "Thread";
    s[0]='t';

Get more conventional behaviour by using `strdup`:

    char *s3 = strdup("Thread");
    free(s3);

### From numbers

`atoi` and `atof` are simple but don't have any error checking and simply evaluate to zero if they fail.

Use `strtol(char *input, char **end, int base)` and `strtod(char *input, char **end)` instead.

With those functions `success = (**end == false)`, since `end` points to the first problematic character encountered during conversion, which will equal the final null character if successful.

### Splitting

`strtok_r`/`strtok_s` (has an extra length argument) or `strsep` ([see differences](http://stackoverflow.com/a/7219504)) can be used to split a string (`strtok` is not thread-safe).

### malloc

When using `malloc`, use `sizeof *var_name` rather than `sizeof(type)` in the expression for maintainability e.g. `int *foo = malloc(5*sizeof *foo)`.

An explicit cast on malloc's result isn't required, since it returns a (void \*) which will be implicitly converted to the assigning type.

### Reasons to use malloc

*   To create a resizable array.
*   To return an array from a function.
*   To persist objects beyond their automatic scope.
*   To allocate objects too large for the stack.

### memcpy vs. memmove

`memmove` offers guaranteed behavior if the memory regions pointed to by the source and destination arguments [overlap](http://www.tedunangst.com/flak/post/memcpy-vs-memmove).

## Enums

Declare enum. By not using a typedef the enum name does not pollute the type namespace.

`enum cardsuit { CLUBS, DIAMONDS };`

Declare a variable of the enum type:

`enum cardsuit suit;`

## Arrays

Static/automatic arrays can be initialised to zero when declared: `int a[10] = {}` or have their length inferred: `int a[] = { 1, 2, 3 }`.

`malloc()` leaves memory uninitialized, while `calloc()` zero-initializes the buffer at a slight performance cost.

An existing array can be resized using `realloc`.

## Const

You can switch a type name and const, and so write either int const or const int. The int const form provides consistency when reading from right to left:

`int const *` - A (variable) pointer to a constant integer

`int * const` - A constant pointer to a (variable) integer

If a third party function has been authored to behave as const but does not take a const, you can type-cast your input to non-const: `(int *)var_name`. Alternatively make a copy of the input and discard it after calling the function.

WARNING: Values in a const struct cannot be modified. However pointer values in a const struct can be.

## Argument annotations

`void func(int array[static 5])` - declares a function with an array of at least 5 elements as an argument

`void func(int const array[const static 5])` - declares a function with a const array of at least 5 const elements as an argument

`void func(int function(int, int))` - declares a function with a function as an argument

## Struct

`typedef struct { char *name; int left, right, up, down; } direction_s;`

### Designated initialisers

Define a struct using "designated initialisers", with a declaration:

`direction_s D = {.name="left", .left=1}`

or inline:

`return (direction_s) {"upper right", .up=1, .right=1}`

The element names can be omitted for brevity (see "upper right" in the last example).

The elements not declared are initialized to zero. Define a struct as `{}` and all members will be zero.

Struct designated initialisers can be combined with array initialisers:

`struct point ptarray[10] = { [2].y = yv2, [2].x = xv2, [0].x = xv0 };`

## typedef

Useful typedefs include `typedef char* string`.

Function signatures can be easily typed by simply declaring them: `typedef int func_type(int, int)`.

## nan

+inf, -inf and NaN can be represented by floats on most platforms (IEEE 754).

The sign bit can be 0 or 1, then the exponent is all 1s, and the rest (mantissa) is nonzero:

S11111111MMMMMMMMMMMMMMMMMMMMMMM

A zero mantissa indicates ±infinity, depending on the sign bit, but otherwise the mantissa can be anything. To store information in it use `nan(char *data)`.

## Preprocessor

String literals separated by whitespace are merged into a single literal:

`"first" "second" == "first second"`

### Macros

Rules:

*   Surround macro expressions in parens to avoid any confusion around precedence.
*   Avoid using variables twice, since this can lead to unexpected behaviour if inputs are e.g. prefix/postfix incremented. Take copies of the inputs instead: `__typeof__ (a) _a = (a);`
*   Surround blocks with curly braces.

To observe the output of macros only run the preprocessor on the source file e.g. `gcc -E source.c`.

The length of an automatic or static list can be calculated using the following macro:

`#define arraysize(list) sizeof(list)/sizeof(list[0])`

`##` can be used to construct identifiers from two input identifiers/macro variables. If the value of `name` is LL, then when you see `name ## _list`, read it as LL\_list.

`#` 'stringifies' an input variable, i.e. takes the value of the macro variable and surrounds it with quotes.

### Implementing default arguments

With some extra work structs can be used to [enable default arguments](http://stackoverflow.com/a/2926165) in C.

Alternatively 'stringifying' a macro variable and comparing it to '\\0' can be used to detect an input argument's absence. This has the advantage of working even when input arguments have a zero value, but requires more work creating macros than the previous approach.

### assert

Include `assert.h` and use:

`assert(condition && "message")`

and in C11: `static_assert(condition)` (or \_STATIC\_ASSERT or \_Static\_assert dependent on compiler)

### Test macros

The C standard defines some test macros. Others can be generated using Autoconf e.g. using `AC_CHECK_FUNCS`.

### Variadic Macros

    #define Print(message, ...) { \
        fprintf(message, __VA_ARGS__); \
    }

`__VA_ARGS__` is replaced by all arguments that fall under `...`.

It's possible to use compound literal arguents with variadic macros to e.g.ensure that the last element of a summed group of numbers is NaN:

    #define sum(...) sum_array((double[]){__VA_ARGS__, NAN})

NB: this only works with loose numbers.
