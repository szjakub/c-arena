/*
 * Copyright (c) 2025 Jakub Szkodny
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
*/

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
