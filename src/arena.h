#ifndef _ARENA_H
#define _ARENA_H

#include <stdint.h>
#include <stdlib.h>
#include <stddef.h>
#include <assert.h>

#define ARENA_DEFAULT_REGION_SIZE (1024*8)

#define MAX(a,b) (((a) > (b)) ? (a) : (b))

/* determine the alignment required for a given type */
#define alignof(type) ((size_t)offsetof(struct {char c; type t;}, t))

/* round up an offset to the next multiple of the given type's alignment */
/* #define alignup(offset, type) (((size_t)(offset) + alignof(type) - 1) / alignof(type) * alignof(type)) */

/*
 * TODO: maybe add some macro magic for different platforms?
 * https://www.gnu.org/software/c-intro-and-ref/manual/html_node/Type-Alignment.html
*/

#define arena_malloc(arena, type, nmembers) _arena_malloc(arena, alignof(type), sizeof(type) * nmembers)

typedef union {
    long long   ll;
    long double ld;
    void        *v;
} arena_max_align_t;

typedef struct Region Region;

struct Region
{
    Region *next;
    size_t capacity;
    size_t size;
    arena_max_align_t data[];
};

typedef struct {
    Region *start, *end;
} Arena;

void arena_init(Arena *arena);
void *_arena_malloc(Arena *arena, size_t alignment, size_t size);
void arena_free(Arena *arena);

static Region *arena_new_region(size_t size);

#endif
