/*
 * Copyright (c) 2020 Gustavo Valiente gustavo.valiente@protonmail.com
 * zlib License, see LICENSE file.
 */

#include "bn_hblank_effects_manager.h"

#include "bn_vector.h"
#include "../hw/include/bn_hw_hblank_effects.h"

#include "bn_bg_palette_color_hblank_effect_handler.h"
#include "bn_bg_palettes_transparent_color_hblank_effect_handler.h"
#include "bn_blending_fade_alpha_hblank_effect_handler.h"
#include "bn_blending_transparency_attributes_hblank_effect_handler.h"
#include "bn_green_swap_hblank_effect_handler.h"
#include "bn_mosaic_attributes_hblank_effect_handler.h"
#include "bn_rect_window_horizontal_boundaries_hblank_effect_handler.h"
#include "bn_rect_window_vertical_boundaries_hblank_effect_handler.h"
#include "bn_regular_bg_attributes_hblank_effect_handler.h"
#include "bn_regular_bg_horizontal_position_hblank_effect_handler.h"
#include "bn_regular_bg_vertical_position_hblank_effect_handler.h"
#include "bn_sprite_affine_mat_pa_register_hblank_effect_handler.h"
#include "bn_sprite_affine_mat_pb_register_hblank_effect_handler.h"
#include "bn_sprite_affine_mat_pc_register_hblank_effect_handler.h"
#include "bn_sprite_affine_mat_pd_register_hblank_effect_handler.h"
#include "bn_sprite_first_attributes_hblank_effect_handler.h"
#include "bn_sprite_regular_second_attributes_hblank_effect_handler.h"
#include "bn_sprite_affine_second_attributes_hblank_effect_handler.h"
#include "bn_sprite_third_attributes_hblank_effect_handler.h"
#include "bn_sprite_horizontal_position_hblank_effect_handler.h"
#include "bn_sprite_vertical_position_hblank_effect_handler.h"
#include "bn_sprite_palette_color_hblank_effect_handler.h"

#include "bn_hblank_effects.cpp.h"
#include "bn_hblank_effect_ptr.cpp.h"
#include "bn_bg_palette_color_hblank_effect_ptr.cpp.h"
#include "bn_bg_palettes_transparent_color_hblank_effect_ptr.cpp.h"
#include "bn_blending_fade_alpha_hblank_effect_ptr.cpp.h"
#include "bn_blending_transparency_attributes_hblank_effect_ptr.cpp.h"
#include "bn_green_swap_hblank_effect_ptr.cpp.h"
#include "bn_mosaic_attributes_hblank_effect_ptr.cpp.h"
#include "bn_rect_window_boundaries_hblank_effect_ptr.cpp.h"
#include "bn_regular_bg_attributes_hblank_effect_ptr.cpp.h"
#include "bn_regular_bg_position_hblank_effect_ptr.cpp.h"
#include "bn_sprite_affine_mat_attributes_hblank_effect_ptr.cpp.h"
#include "bn_sprite_affine_mat_pa_register_hblank_effect_ptr.cpp.h"
#include "bn_sprite_affine_mat_pb_register_hblank_effect_ptr.cpp.h"
#include "bn_sprite_affine_mat_pc_register_hblank_effect_ptr.cpp.h"
#include "bn_sprite_affine_mat_pd_register_hblank_effect_ptr.cpp.h"
#include "bn_sprite_affine_second_attributes_hblank_effect_ptr.cpp.h"
#include "bn_sprite_first_attributes_hblank_effect_ptr.cpp.h"
#include "bn_sprite_palette_color_hblank_effect_ptr.cpp.h"
#include "bn_sprite_position_hblank_effect_ptr.cpp.h"
#include "bn_sprite_regular_second_attributes_hblank_effect_ptr.cpp.h"
#include "bn_sprite_third_attributes_hblank_effect_ptr.cpp.h"

namespace bn::hblank_effects_manager
{

namespace
{
    constexpr const int max_items = BN_CFG_HBLANK_EFFECTS_MAX_ITEMS;

    static_assert(max_items > 0 && max_items <= 8);

    using last_value_type = any<4 * sizeof(int)>;
    using hw_entry = hw::hblank_effects::entry;

    class item_type
    {

    public:
        last_value_type target_last_value;
        const void* values_ptr = nullptr;
        int target_id = 0;
        unsigned usages = 0;
        uint16_t* output_register = nullptr;
        alignas(int) uint16_t output_values_a[display::height()] = {};
        alignas(int) uint16_t output_values_b[display::height()] = {};
        handler_type handler;
        bool visible: 1 = false;
        bool update: 1 = false;
        bool on_screen: 1 = false;
        bool output_values_a_active: 1 = false;
        bool output_values_written: 1 = false;

        void setup_target()
        {
            switch(handler)
            {

            case handler_type::BG_PALETTE_COLOR:
                bg_palette_color_hblank_effect_handler::setup_target(target_id, target_last_value);
                break;

            case handler_type::BG_PALETTES_TRANSPARENT_COLOR:
                bg_palettes_transparent_color_hblank_effect_handler::setup_target(target_id, target_last_value);
                break;

            case handler_type::BLENDING_FADE_ALPHA:
                blending_fade_alpha_hblank_effect_handler::setup_target(target_id, target_last_value);
                break;

            case handler_type::BLENDING_TRANSPARENCY_ATTRIBUTES:
                blending_transparency_attributes_hblank_effect_handler::setup_target(target_id, target_last_value);
                break;

            case handler_type::GREEN_SWAP:
                green_swap_hblank_effect_handler::setup_target(target_id, target_last_value);
                break;

            case handler_type::MOSAIC_ATTRIBUTES:
                mosaic_attributes_hblank_effect_handler::setup_target(target_id, target_last_value);
                break;

            case handler_type::RECT_WINDOW_HORIZONTAL_BOUNDARIES:
                rect_window_horizontal_boundaries_hblank_effect_handler::setup_target(target_id, target_last_value);
                break;

            case handler_type::RECT_WINDOW_VERTICAL_BOUNDARIES:
                rect_window_vertical_boundaries_hblank_effect_handler::setup_target(target_id, target_last_value);
                break;

            case handler_type::REGULAR_BG_ATTRIBUTES:
                regular_bg_attributes_hblank_effect_handler::setup_target(target_id, target_last_value);
                break;

            case handler_type::REGULAR_BG_HORIZONTAL_POSITION:
                regular_bg_horizontal_position_hblank_effect_handler::setup_target(target_id, target_last_value);
                break;

            case handler_type::REGULAR_BG_VERTICAL_POSITION:
                regular_bg_vertical_position_hblank_effect_handler::setup_target(target_id, target_last_value);
                break;

            case handler_type::SPRITE_AFFINE_MAT_PA_REGISTER_ATTRIBUTES:
                sprite_affine_mat_pa_register_attributes_hblank_effect_handler::setup_target(target_id, target_last_value);
                break;

            case handler_type::SPRITE_AFFINE_MAT_PA_REGISTER_VALUES:
                sprite_affine_mat_pa_register_values_hblank_effect_handler::setup_target(target_id, target_last_value);
                break;

            case handler_type::SPRITE_AFFINE_MAT_PB_REGISTER_ATTRIBUTES:
                sprite_affine_mat_pb_register_attributes_hblank_effect_handler::setup_target(target_id, target_last_value);
                break;

            case handler_type::SPRITE_AFFINE_MAT_PB_REGISTER_VALUES:
                sprite_affine_mat_pb_register_values_hblank_effect_handler::setup_target(target_id, target_last_value);
                break;

            case handler_type::SPRITE_AFFINE_MAT_PC_REGISTER_ATTRIBUTES:
                sprite_affine_mat_pc_register_attributes_hblank_effect_handler::setup_target(target_id, target_last_value);
                break;

            case handler_type::SPRITE_AFFINE_MAT_PC_REGISTER_VALUES:
                sprite_affine_mat_pc_register_values_hblank_effect_handler::setup_target(target_id, target_last_value);
                break;

            case handler_type::SPRITE_AFFINE_MAT_PD_REGISTER_ATTRIBUTES:
                sprite_affine_mat_pd_register_attributes_hblank_effect_handler::setup_target(target_id, target_last_value);
                break;

            case handler_type::SPRITE_AFFINE_MAT_PD_REGISTER_VALUES:
                sprite_affine_mat_pd_register_values_hblank_effect_handler::setup_target(target_id, target_last_value);
                break;

            case handler_type::SPRITE_FIRST_ATTRIBUTES:
                sprite_first_attributes_hblank_effect_handler::setup_target(target_id, target_last_value);
                break;

            case handler_type::SPRITE_REGULAR_SECOND_ATTRIBUTES:
                sprite_regular_second_attributes_hblank_effect_handler::setup_target(target_id, target_last_value);
                break;

            case handler_type::SPRITE_AFFINE_SECOND_ATTRIBUTES:
                sprite_affine_second_attributes_hblank_effect_handler::setup_target(target_id, target_last_value);
                break;

            case handler_type::SPRITE_THIRD_ATTRIBUTES:
                sprite_third_attributes_hblank_effect_handler::setup_target(target_id, target_last_value);
                break;

            case handler_type::SPRITE_HORIZONTAL_POSITION:
                sprite_horizontal_position_hblank_effect_handler::setup_target(target_id, target_last_value);
                break;

            case handler_type::SPRITE_VERTICAL_POSITION:
                sprite_vertical_position_hblank_effect_handler::setup_target(target_id, target_last_value);
                break;

            case handler_type::SPRITE_PALETTE_COLOR:
                sprite_palette_color_hblank_effect_handler::setup_target(target_id, target_last_value);
                break;

            default:
                BN_ERROR("Unknown handler: ", int(handler));
                break;
            }
        }

        [[nodiscard]] bool check_update()
        {
            switch(handler)
            {

            case handler_type::BG_PALETTE_COLOR:
                return _check_update_impl<bg_palette_color_hblank_effect_handler>();

            case handler_type::BG_PALETTES_TRANSPARENT_COLOR:
                return _check_update_impl<bg_palettes_transparent_color_hblank_effect_handler>();

            case handler_type::BLENDING_FADE_ALPHA:
                return _check_update_impl<blending_fade_alpha_hblank_effect_handler>();

            case handler_type::BLENDING_TRANSPARENCY_ATTRIBUTES:
                return _check_update_impl<blending_transparency_attributes_hblank_effect_handler>();

            case handler_type::GREEN_SWAP:
                return _check_update_impl<green_swap_hblank_effect_handler>();

            case handler_type::MOSAIC_ATTRIBUTES:
                return _check_update_impl<mosaic_attributes_hblank_effect_handler>();

            case handler_type::RECT_WINDOW_HORIZONTAL_BOUNDARIES:
                return _check_update_impl<rect_window_horizontal_boundaries_hblank_effect_handler>();

            case handler_type::RECT_WINDOW_VERTICAL_BOUNDARIES:
                return _check_update_impl<rect_window_vertical_boundaries_hblank_effect_handler>();

            case handler_type::REGULAR_BG_ATTRIBUTES:
                return _check_update_impl<regular_bg_attributes_hblank_effect_handler>();

            case handler_type::REGULAR_BG_HORIZONTAL_POSITION:
                return _check_update_impl<regular_bg_horizontal_position_hblank_effect_handler>();

            case handler_type::REGULAR_BG_VERTICAL_POSITION:
                return _check_update_impl<regular_bg_vertical_position_hblank_effect_handler>();

            case handler_type::SPRITE_AFFINE_MAT_PA_REGISTER_ATTRIBUTES:
                return _check_update_impl<sprite_affine_mat_pa_register_attributes_hblank_effect_handler>();

            case handler_type::SPRITE_AFFINE_MAT_PA_REGISTER_VALUES:
                return _check_update_impl<sprite_affine_mat_pa_register_values_hblank_effect_handler>();

            case handler_type::SPRITE_AFFINE_MAT_PB_REGISTER_ATTRIBUTES:
                return _check_update_impl<sprite_affine_mat_pb_register_attributes_hblank_effect_handler>();

            case handler_type::SPRITE_AFFINE_MAT_PB_REGISTER_VALUES:
                return _check_update_impl<sprite_affine_mat_pb_register_values_hblank_effect_handler>();

            case handler_type::SPRITE_AFFINE_MAT_PC_REGISTER_ATTRIBUTES:
                return _check_update_impl<sprite_affine_mat_pc_register_attributes_hblank_effect_handler>();

            case handler_type::SPRITE_AFFINE_MAT_PC_REGISTER_VALUES:
                return _check_update_impl<sprite_affine_mat_pc_register_values_hblank_effect_handler>();

            case handler_type::SPRITE_AFFINE_MAT_PD_REGISTER_ATTRIBUTES:
                return _check_update_impl<sprite_affine_mat_pd_register_attributes_hblank_effect_handler>();

            case handler_type::SPRITE_AFFINE_MAT_PD_REGISTER_VALUES:
                return _check_update_impl<sprite_affine_mat_pd_register_values_hblank_effect_handler>();

            case handler_type::SPRITE_FIRST_ATTRIBUTES:
                return _check_update_impl<sprite_first_attributes_hblank_effect_handler>();

            case handler_type::SPRITE_REGULAR_SECOND_ATTRIBUTES:
                return _check_update_impl<sprite_regular_second_attributes_hblank_effect_handler>();

            case handler_type::SPRITE_AFFINE_SECOND_ATTRIBUTES:
                return _check_update_impl<sprite_affine_second_attributes_hblank_effect_handler>();

            case handler_type::SPRITE_THIRD_ATTRIBUTES:
                return _check_update_impl<sprite_third_attributes_hblank_effect_handler>();

            case handler_type::SPRITE_HORIZONTAL_POSITION:
                return _check_update_impl<sprite_horizontal_position_hblank_effect_handler>();

            case handler_type::SPRITE_VERTICAL_POSITION:
                return _check_update_impl<sprite_vertical_position_hblank_effect_handler>();

            case handler_type::SPRITE_PALETTE_COLOR:
                return _check_update_impl<sprite_palette_color_hblank_effect_handler>();

            default:
                BN_ERROR("Unknown handler: ", int(handler));
                return false;
            }
        }

        void setup_entry(hw_entry& entry) const
        {
            entry.src = output_values_a_active ? output_values_a : output_values_b;
            entry.dest = output_register;
        }

        void show()
        {
            switch(handler)
            {

            case handler_type::BG_PALETTE_COLOR:
                bg_palette_color_hblank_effect_handler::show(target_id);
                break;

            case handler_type::BG_PALETTES_TRANSPARENT_COLOR:
                bg_palettes_transparent_color_hblank_effect_handler::show(target_id);
                break;

            case handler_type::BLENDING_FADE_ALPHA:
                blending_fade_alpha_hblank_effect_handler::show(target_id);
                break;

            case handler_type::BLENDING_TRANSPARENCY_ATTRIBUTES:
                blending_transparency_attributes_hblank_effect_handler::show(target_id);
                break;

            case handler_type::GREEN_SWAP:
                green_swap_hblank_effect_handler::show(target_id);
                break;

            case handler_type::MOSAIC_ATTRIBUTES:
                mosaic_attributes_hblank_effect_handler::show(target_id);
                break;

            case handler_type::RECT_WINDOW_HORIZONTAL_BOUNDARIES:
                rect_window_horizontal_boundaries_hblank_effect_handler::show(target_id);
                break;

            case handler_type::RECT_WINDOW_VERTICAL_BOUNDARIES:
                rect_window_vertical_boundaries_hblank_effect_handler::show(target_id);
                break;

            case handler_type::REGULAR_BG_ATTRIBUTES:
                regular_bg_attributes_hblank_effect_handler::show(target_id);
                break;

            case handler_type::REGULAR_BG_HORIZONTAL_POSITION:
                regular_bg_horizontal_position_hblank_effect_handler::show(target_id);
                break;

            case handler_type::REGULAR_BG_VERTICAL_POSITION:
                regular_bg_vertical_position_hblank_effect_handler::show(target_id);
                break;

            case handler_type::SPRITE_AFFINE_MAT_PA_REGISTER_ATTRIBUTES:
                sprite_affine_mat_pa_register_attributes_hblank_effect_handler::show(target_id);
                break;

            case handler_type::SPRITE_AFFINE_MAT_PA_REGISTER_VALUES:
                sprite_affine_mat_pa_register_values_hblank_effect_handler::show(target_id);
                break;

            case handler_type::SPRITE_AFFINE_MAT_PB_REGISTER_ATTRIBUTES:
                sprite_affine_mat_pb_register_attributes_hblank_effect_handler::show(target_id);
                break;

            case handler_type::SPRITE_AFFINE_MAT_PB_REGISTER_VALUES:
                sprite_affine_mat_pb_register_values_hblank_effect_handler::show(target_id);
                break;

            case handler_type::SPRITE_AFFINE_MAT_PC_REGISTER_ATTRIBUTES:
                sprite_affine_mat_pc_register_attributes_hblank_effect_handler::show(target_id);
                break;

            case handler_type::SPRITE_AFFINE_MAT_PC_REGISTER_VALUES:
                sprite_affine_mat_pc_register_values_hblank_effect_handler::show(target_id);
                break;

            case handler_type::SPRITE_AFFINE_MAT_PD_REGISTER_ATTRIBUTES:
                sprite_affine_mat_pd_register_attributes_hblank_effect_handler::show(target_id);
                break;

            case handler_type::SPRITE_AFFINE_MAT_PD_REGISTER_VALUES:
                sprite_affine_mat_pd_register_values_hblank_effect_handler::show(target_id);
                break;

            case handler_type::SPRITE_FIRST_ATTRIBUTES:
                sprite_first_attributes_hblank_effect_handler::show(target_id);
                break;

            case handler_type::SPRITE_REGULAR_SECOND_ATTRIBUTES:
                sprite_regular_second_attributes_hblank_effect_handler::show(target_id);
                break;

            case handler_type::SPRITE_AFFINE_SECOND_ATTRIBUTES:
                sprite_affine_second_attributes_hblank_effect_handler::show(target_id);
                break;

            case handler_type::SPRITE_THIRD_ATTRIBUTES:
                sprite_third_attributes_hblank_effect_handler::show(target_id);
                break;

            case handler_type::SPRITE_HORIZONTAL_POSITION:
                sprite_horizontal_position_hblank_effect_handler::show(target_id);
                break;

            case handler_type::SPRITE_VERTICAL_POSITION:
                sprite_vertical_position_hblank_effect_handler::show(target_id);
                break;

            case handler_type::SPRITE_PALETTE_COLOR:
                sprite_palette_color_hblank_effect_handler::show(target_id);
                break;

            default:
                BN_ERROR("Unknown handler: ", int(handler));
                break;
            }
        }

        void cleanup()
        {
            switch(handler)
            {

            case handler_type::BG_PALETTE_COLOR:
                bg_palette_color_hblank_effect_handler::cleanup(target_id);
                break;

            case handler_type::BG_PALETTES_TRANSPARENT_COLOR:
                bg_palettes_transparent_color_hblank_effect_handler::cleanup(target_id);
                break;

            case handler_type::BLENDING_FADE_ALPHA:
                blending_fade_alpha_hblank_effect_handler::cleanup(target_id);
                break;

            case handler_type::BLENDING_TRANSPARENCY_ATTRIBUTES:
                blending_transparency_attributes_hblank_effect_handler::cleanup(target_id);
                break;

            case handler_type::GREEN_SWAP:
                green_swap_hblank_effect_handler::cleanup(target_id);
                break;

            case handler_type::MOSAIC_ATTRIBUTES:
                mosaic_attributes_hblank_effect_handler::cleanup(target_id);
                break;

            case handler_type::RECT_WINDOW_HORIZONTAL_BOUNDARIES:
                rect_window_horizontal_boundaries_hblank_effect_handler::cleanup(target_id);
                break;

            case handler_type::RECT_WINDOW_VERTICAL_BOUNDARIES:
                rect_window_vertical_boundaries_hblank_effect_handler::cleanup(target_id);
                break;

            case handler_type::REGULAR_BG_ATTRIBUTES:
                regular_bg_attributes_hblank_effect_handler::cleanup(target_id);
                break;

            case handler_type::REGULAR_BG_HORIZONTAL_POSITION:
                regular_bg_horizontal_position_hblank_effect_handler::cleanup(target_id);
                break;

            case handler_type::REGULAR_BG_VERTICAL_POSITION:
                regular_bg_vertical_position_hblank_effect_handler::cleanup(target_id);
                break;

            case handler_type::SPRITE_AFFINE_MAT_PA_REGISTER_ATTRIBUTES:
                sprite_affine_mat_pa_register_attributes_hblank_effect_handler::cleanup(target_id);
                break;

            case handler_type::SPRITE_AFFINE_MAT_PA_REGISTER_VALUES:
                sprite_affine_mat_pa_register_values_hblank_effect_handler::cleanup(target_id);
                break;

            case handler_type::SPRITE_AFFINE_MAT_PB_REGISTER_ATTRIBUTES:
                sprite_affine_mat_pb_register_attributes_hblank_effect_handler::cleanup(target_id);
                break;

            case handler_type::SPRITE_AFFINE_MAT_PB_REGISTER_VALUES:
                sprite_affine_mat_pb_register_values_hblank_effect_handler::cleanup(target_id);
                break;

            case handler_type::SPRITE_AFFINE_MAT_PC_REGISTER_ATTRIBUTES:
                sprite_affine_mat_pc_register_attributes_hblank_effect_handler::cleanup(target_id);
                break;

            case handler_type::SPRITE_AFFINE_MAT_PC_REGISTER_VALUES:
                sprite_affine_mat_pc_register_values_hblank_effect_handler::cleanup(target_id);
                break;

            case handler_type::SPRITE_AFFINE_MAT_PD_REGISTER_ATTRIBUTES:
                sprite_affine_mat_pd_register_attributes_hblank_effect_handler::cleanup(target_id);
                break;

            case handler_type::SPRITE_AFFINE_MAT_PD_REGISTER_VALUES:
                sprite_affine_mat_pd_register_values_hblank_effect_handler::cleanup(target_id);
                break;

            case handler_type::SPRITE_FIRST_ATTRIBUTES:
                sprite_first_attributes_hblank_effect_handler::cleanup(target_id);
                break;

            case handler_type::SPRITE_REGULAR_SECOND_ATTRIBUTES:
                sprite_regular_second_attributes_hblank_effect_handler::cleanup(target_id);
                break;

            case handler_type::SPRITE_AFFINE_SECOND_ATTRIBUTES:
                sprite_affine_second_attributes_hblank_effect_handler::cleanup(target_id);
                break;

            case handler_type::SPRITE_THIRD_ATTRIBUTES:
                sprite_third_attributes_hblank_effect_handler::cleanup(target_id);
                break;

            case handler_type::SPRITE_HORIZONTAL_POSITION:
                sprite_horizontal_position_hblank_effect_handler::cleanup(target_id);
                break;

            case handler_type::SPRITE_VERTICAL_POSITION:
                sprite_vertical_position_hblank_effect_handler::cleanup(target_id);
                break;

            case handler_type::SPRITE_PALETTE_COLOR:
                sprite_palette_color_hblank_effect_handler::cleanup(target_id);
                break;

            default:
                BN_ERROR("Unknown handler: ", int(handler));
                break;
            }
        }

    private:
        template<class Handler>
        [[nodiscard]] bool _check_update_impl()
        {
            bool updated = update;
            update = false;

            bool old_on_screen = on_screen;
            bool new_on_screen = Handler::target_visible(target_id);
            on_screen = new_on_screen;

            if(new_on_screen)
            {
                updated |= Handler::target_updated(target_id, target_last_value);

                if(! output_values_written)
                {
                    updated = true;
                    output_values_written = true;
                }

                if(updated)
                {
                    uint16_t* output_values_ptr;

                    if(output_values_a_active)
                    {
                        output_values_ptr = output_values_b;
                        output_values_a_active = false;
                    }
                    else
                    {
                        output_values_ptr = output_values_a;
                        output_values_a_active = true;
                    }

                    Handler::write_output_values(target_id, target_last_value, values_ptr, output_values_ptr);
                }

                uint16_t* old_output_register = output_register;
                output_register = Handler::output_register(target_id);
                updated |= old_output_register != output_register;
            }

            updated |= old_on_screen != new_on_screen;
            return updated;
        }
    };

    class static_external_data
    {

    public:
        item_type items[max_items];
        vector<int8_t, max_items> free_item_indexes;
        int8_t old_entries_count = 0;
        int8_t new_entries_count = 0;
        int8_t first_visible_item_index = max_items - 1;
        int8_t last_visible_item_index = 0;
        bool entries_a_active = false;
        bool update = false;
        bool commit = false;
        bool enabled = false;
    };

    class static_internal_data
    {

    public:
        hw_entry entries_a[max_items];
        hw_entry entries_b[max_items];
    };

    BN_DATA_EWRAM static_external_data external_data;
    static_internal_data internal_data;

    void _update_visible_item_index(int item_index)
    {
        static_external_data& data = external_data;
        data.items[item_index].show();

        if(data.first_visible_item_index != max_items)
        {
            data.first_visible_item_index = int8_t(min(int(data.first_visible_item_index), item_index));
            data.last_visible_item_index = int8_t(max(int(data.last_visible_item_index), item_index));
        }
    }

    void _update_hidden_item_index(int item_index)
    {
        static_external_data& data = external_data;
        data.items[item_index].cleanup();

        if(item_index == data.first_visible_item_index || item_index == data.last_visible_item_index)
        {
            data.first_visible_item_index = max_items;
            data.last_visible_item_index = 0;
        }
    }

    [[nodiscard]] int _create(const void* values_ptr, int target_id, handler_type handler)
    {
        int item_index = external_data.free_item_indexes.back();
        external_data.free_item_indexes.pop_back();

        item_type& new_item = external_data.items[item_index];
        new_item.values_ptr = values_ptr;
        new_item.target_id = target_id;
        new_item.usages = 1;
        new_item.output_register = nullptr;
        new_item.handler = handler;
        new_item.visible = true;
        new_item.update = true;
        new_item.on_screen = false;
        new_item.output_values_written = false;
        new_item.setup_target();

        _update_visible_item_index(item_index);
        external_data.update = true;

        return item_index;
    }
}

void init()
{
    hw::hblank_effects::init();

    for(int index = max_items - 1; index >= 0; --index)
    {
        external_data.free_item_indexes.push_back(int8_t(index));
    }
}

int used_count()
{
    return external_data.free_item_indexes.available();
}

int available_count()
{
    return external_data.free_item_indexes.size();
}

void enable()
{
    if(external_data.enabled)
    {
        hw::hblank_effects::enable();
    }
}

void disable()
{
    if(external_data.enabled)
    {
        hw::hblank_effects::disable();
    }
}

int create(const void* values_ptr, [[maybe_unused]] int values_count, int target_id, handler_type handler)
{
    BN_ASSERT(values_ptr, "Values ptr is null");
    BN_ASSERT(aligned<alignof(int)>(values_ptr), "Values are not aligned");
    BN_ASSERT(values_count == display::height(), "Invalid values count: ", values_count, " - ", display::height());
    BN_ASSERT(! external_data.free_item_indexes.empty(), "No more available HBlank effects");

    return _create(values_ptr, target_id, handler);
}

int create_optional(const void* values_ptr, [[maybe_unused]] int values_count, int target_id, handler_type handler)
{
    BN_ASSERT(values_ptr, "Values ptr is null");
    BN_ASSERT(aligned<alignof(int)>(values_ptr), "Values are not aligned");
    BN_ASSERT(values_count == display::height(), "Invalid values count: ", values_count, " - ", display::height());

    if(external_data.free_item_indexes.empty())
    {
        return -1;
    }

    return _create(values_ptr, target_id, handler);
}

void increase_usages(int id)
{
    item_type& item = external_data.items[id];
    ++item.usages;
}

void decrease_usages(int id)
{
    item_type& item = external_data.items[id];
    --item.usages;

    if(! item.usages)
    {
        if(item.visible)
        {
            item.visible = false;

            _update_hidden_item_index(id);
            external_data.update = true;
        }

        external_data.free_item_indexes.push_back(int8_t(id));
        item.target_last_value.reset();
        item.update = false;
    }
}

const void* values_ref(int id)
{
    const item_type& item = external_data.items[id];
    return item.values_ptr;
}

void set_values_ref(int id, const void* values_ptr, [[maybe_unused]] int values_count)
{
    BN_ASSERT(values_ptr, "Values ptr is null");
    BN_ASSERT(aligned<alignof(int)>(values_ptr), "Values are not aligned");
    BN_ASSERT(values_count == display::height(), "Invalid values count: ", values_count, " - ", display::height());

    item_type& item = external_data.items[id];
    item.values_ptr = values_ptr;
    item.update = true;

    if(item.visible)
    {
        external_data.update = true;
    }
}

void reload_values_ref(int id)
{
    item_type& item = external_data.items[id];
    item.update = true;

    if(item.visible)
    {
        external_data.update = true;
    }
}

[[nodiscard]] bool visible(int id)
{
    const item_type& item = external_data.items[id];
    return item.visible;
}

void set_visible(int id, bool visible)
{
    item_type& item = external_data.items[id];

    if(visible != item.visible)
    {
        item.visible = visible;
        external_data.update = true;

        if(visible)
        {
            _update_visible_item_index(id);
        }
        else
        {
            _update_hidden_item_index(id);
        }
    }
}

void update()
{
    bool update = external_data.update;
    external_data.update = false;

    int first_visible_item_index = external_data.first_visible_item_index;
    int last_visible_item_index = external_data.last_visible_item_index;

    if(first_visible_item_index < max_items)
    {
        for(int item_index = first_visible_item_index; item_index <= last_visible_item_index; ++item_index)
        {
            item_type& item = external_data.items[item_index];

            if(item.visible)
            {
                update |= item.check_update();
            }
        }
    }
    else
    {
        for(int item_index = 0; item_index < max_items; ++item_index)
        {
            item_type& item = external_data.items[item_index];

            if(item.visible)
            {
                update |= item.check_update();
                first_visible_item_index = min(first_visible_item_index, item_index);
                last_visible_item_index = item_index;
            }
        }

        if(first_visible_item_index == max_items)
        {
            external_data.first_visible_item_index = max_items - 1;
            external_data.last_visible_item_index = 0;
        }
        else
        {
            external_data.first_visible_item_index = int8_t(first_visible_item_index);
            external_data.last_visible_item_index = int8_t(last_visible_item_index);
        }
    }

    if(update)
    {
        hw_entry* entries;
        int entries_count = 0;

        if(external_data.entries_a_active)
        {
            entries = internal_data.entries_b;
            external_data.entries_a_active = false;
        }
        else
        {
            entries = internal_data.entries_a;
            external_data.entries_a_active = true;
        }

        for(int item_index = first_visible_item_index; item_index <= last_visible_item_index; ++item_index)
        {
            const item_type& item = external_data.items[item_index];

            if(item.visible && item.on_screen)
            {
                hw_entry& entry = entries[entries_count];
                item.setup_entry(entry);
                ++entries_count;
            }
        }

        external_data.new_entries_count = int8_t(entries_count);
        external_data.commit = true;
    }
}

void commit()
{
    if(external_data.commit)
    {
        int old_entries_count = external_data.old_entries_count;
        int new_entries_count = external_data.new_entries_count;
        external_data.old_entries_count = external_data.new_entries_count;
        external_data.commit = false;

        if(new_entries_count)
        {
            hw_entry* entries_ptr = external_data.entries_a_active ? internal_data.entries_a : internal_data.entries_b;
            hw::hblank_effects::commit_entries_ptr(entries_ptr);

            if(old_entries_count != new_entries_count)
            {
                hw::hblank_effects::commit_entries_count(new_entries_count);
                external_data.enabled = true;
            }
            else if(! external_data.enabled)
            {
                external_data.enabled = true;
                hw::hblank_effects::enable();
            }
        }
        else
        {
            if(external_data.enabled)
            {
                external_data.enabled = false;
                hw::hblank_effects::disable();
            }
        }
    }
}

}
