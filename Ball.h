#include "SpriteObject.h"

class Ball : public SpriteObject {
    public:
        Ball();
        void draw() override;
        void update(float delta_time) override;
        void event_update(SDL_Event event) override;
        void set_launched(bool launched);
        ~Ball() override;
    private:
        bool launched;
        float get_launch_velocity(BaseObject* paddle);
        void launch();
};