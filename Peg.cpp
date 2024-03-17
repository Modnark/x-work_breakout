#include "Peg.h"

Peg::Peg(Vector2 position) {
    texture_name = "Resources/png/peg.png";
    size = Vector2(64,16);
    this->position = position;

    object_name = "Peg";
    object_class_name = "Peg";

    load_texture();
}

void Peg::draw() { SpriteObject::draw(); }

void Peg::update(float delta_time) {
    SpriteObject::update(delta_time);
}