#include "Ball.h"
#include "Paddle.h"
#include "Peg.h"
#include "Game.h"
#include "Collision.h"
#include "DebugUtils.h"
#include "SoundPlayer.h"

#define BALL_SPEED 500.0f
#define PADDLE_BOUNDS_LEFT_MIN -20
#define PADDLE_BOUNDS_LEFT_MAX 15
#define PADDLE_BOUNDS_LEFT_HALF_MIN -10
#define PADDLE_BOUNDS_LEFT_HALF_MAX 7
#define PADDLE_BOUNDS_RIGHT_MIN -96
#define PADDLE_BOUNDS_RIGHT_MAX -61
#define PADDLE_BOUNDS_RIGHT_HALF_MIN -86
#define PADDLE_BOUNDS_RIGHT_HALF_MAX -69

static Vector2 window_size = Vector2(0, 0);
static Vector2 ball_size = Vector2(16, 16);
static std::vector<BaseObject*> colliders;

int paddle_aim_offset = 0;

void Ball::set_launched(bool launched) {
    this->launched = launched;
}

Ball::Ball() {
    launched = false;
    texture_name = "Resources/png/ball.png";

    window_size = Game::get_instance()->get_window()->get_size();
    position = Vector2((window_size.X / 2) - (ball_size.X / 2), (window_size.Y / 2) - (ball_size.Y / 2));
    size = ball_size;
    velocity = Vector2(0.0f, -BALL_SPEED);

    object_name = "Ball";
    object_class_name = "Ball";
    
    // Cursed
    std::vector<BaseObject*> objects = Game::get_instance()->get_game_objects();
    for(int i = 0; i < objects.size(); i++)
    {
        BaseObject* object = objects.at(i);
        std::string class_name = object->get_class_name();
        if(class_name == "Peg") {
            colliders.push_back(object);
            continue;
        } else if(class_name == "Paddle") {
            colliders.push_back(object);
            continue;
        }
    }

    load_texture();
}

float Ball::get_launch_velocity(BaseObject* paddle) {
    float relative_x = paddle->get_position().X - position.X;
    float new_x = 0.0f;
    if(relative_x >= PADDLE_BOUNDS_LEFT_HALF_MIN && relative_x <= PADDLE_BOUNDS_LEFT_HALF_MAX) {
        new_x = -BALL_SPEED / 2;
    } else if(relative_x >= PADDLE_BOUNDS_RIGHT_HALF_MIN && relative_x <= PADDLE_BOUNDS_RIGHT_HALF_MAX) {
        new_x = BALL_SPEED / 2;
    } else if(relative_x >= PADDLE_BOUNDS_LEFT_MIN && relative_x <= PADDLE_BOUNDS_LEFT_MAX) {
        new_x = -BALL_SPEED;
    } else if(relative_x >= PADDLE_BOUNDS_RIGHT_MIN && relative_x <= PADDLE_BOUNDS_RIGHT_MAX) {
        new_x = BALL_SPEED;
    } else {
        new_x = velocity.X * -1;
    }

    return new_x;    
}

void Ball::launch() {
    launched = true;
    BaseObject* paddle = Game::get_instance()->get_game_object_by_name("Paddle");
    float new_x = get_launch_velocity(paddle);
    velocity.X = new_x;
}

void Ball::event_update(SDL_Event event) {
    if(!launched) {
        switch(event.type) {
            case SDL_MOUSEBUTTONUP:
                if(event.button.button == SDL_BUTTON_LEFT)
                    launch();
                break;       
            case SDL_MOUSEWHEEL:
                if(event.wheel.y > 0) {
                    paddle_aim_offset += 5;
                } else if(event.wheel.y < 0) {
                    paddle_aim_offset -= 5;
                }
                break;
        }
    }
}

void Ball::draw() {
    SpriteObject::draw();
}

void Ball::update(float delta_time) {
    SpriteObject::update(delta_time);

    if(!launched) {
        BaseObject* paddle = Game::get_instance()->get_game_object_by_name("Paddle");
        if(paddle) {
            if(paddle_aim_offset > 40)
                paddle_aim_offset = 40;
            if(paddle_aim_offset < -40)
                paddle_aim_offset = -40;

            position.X = (paddle->get_position().X + (paddle->get_size().X / 2) - ball_size.X / 2) + paddle_aim_offset;
            position.Y = paddle->get_position().Y - paddle->get_size().Y;
        } else {
            DebugUtils::PrintOutput(Console, Info, "[Ball::update] I CAN'T FIND THE PADDLE!\n");
        }
        return;
    }

    for(int i = 0; i < colliders.size(); i++) {
        CollisionDirection direction;
        BaseObject* obj = colliders.at(i);
        std::string class_name = obj->get_class_name();
        
        if(Collision::test(this, obj, &direction, delta_time)) {
            if (class_name == "Peg") {
                Game::get_instance()->remove_game_object(obj);
                colliders.erase(colliders.begin() + i);
                SoundPlayer::get_instance()->play_sfx("Resources/wav/hit.wav", -1, 0);
            } else if(class_name == "Paddle") {
                SoundPlayer::get_instance()->play_sfx("Resources/wav/hit_paddle.wav", 1, 0);
            }
                
            if(class_name == "Paddle") {
                float new_x = get_launch_velocity(obj);

                switch(direction) {
                    case COLLISION_TOP:
                        velocity.X = new_x;
                        velocity.Y = -BALL_SPEED;
                        break;
                    case COLLISION_LEFT:
                        velocity.X = -BALL_SPEED;
                        velocity.Y = BALL_SPEED;
                        break;
                    case COLLISION_RIGHT:
                        velocity.X = BALL_SPEED;
                        velocity.Y = BALL_SPEED;
                        break;
                }
            } else {
                switch(direction) {
                    case COLLISION_TOP:
                        velocity.Y = -BALL_SPEED;
                        break;
                    case COLLISION_BOTTOM:
                        velocity.Y = BALL_SPEED;
                        break;
                    case COLLISION_LEFT:
                        velocity.X = BALL_SPEED;
                        break;
                    case COLLISION_RIGHT:
                        velocity.X = -BALL_SPEED;
                        break;
                }
            }
        }
    }

    // Keep within screen
    if(position.X <= 0)
        velocity.X = fabs(velocity.X);
    
    if(position.X >= window_size.X - ball_size.X)
        velocity.X = -fabs(velocity.X);

    if(position.Y <= 0)
        velocity.Y = fabs(velocity.Y);

    position.X += (velocity.X * delta_time);
    position.Y += (velocity.Y * delta_time); 
}

Ball::~Ball() {
    colliders.clear();
}