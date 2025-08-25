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

#include "Scene/black_win_scene.hpp"
#include "UI/Component/ImageButton.hpp"
#include "UI/Component/Label.hpp"
#include "UI/Component/Slider.hpp"
#include "anqi_scene.hpp"

void Black_Win_Scene::Initialize(){
    int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
    int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
    int halfW = w / 2;
    int halfH = h / 2;
    Engine::ImageButton* btn;

    
    AddNewObject(new Engine::Label("<< 黑方勝利 >>", "font2.ttc", 150, halfW, 150, 116, 96, 182, 255, 0.5, 0.5));
    AddNewObject(new Engine::Image("black_win.png", 1333-450 , 550 , 900 , 864 , 0, 0));
    AddNewObject(new Engine::Label("分數："+std::to_string(score), "font2.ttc", 100, halfW, 350 , 0, 128, 255, 255, 0.5, 0.5));

    btn = new Engine::ImageButton("block.png", "start/dirt.png", w - 300, halfH *3 / 2 + 200, 200, 100);
    btn->SetOnClickCallback(std::bind(&Black_Win_Scene::BackOnClick, this, 1));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("返回", "font2.ttc", 70, w-200, halfH *3 / 2 + 250, 0,0,102 , 255, 0.5, 0.5));
    

    btn = new Engine::ImageButton("block.png", "start/dirt.png", 100, halfH *3 / 2 + 200, 200, 100);
    btn->SetOnClickCallback(std::bind(&Black_Win_Scene::BackOnClick, this, 2));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("紀錄", "font2.ttc", 70, 200, halfH *3 / 2 + 250, 0,0,102 , 255, 0.5, 0.5));
    bgmId = AudioHelper::PlayBGM("black_win_music.ogg");

}

void Black_Win_Scene::Terminate(){
    AudioHelper::StopBGM(bgmId);
    IScene::Terminate();
    
}
void Black_Win_Scene::BackOnClick(int stage){
    if(stage==1)
        Engine::GameEngine::GetInstance().ChangeScene("anqi_start");
    else 
        Engine::GameEngine::GetInstance().ChangeScene("record");
}