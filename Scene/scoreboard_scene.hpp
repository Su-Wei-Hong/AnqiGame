#ifndef SCOREBOARDSCENE_HPP
#define SCOREBOARDSCENE_HPP
#include "Engine/IScene.hpp"
#include <allegro5/allegro_audio.h>
#include <vector>
#include <string>
extern std::vector<std::pair<std::string,int>> data;
class ScoreBoardScene final : public Engine::IScene {
private:
    ALLEGRO_SAMPLE_ID bgmId;

public:
    explicit ScoreBoardScene() = default;
    void Initialize() override;
    void Terminate() override;
    void Update(int a);
    void BackOnClick(int stage);
};

#endif   // WINSCENE_HPP
