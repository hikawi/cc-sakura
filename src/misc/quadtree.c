#include "misc/quadtree.h"

#include "app.h"
#include "engine/collision.h"
#include "misc/list.h"
#include "SDL3/SDL_stdinc.h"

#define MAX_COLLIDERS_PER_NODE 10
#define MAX_QUADTREE_DEPTH     7

QuadtreeNode *quadtree_init(void)
{
    QuadtreeNode *root = SDL_malloc(sizeof(QuadtreeNode));
    if (!root)
    {
        app_panic("Unable to allocate memory for a quadtree.");
        return NULL;
    }

    SDL_memset(root->children, 0, 4 * sizeof(QuadtreeNode *));
    root->colliders = list_init();
    root->depth = 0;
    if (!root->colliders)
    {
        SDL_free(root);
        app_panic("Unable to allocate memory for a quadtree's collider list");
        return NULL;
    }

    return root;
}

/**
 * Subdivides the current node into the next generation of quadtree nodes.
 */
void quadtree_subdivide(QuadtreeNode *node)
{
    if (!node)
    {
        return;
    }

    Vector2 dirs[4] = {
        (Vector2){.x = -node->region.w / 2, .y = -node->region.h / 2},
        (Vector2){.x = node->region.w / 2, .y = -node->region.h / 2},
        (Vector2){.x = -node->region.w / 2, .y = node->region.h / 2},
        (Vector2){.x = node->region.w / 2, .y = node->region.h / 2},
    };
    for (int i = 0; i < 4; i++)
    {
        node->children[i] = quadtree_init();
        node->children[i]->depth = node->depth + 1;
        node->children[i]->region.x = node->region.x + dirs[i].x;
        node->children[i]->region.y = node->region.y + dirs[i].y;
        node->children[i]->region.w = node->region.w / 2;
        node->children[i]->region.h = node->region.h / 2;
    }
}

bool quadtree_add(QuadtreeNode *root, const Collider *item)
{
    if (!root || !item)
    {
        return false;
    }

    Collider aabb = collision_convert_to_aabb(item);

    if (!root->children[0])
    {
        if (root->colliders->length < MAX_COLLIDERS_PER_NODE || root->depth >= MAX_QUADTREE_DEPTH)
        {
            list_add(root->colliders, item);
            return true;
        }

        quadtree_subdivide(root);

        // Move colliders into children.
        List *current_colliders = list_init();
        for (uint32_t i = 0; i < root->colliders->length; i++)
        {
            Collider col = collision_convert_to_aabb(root->colliders->items[i]);

            bool added = false;
            for (int j = 0; j < 4; j++)
            {
                Collider child;
                child.collider_shape_type = COLLIDER_SHAPE_TYPE_AABB;
                child.aabb = root->children[j]->region;

                if (collision_is_fully_enclosed(&child, &col))
                {
                    added = quadtree_add(root->children[j], root->colliders->items[i]);
                    break;
                }
            }

            if (!added)
            {
                list_add(current_colliders, root->colliders->items[i]);
            }
        }

        // Move all back if needed.
        list_clear(root->colliders);
        list_join(root->colliders, current_colliders);
        list_destroy(current_colliders);

        // Find where to put our current collider.
        bool added = false;
        for (int i = 0; i < 4; i++)
        {
            QuadtreeNode *child_node = root->children[i];
            Collider child;
            child.collider_shape_type = COLLIDER_SHAPE_TYPE_AABB;
            child.aabb = child_node->region;

            if (collision_is_fully_enclosed(&child, &aabb))
            {
                added = quadtree_add(child_node, item);
                if (added)
                {
                    break;
                }
            }
        }

        if (!added)
        {
            list_add(root->colliders, item);
        }
        return true;
    }
    else
    {
        // Not a leaf node, check children.
        bool added = false;
        for (int i = 0; i < 4; i++)
        {
            QuadtreeNode *child_node = root->children[i];
            Collider child;
            child.collider_shape_type = COLLIDER_SHAPE_TYPE_AABB;
            child.aabb = child_node->region;

            if (collision_is_fully_enclosed(&child, &aabb))
            {
                added = quadtree_add(child_node, item);
                if (added)
                {
                    break;
                }
            }
        }

        if (!added)
        {
            list_add(root->colliders, item);
        }
        return true;
    }
}

/**
 * Recursively looks for an item and delete it from a quadtree.
 */
bool quadtree_remove_recur(QuadtreeNode *cur, QuadtreeNode *parent, const Collider *item)
{
    if (!cur)
    {
        return false;
    }

    int idx = list_find(cur->colliders, item);
    if (idx >= 0)
    {
        list_remove_at(cur->colliders, (uint32_t)idx);

        // Merge if needed.
        // If this is root, no need to merge at all.
        if (!parent)
        {
            return true;
        }

        uint32_t count = parent->colliders->length;
        for (int i = 0; i < 4; i++)
        {
            if (!parent->children[i])
            {
                continue;
            }
            count += parent->children[i]->colliders->length;
        }

        if (count <= MAX_COLLIDERS_PER_NODE)
        {
            // We should merge.
            for (int i = 0; i < 4; i++)
            {
                if (!parent->children[i])
                {
                    continue;
                }

                list_join(parent->colliders, parent->children[i]->colliders);
                quadtree_destroy(parent->children[i]);
                parent->children[i] = NULL;
            }
        }

        return true;
    }
    else if (cur->children[0])
    {
        for (int i = 0; i < 4; i++)
        {
            if (quadtree_remove_recur(cur->children[i], cur, item))
                return true;
        }
    }

    // That node doesn't exist.
    return false;
}

bool quadtree_remove(QuadtreeNode *root, const Collider *item)
{
    return quadtree_remove_recur(root, NULL, item);
}

void quadtree_query(const QuadtreeNode *root, const Collider *collider, List *list)
{
    if (!root || !collider || !list)
    {
        return;
    }

    Collider node;
    node.collider_shape_type = COLLIDER_SHAPE_TYPE_AABB;
    node.aabb = root->region;

    Collider target = collision_convert_to_aabb(collider);

    if (!collision_check(&node, &target).is_colliding)
    {
        // There is absolutely no chance anyone here collides.
        return;
    }

    // Check all current colliders.
    for (uint32_t i = 0; i < root->colliders->length; i++)
    {
        const Collider *potential = root->colliders->items[i];
        if (potential != collider && collision_partial_check(&target, potential).is_colliding)
        {
            list_add(list, potential);
        }
    }

    // Recurse
    for (int i = 0; i < 4; i++)
    {
        quadtree_query(root->children[i], collider, list);
    }
}

void quadtree_destroy(QuadtreeNode *root)
{
    if (!root)
    {
        return;
    }

    if (root->colliders)
    {
        list_destroy(root->colliders);
    }
    for (int i = 0; i < 4; i++)
    {
        if (root->children[i])
        {
            quadtree_destroy(root->children[i]);
            root->children[i] = NULL;
        }
    }
    SDL_free(root);
}
