#include "SoundEngine.h"

const std::string SoundEngine::BACKGROUND_WIND_SOUND = "audio/wind.mp3";
const std::string SoundEngine::PLAYER_LAND_SOUND = "audio/land.mp3";
const std::string SoundEngine::PLAYER_JUMP_SOUND = "audio/jump.mp3";
const std::string SoundEngine::TRIGGER_ACTIVATED_SOUND = "audio/triggerActivated.mp3";
const std::string SoundEngine::PROJECTILE_DISAPPEAR_SOUND = "audio/projectileDisappear.mp3";

irrklang::ISoundEngine *SoundEngine::soundEngine;
bool SoundEngine::soundEnabled = true;

double SoundEngine::soundEndTime;
bool SoundEngine::foregroundSoundPlaying = false;
std::string SoundEngine::currentSound;

void SoundEngine::init() {
    soundEngine = irrklang::createIrrKlangDevice();
    if (!soundEngine) {
        soundEnabled = false;
    }
    if (soundEnabled) {
        std::string windSound = BACKGROUND_WIND_SOUND;
        soundEngine->play2D(windSound.c_str(), true);
    }
}

void SoundEngine::update(float delta, double currentTime) {
    if (soundEngine->isCurrentlyPlaying(currentSound.c_str())) {
        soundEndTime = currentTime;
    } else if (currentTime - soundEndTime >= 5) {
        currentSound = "audio/" + std::to_string(rand() % 7) + ".mp3";
        soundEngine->play2D(currentSound.c_str());
    }
}

void SoundEngine::play2D(std::string soundFilePath) {
    soundEngine->play2D(soundFilePath.c_str());
}

std::string SoundEngine::getRandomFootstepSound() {
    return "audio/steps/" + std::to_string(rand() % 3) + ".mp3";;
}
