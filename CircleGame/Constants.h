#pragma once

#define COMP_DEBUG_VEL 0
#define COMP_DEBUG_VARS 1
#define COMP_DARK_MODE 1

// Minimum distance the player can shoot themselves
constexpr float MIN_SHOOT_DIST = 150;
// Cooldown for shoot and rope
constexpr float TIME_UNTIL_SHOOT = 0.4f;
// Double this number of fallers spawn: one row on screen, one row off screen.
constexpr int NUM_FALLERS = 8;