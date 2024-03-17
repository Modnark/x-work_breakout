#include "Paddle.h"
#include "Game.h"

static Vector2 window_size = Vector2(0, 0);
static Vector2 paddle_size = Vector2(96, 16);

bool key_states[512];

Paddle::Paddle() {
    for(int i = 0; i < 512; i++)
        key_states[i] = false;

    texture_name = "Resources/png/paddle.png";
    window_size = Game::get_instance()->get_window()->get_size();
    position = Vector2((window_size.X / 2) - (paddle_size.X / 2), window_size.Y - 100);
    size = paddle_size;

    object_name = "Paddle";
    object_class_name = "Paddle";

    load_texture();
}

void Paddle::event_update(SDL_Event event) {}

void Paddle::draw() { SpriteObject::draw(); }

void Paddle::update(float delta_time) {
    SpriteObject::update(delta_time);

    int mouseX, mouseY;
    SDL_GetMouseState(&mouseX, &mouseY);
    position.X = mouseX - (paddle_size.X / 2);

    // Collision with wall
    if(position.X > window_size.X - paddle_size.X)
        position.X = (window_size.X - paddle_size.X) + 1.0f;

    if(position.X < 0)
        position.X = 0;
}