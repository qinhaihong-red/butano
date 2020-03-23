#ifndef BF_GAME_BACKGROUND_H
#define BF_GAME_BACKGROUND_H

#include "btn_optional.h"
#include "btn_blending_actions.h"
#include "btn_regular_bg_actions.h"
#include "btn_bgs_mosaic_actions.h"
#include "btn_regular_bg_hblank_effects.h"
#include "bf_wave_generator.h"

namespace bf
{

class game_background
{

public:
    game_background();

    game_background(const game_background& other) = delete;

    game_background& operator=(const game_background& other) = delete;

    void show_mosaic();

    void hide_mosaic();

    void show_blending();

    void hide_blending();

    void show_hblank_effect(int frames);

    void update();

private:
    btn::regular_bg_move_by_action _ground_bg_move_action;
    btn::regular_bg_move_by_action _clouds_bg_move_action;
    btn::optional<btn::bgs_mosaic_stretch_loop_action> _mosaic_action;
    btn::optional<btn::blending_transparency_alpha_to_action> _blending_action;
    btn::fixed _hblank_effect_deltas[160];
    btn::regular_bg_position_hblank_effect_ptr _hblank_effect;
    wave_generator _wave_generator;
    int _hblank_effect_frames = 0;
};

}

#endif