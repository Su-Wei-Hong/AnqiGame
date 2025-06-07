#ifndef PlaySCENE_HPP
#define PlaySCENE_HPP

#include <allegro5/allegro_audio.h>
#include <memory>

#include "Engine/IScene.hpp"
class PlayScene final : public Engine::IScene {
public:
    explicit PlayScene() = default;
    void Initialize() override;
    void Terminate() override;
    // void PlayOnClick(int stage);
    // void SettingsOnClick(int stage);
};
#endif
