#ifndef RECORDSCENE_HPP
#define RECORDSCENE_HPP

#include <allegro5/allegro_audio.h>
#include <memory>

#include "Engine/IScene.hpp"
class RecordScene final : public Engine::IScene {
private:
    ALLEGRO_SAMPLE_ID bgmId;
public:
    explicit RecordScene() = default;
    void Initialize() override;
    void Update(float deltaTime) override;
    void OnKeyDown(int keyCode) override;
    void Terminate() override;
    void ButtonClick(int id);
    void BackOnClick(int stage);
};
#endif
