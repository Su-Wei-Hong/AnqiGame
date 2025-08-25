#ifndef LEVELSELECTSCENE_HPP
#define LEVELSELECTSCENE_HPP

#include <allegro5/allegro_audio.h>
#include <memory>

#include "Engine/IScene.hpp"
extern int level;
class LevelSelectScene final : public Engine::IScene {
public:
    explicit LevelSelectScene() = default;
    void Initialize() override;
    void Terminate() override;
    void PlayOnClick(int stage);
};
#endif
