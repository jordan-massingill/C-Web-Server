#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hashtable.h"
#include "cache.h"

/**
 * Allocate a cache entry
 */
struct cache_entry *alloc_entry(char *path, char *content_type, void *content, int content_length)
{
    ///////////////////
    // IMPLEMENT ME! //
    ///////////////////
    struct cache_entry *newentry = malloc(sizeof(struct cache_entry));
    newentry->next = NULL;
    newentry->prev = NULL;
    newentry->path = strdup(path);
    newentry->content_type = strdup(content_type);
    newentry->content_length = content_length;
    newentry->content = malloc(content_length+1);
    newentry->content = memcpy(newentry->content, content, content_length+1);
    return newentry;
}

/**
 * Deallocate a cache entry
 */
void free_entry(struct cache_entry *entry)
{
    ///////////////////
    // IMPLEMENT ME! //
    ///////////////////
    free(entry->content);
    free(entry->path);
    free(entry->content_type);
    // free(entry->content_length);
    free(entry);

}

/**
 * Insert a cache entry at the head of the linked list
 */
void dllist_insert_head(struct cache *cache, struct cache_entry *ce)
{
    // Insert at the head of the list
    if (cache->head == NULL) {
        cache->head = cache->tail = ce;
        ce->prev = ce->next = NULL;
    } else {
        cache->head->prev = ce;
        ce->next = cache->head;
        ce->prev = NULL;
        cache->head = ce;
    }
}

/**
 * Move a cache entry to the head of the list
 */
void dllist_move_to_head(struct cache *cache, struct cache_entry *ce)
{
    if (ce != cache->head) {
        if (ce == cache->tail) {
            // We're the tail
            cache->tail = ce->prev;
            cache->tail->next = NULL;

        } else {
            // We're neither the head nor the tail
            ce->prev->next = ce->next;
            ce->next->prev = ce->prev;
        }

        ce->next = cache->head;
        cache->head->prev = ce;
        ce->prev = NULL;
        cache->head = ce;
    }
}


/**
 * Removes the tail from the list and returns it
 *
 * NOTE: does not deallocate the tail
 */
struct cache_entry *dllist_remove_tail(struct cache *cache)
{
    struct cache_entry *oldtail = cache->tail;

    cache->tail = oldtail->prev;
    cache->tail->next = NULL;

    cache->cur_size--;

    return oldtail;
}

/**
 * Create a new cache
 *
 * max_size: maximum number of entries in the cache
 * hashsize: hashtable size (0 for default)
 */
struct cache *cache_create(int max_size, int hashsize)
{
    ///////////////////
    // IMPLEMENT ME! //
    ///////////////////
    struct cache *newcache = malloc(sizeof(struct cache));
    newcache->index = hashtable_create(hashsize, NULL);
    newcache->max_size = max_size;
    newcache->cur_size = hashsize;
    newcache->head = malloc(sizeof(struct cache_entry));
    newcache->head = NULL;
    newcache->tail = malloc(sizeof(struct cache_entry));
    newcache->tail = NULL;
    return newcache;
}

void cache_free(struct cache *cache)
{
    struct cache_entry *cur_entry = cache->head;

    hashtable_destroy(cache->index);

    while (cur_entry != NULL) {
        struct cache_entry *next_entry = cur_entry->next;

        free_entry(cur_entry);

        cur_entry = next_entry;
    }

    free(cache);
}

/**
 * Store an entry in the cache
 *
 * This will also remove the least-recently-used items as necessary.
 *
 * NOTE: doesn't check for duplicate cache entries
 */
void cache_put(struct cache *cache, char *path, char *content_type, void *content, int content_length)
{
    ///////////////////
    // IMPLEMENT ME! //
    ///////////////////
    struct cache_entry *newentry = alloc_entry(path, content_type, content, content_length);
    dllist_insert_head(cache, newentry);
    hashtable_put(cache->index, newentry->path, newentry);
    cache->cur_size+=1;
    if (cache->cur_size > cache->max_size) {
      hashtable_delete(cache->index, cache->tail->path);
      free_entry(dllist_remove_tail(cache));
    }
}

/**
 * Retrieve an entry from the cache
 */
struct cache_entry *cache_get(struct cache *cache, char *path)
{
    ///////////////////
    // IMPLEMENT ME! //
    ///////////////////
    struct cache_entry *entry = hashtable_get(cache->index, path);
    if (entry == NULL) {
      return NULL;
    }
    dllist_move_to_head(cache, entry);
    return entry;
}
