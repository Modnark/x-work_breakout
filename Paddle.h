#include "SpriteObject.h"

class Paddle : public SpriteObject {
    public:
        Paddle();
        void draw() override;
        void update(float delta_time) override;
        void event_update(SDL_Event event) override;
};