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

#include "arena.h"

static Region *arena_new_region(size_t capacity)
{
    Region *region = calloc(1, offsetof(Region, data) + capacity);
    if (region == NULL) return NULL;
    region->size     = 0;
    region->capacity = capacity;
    region->next     = NULL;
    return region;
}

int arena_init(Arena *arena)
{
    Region *new_region = arena_new_region(ARENA_DEFAULT_REGION_SIZE);
    if (new_region == NULL) return -1;

    arena->start = arena->end = arena_new_region(ARENA_DEFAULT_REGION_SIZE);
    return 0;
}

void arena_deinit(Arena *arena)
{
    Region *region = arena->start;
    while (region != NULL)
    {
        Region *tmp = region;
        region = region->next;
        free(tmp);
    }
}

/* round up an offset to the next multiple of the alignment */
size_t arena_alignup(size_t size, size_t alignment)
{
    assert(alignment > 0);
    return ((size + alignment - 1) / alignment) * alignment;
}

void *arena_alloc(Arena *arena, size_t size)
{
    return arena_alloc_aligned(arena, size, ARENA_DEFAULT_ALIGNMENT);
}

void *arena_alloc_aligned(Arena *arena, size_t size, size_t alignment)
{
    Region *current_region = arena->start;
    while (current_region)
    {
        size_t offset = arena_alignup(current_region->size, alignment);
        if (current_region->capacity >= offset + size)
        {
            current_region->size = offset + size;
            /* ensure pointer arithmetic is byte-wise */
            return (void *)((char *)current_region->data + offset);
        }
        current_region = current_region->next;
    }
    Region *new_region = arena_new_region(MAX(size, ARENA_DEFAULT_REGION_SIZE));
    if (new_region == NULL) return NULL;

    arena->end->next = new_region;
    arena->end = new_region;
    new_region->size = size;
    return (void *)(new_region->data);
}
