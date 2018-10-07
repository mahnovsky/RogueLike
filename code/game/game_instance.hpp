#pragma once

#include "engine.hpp"

class GameInstance
{
public:
    GameInstance();

    void init();

    void draw( IRender* render );

    void frame();

    void cleanup();
};



