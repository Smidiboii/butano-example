#include "btn_sorted_sprites.h"

#include "btn_pool.h"
#include "btn_vector.h"
#include "btn_config_sprites.h"
#include "btn_sprites_manager_item.h"
#include "../hw/include/btn_hw_bgs.h"

namespace btn::sorted_sprites
{

namespace
{
    static_assert(BTN_CFG_SPRITES_MAX_SORT_LAYERS >= hw::bgs::count() &&
                  BTN_CFG_SPRITES_MAX_SORT_LAYERS <= BTN_CFG_SPRITES_MAX_ITEMS);

    class static_data
    {

    public:
        pool<layer, BTN_CFG_SPRITES_MAX_SORT_LAYERS> layer_pool;
        vector<layer*, BTN_CFG_SPRITES_MAX_SORT_LAYERS> layer_ptrs;
    };

    BTN_DATA_EWRAM static_data data;

    [[nodiscard]] ivector<layer*>::iterator _find_layers_it(sort_key key)
    {
        layers_type& layers = data.layer_ptrs;
        return lower_bound(layers.begin(), layers.end(), key, [](const layer* layer, sort_key sort_key) {
            return layer->layer_sort_key() < sort_key;
        });
    }
}

layers_type& layers()
{
    return data.layer_ptrs;
}

void insert(sprites_manager_item& item)
{
    layers_type& layers = data.layer_ptrs;
    sort_key sort_key = item.sprite_sort_key;
    ivector<layer*>::iterator layers_it = _find_layers_it(sort_key);

    if(layers_it == layers.end())
    {
        BTN_ASSERT(! layers.full(), "No more sprite sort layers available");

        layer& pool_layer = data.layer_pool.create(sort_key);
        layers.push_back(&pool_layer);
    }
    else if(sort_key != (*layers_it)->layer_sort_key())
    {
        BTN_ASSERT(! layers.full(), "No more sprite sort layers available");

        layer& pool_layer = data.layer_pool.create(sort_key);
        layers_it = layers.insert(layers_it, &pool_layer);
    }

    layer* layer = *layers_it;
    layer->push_front(item);
}

void erase(sprites_manager_item& item)
{
    layers_type& layers = data.layer_ptrs;
    sort_key sort_key = item.sprite_sort_key;
    ivector<layer*>::iterator layers_it = _find_layers_it(sort_key);
    BTN_ASSERT(layers_it != layers.end(),
               "Sprite sort key not found: ", sort_key.priority(), " - ", sort_key.z_order());
    BTN_ASSERT(sort_key == (*layers_it)->layer_sort_key(),
               "Sprite sort key not found: ", sort_key.priority(), " - ", sort_key.z_order());

    layer* layer = *layers_it;
    layer->erase(item);

    if(layer->empty())
    {
        layers.erase(layers_it);
        data.layer_pool.destroy(*layer);
    }
}

bool put_in_front_of_layer(sprites_manager_item& item)
{
    sort_key sort_key = item.sprite_sort_key;
    ivector<layer*>::iterator layers_it = _find_layers_it(sort_key);
    BTN_ASSERT(layers_it != data.layer_ptrs.end(),
               "Sprite sort key not found: ", sort_key.priority(), " - ", sort_key.z_order());
    BTN_ASSERT(sort_key == (*layers_it)->layer_sort_key(),
               "Sprite sort key not found: ", sort_key.priority(), " - ", sort_key.z_order());

    layer* layer = *layers_it;
    bool sort = &layer->front() != &item;

    if(sort)
    {
        layer->erase(item);
        layer->push_front(item);
    }

    return sort;
}

}
