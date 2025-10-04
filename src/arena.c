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

/* round up an offset to the next multiple of the alignment */
size_t arena_alignup(size_t region_size, size_t alignment)
{
    assert(alignment > 0);
    return (region_size + alignment - 1) / alignment * alignment;
}

void arena_init(Arena *arena)
{
    arena->start = arena->end = arena_new_region(ARENA_DEFAULT_REGION_SIZE);
}

void *_arena_malloc(Arena *arena, size_t alignment, size_t size)
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

void arena_free(Arena *arena)
{
    Region *region = arena->start;
    while (region != NULL)
    {
        Region *tmp = region;
        region = region->next;
        free(tmp);
    }
}
