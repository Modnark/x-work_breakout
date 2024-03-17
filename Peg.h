#include "SpriteObject.h"

class Peg : public SpriteObject {
    public:
        Peg(Vector2 position);
        void draw() override;
        void update(float delta_time) override;
};