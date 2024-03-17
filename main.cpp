#include <stdio.h>

#include "Game.h"
#include "StringUtils.h"
#include "DebugUtils.h"
#include "Vector2.h"
#include "Color3.h"

#include "TextObject.h"
#include "SpriteObject.h"
#include "SoundPlayer.h"

#include "Paddle.h"
#include "Ball.h"
#include "Peg.h"

#define VERSION "X-Work a1.1.2"

// flags
bool won = false;
bool lost = false;
bool respawning = false;

Uint32 respawn_start_ticks;
Uint32 respawn_elapsed_ticks;
float respawn_delta;
const Uint32 respawn_delay = 3000;

Vector2 window_size = Vector2(1024, 768);

int ball_count = 4;
int peg_width = 64;
int peg_x_count = 10;
int total_width = peg_x_count * peg_width;

void setup() {
    Game* game = Game::get_instance();
    
    // Create game board
    for(int i = 0; i < 10; i++) {
        Color3 peg_color = Color3(255, 0, 255);
        int x_start = (window_size.X - total_width) / 2;
        int x_pos = (x_start + 64 * i);
        
        for(int x = 0; x < 4; x++) {
            int y_start = 72; 
            int y_pos = (y_start + 16 * x);
            switch(x) {
                case 1: peg_color = Color3(255, 0, 0); break;
                case 2: peg_color = Color3(0, 255, 0); break;
                case 3: peg_color = Color3(0, 0, 255); break;
            }

            Peg* peg = new Peg(Vector2(x_pos, y_pos));
            peg->set_color_mod(peg_color);
            game->add_game_object(peg);
        }
    }
}

void win(Ball* ball, Paddle* paddle, TextObject* win_text_obj) {
    won = true;
    // win condition
    ball->set_active(false);
    paddle->set_active(false);
    win_text_obj->set_visible(true);

    SoundPlayer::get_instance()->play_sfx("Resources/wav/tada.wav", -1, 0);
}

void lose(Ball* ball, Paddle* paddle, TextObject* lose_text_obj) {
    lost = true;
    // win condition
    ball->set_active(false);
    paddle->set_active(false);
    lose_text_obj->set_visible(true);

    SoundPlayer::get_instance()->play_sfx("Resources/wav/lose.wav", -1, 0);
}

void check_ball_state(Ball* ball) {
    if(!lost) {
        Game* game = Game::get_instance();
        Vector2 ball_pos = ball->get_position();
        if(ball_pos.Y >= window_size.Y) {
            if(!respawning) {
                respawning = true;
                respawn_start_ticks = SDL_GetTicks();
                BaseObject* obj = game->get_game_object_by_name(StringUtils::Format("LIFE_BALL_%d", ball_count - 1));
                if(obj)
                    game->remove_game_object(obj);
                ball_count -= 1;
                ball->set_active(false);
                ball->set_visible(false);
                ball->set_velocity(Vector2(0, 500));
                ball->set_launched(false);
                SoundPlayer::get_instance()->play_sfx("Resources/wav/lose_life.wav", -1, 0);
            }

            respawn_elapsed_ticks = SDL_GetTicks() - respawn_start_ticks;
            if(respawn_elapsed_ticks >= respawn_delay) {
                respawning = false;
                Vector2 ball_size = ball->get_size();
                ball->set_position(Vector2((window_size.X / 2) - (ball_size.X / 2), (window_size.Y / 2) - (ball_size.Y / 2)));
                ball->set_active(true);
                ball->set_visible(true);
            }
        }
    }
}

int main(int, char **) {
    DebugUtils::SetConsoleTitle("X-WORK - Debug Console");

    std::string window_title = "X-WORK - Breakout Demo";
    
    // Init SoundSystem (yes it's a bit scuffed)
    SoundPlayer::get_instance();

    // Setup Game & GameWindow
    Game* game = Game::get_instance();
    game->create_window(window_title, window_size);
    game->get_window()->show(true);

    // Setup Debug GUI objects
    std::string debug_text = "%s\nX-Work: Breakout Demo\nFPS: %d\nPegs Left: %d\n";
    TextObject* debug_text_obj = new TextObject("idk", 16, Vector2(0.0f, 0.0f), "Resources/fonts/c64.ttf");
    game->add_game_object(debug_text_obj);

    // Setup win / lose messages
    std::string win_text = "You Win!";
    TextObject* win_text_obj = new TextObject(win_text, 32, Vector2(0.0f, 0.0f), "Resources/fonts/c64.ttf");
    Vector2 text_size = win_text_obj->get_size();
    win_text_obj->set_position(Vector2((window_size.X - text_size.X) / 2, (window_size.Y - text_size.Y) / 2));
    win_text_obj->set_visible(false);
    game->add_game_object(win_text_obj);

    std::string lose_text = "You Lose!";
    TextObject* lose_text_obj = new TextObject(lose_text, 32, Vector2(0.0f, 0.0f), "Resources/fonts/c64.ttf");
    Vector2 l_text_size = lose_text_obj->get_size();
    lose_text_obj->set_position(Vector2((window_size.X - l_text_size.X) / 2, (window_size.Y - l_text_size.Y) / 2));
    lose_text_obj->set_visible(false);
    game->add_game_object(lose_text_obj);

    // Setup bottom-left ball count
    for(int i = 0; i < 3; i++) {
        Vector2 ball_size = Vector2(16, 16);
        Vector2 position = Vector2((ball_size.X + 4) * i + 4, (window_size.Y - 4) - ball_size.Y);
        SpriteObject* spobj = new SpriteObject("Resources/png/ball.png", position, ball_size);
        spobj->set_name(StringUtils::Format("LIFE_BALL_%d", i));
        game->add_game_object(spobj);        
    }

    // Setup game objects
    Paddle* paddle = new Paddle();
    game->add_game_object(paddle);

    // Setup game board
    setup();
    
    Ball* ball = new Ball();
    game->add_game_object(ball);

    // Tell the game we can run
    game->run();

    // Main loop
    while(game->is_running()) {
        int number_of_pegs = game->get_game_objects_by_class_name("Peg").size();
        debug_text_obj->set_content(StringUtils::Format(debug_text.c_str(), VERSION, game->get_fps(), number_of_pegs));
        
        // Lose life
        check_ball_state(ball);

        // Win game
        if(number_of_pegs == 0) {
            if(!won) 
                win(ball, paddle, win_text_obj);
        }

        // Lose game
        if(ball_count == 0) {
            if(!lost)
                lose(ball, paddle, lose_text_obj);
        }
        
        game->step();
    }

    game->destroy_instance();
    return 0;
}