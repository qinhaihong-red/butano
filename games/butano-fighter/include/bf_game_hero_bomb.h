#ifndef BF_GAME_HERO_BOMB_H
#define BF_GAME_HERO_BOMB_H

#include "btn_optional.h"
#include "btn_blending_actions.h"
#include "btn_regular_bg_actions.h"
#include "btn_rect_window_actions.h"
#include "btn_regular_bg_hblank_effects.h"

namespace bf
{

class game_hero;
class game_background;

class game_hero_bomb
{

public:
    game_hero_bomb();

    game_hero_bomb(const game_hero_bomb& other) = delete;

    game_hero_bomb& operator=(const game_hero_bomb& other) = delete;

    void update(game_hero& hero, game_background& background);

private:
    enum class status_type
    {
        INACTIVE,
        OPEN,
        CLOSE
    };

    btn::regular_bg_ptr _bg;
    btn::regular_bg_move_by_action _bg_move_action;
    btn::optional<btn::rect_window_move_top_by_action> _move_window_top_action;
    btn::optional<btn::rect_window_move_bottom_by_action> _move_window_bottom_action;
    btn::optional<btn::blending_transparency_alpha_to_action> _blending_action;
    btn::fixed _hblank_effect_deltas[160];
    btn::regular_bg_position_hblank_effect_ptr _hblank_effect;
    status_type _status = status_type::INACTIVE;
    int _counter = 0;
};

}

#endif