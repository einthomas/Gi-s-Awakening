#pragma once

#include <irrKlang.h>
#include <string>

class SoundEngine {
public:
    static const std::string BACKGROUND_WIND_SOUND;
    static const std::string PLAYER_LAND_SOUND;
    static const std::string PLAYER_JUMP_SOUND;
    static const std::string TRIGGER_ACTIVATED_SOUND;
    static const std::string PROJECTILE_DISAPPEAR_SOUND;

    static double soundEndTime;

    static void init();
    static void update(float delta, double currentTime);
    static void play2D(std::string soundFilePath);
    static std::string getRandomFootstepSound();

private:
    static irrklang::ISoundEngine *soundEngine;
    static bool soundEnabled;

    static bool foregroundSoundPlaying;
    static std::string currentSound;
};
