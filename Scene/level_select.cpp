#include <allegro5/allegro_audio.h>
#include <functional>
#include <memory>
#include <string>
#include <ostream>
#include <iostream>
#include "Engine/AudioHelper.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Point.hpp"
#include "Engine/Resources.hpp"

#include "Scene/anqi_scene.hpp"
#include "UI/Component/ImageButton.hpp"
#include "UI/Component/Label.hpp"
#include "UI/Component/Slider.hpp"
#include "level_select.hpp"

int level;
void LevelSelectScene::Initialize(){
    int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
    int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
    int halfW = w / 2;
    int halfH = h / 2;
    Engine::ImageButton *btn;
    btn = new Engine::ImageButton("start/block.png", "start/dirt.png", halfW - 400, halfH / 2 -200, 800, 200);
    btn->SetOnClickCallback(std::bind(&LevelSelectScene::PlayOnClick, this, 2));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("易", "font2.ttc", 120, halfW, halfH / 2 - 100, 0, 0, 102, 255, 0.5, 0.5));

    btn = new Engine::ImageButton("start/block.png", "start/dirt.png", halfW - 400, halfH / 2 + 250, 800, 200);
    btn->SetOnClickCallback(std::bind(&LevelSelectScene::PlayOnClick, this, 4));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("中", "font2.ttc", 120, halfW, halfH / 2 + 350, 0, 0, 102, 255, 0.5, 0.5));

    // btn = new Engine::ImageButton("start/block.png", "start/dirt.png", halfW - 400, halfH / 2 + 300, 800, 200);
    // btn->SetOnClickCallback(std::bind(&StartScene::PlayOnClick, this, 1));
    // AddNewControlObject(btn);
    // AddNewObject(new Engine::Label("開始", "font2.ttc", 140, halfW, halfH / 2 + 400, 0, 0, 102, 255, 0.5, 0.5));

    btn = new Engine::ImageButton("start/block.png", "start/dirt.png", halfW - 400, halfH / 2 + 700, 800, 200);
    btn->SetOnClickCallback(std::bind(&LevelSelectScene::PlayOnClick, this, 6));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("難", "font2.ttc", 120, halfW, halfH / 2 + 800, 0, 0, 102, 255, 0.5, 0.5));


}
void LevelSelectScene::Terminate(){
    IScene::Terminate();
}
void LevelSelectScene::PlayOnClick(int stage){
    level = stage;
    Engine::GameEngine::GetInstance().ChangeScene("play");
}