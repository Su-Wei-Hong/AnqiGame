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

#include "Scene/anqi_setting_scene.hpp"
#include "UI/Component/ImageButton.hpp"
#include "UI/Component/Label.hpp"
#include "UI/Component/Slider.hpp"
#include "anqi_scene.hpp"
#include <cmath>
int decide = 0;
int decide2 = 1;
int x1,x2,x3;
int y_1,y_2,y_3;
int pointer_tick = -45;

    int tempx = 2050;
Engine::Image *pointer1;
Engine::Image *pointer2;
std::string close_img = "unknown.png";
std::string frame_img = "frame.png";
void AnqiSettingScene::Update(float deltaTime){
    int x,y;
    if(decide2==1)
        y = y_1; 
    else if(decide2==2)
        y = y_2;
    else
        y = y_3;


    if(decide==0){
        close_img = "unknown.png";
        frame_img = "frame.png";
        x = x1;
    }
    else if(decide==2){
        close_img = "unknown2.png";
        frame_img = "frame.png";
        x = x2;
    }
    else{
        close_img = "unknown3.png";
        frame_img = "frame.png";
        x = x3;
    }

    pointer1->Position.x = x + 10*sin(2*pi*pointer_tick/45);
    pointer2->Position.x = tempx + 10*sin(2*pi*pointer_tick/45);
    pointer2->Position.y = y;
    pointer_tick++;
    if(pointer_tick>45){
        pointer_tick = -45;
    }
}
void AnqiSettingScene::Initialize() {
    pointer_tick = -45;
    int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
    int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
    int halfW = w / 2;
    int halfH = h / 2;
    Engine::ImageButton *btn;
    // btn = new Engine::ImageButton("block6.png", "block6.png", halfW - 450, 200, 900, 200);
    // btn->SetOnClickCallback(std::bind(&SettingScene::PlayOnClick, this, 0));
    // AddNewControlObject(btn);
    //AddNewObject(new Engine::Label("開始", "font2.ttc", 140, halfW, 880, 255,255,255 , 255, 0.5, 0.5));
    AddNewObject(new Engine::Label("請選擇棋子樣式與對戰音樂", "font2.ttc", 150, halfW, halfH / 4, 255, 255, 255, 255, 0.5, 0.5));

    AddNewObject(new Engine::Image("black_knight.png", 400, 450 , 300 , 300 , 0, 0));
    AddNewObject(new Engine::Image("red_knight.png",400, 800 , 300 , 300 , 0, 0));
    
    AddNewObject(new Engine::Image("black_king3.png", 1600, 450 , 300 , 300 , 0, 0));
    AddNewObject(new Engine::Image("red_king3.png", 1600, 800 , 300 , 300 , 0, 0));

    AddNewObject(new Engine::Image("black_elephant2.png", 1000, 450 , 300 , 300 , 0, 0));
    AddNewObject(new Engine::Image("red_elephant2.png", 1000, 800 , 300 , 300 , 0, 0));
    x1 = 190;
    x2 = 790;
    x3 = 1390;
    y_1 = 375;
    y_2 = 775;
    y_3 = 1175;
    pointer1 = new Engine::Image("pointer2.png",x1 , 1225 , 200 , 200 , 0, 0);
    AddNewObject(pointer1);
    pointer2 = new Engine::Image("pointer2.png",tempx , y_1 , 200 , 200 , 0, 0);
    AddNewObject(pointer2);
    // auto font = al_load_font("pirulen.ttf", 120, 0);
    // if (!font) std::cerr << "Font load failed!" << std::endl;

    btn = new Engine::ImageButton("block2.png", "block5.png", 400, 1250, 300,150);
    btn->SetOnClickCallback(std::bind(&AnqiSettingScene::SettingsOnClick, this, 0));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("選擇", "font2.ttc", 100, 550, 1325, 255,255,255 , 255, 0.5, 0.5));

    btn = new Engine::ImageButton("block2.png", "block5.png", 1600, 1250, 300,150);
    btn->SetOnClickCallback(std::bind(&AnqiSettingScene::SettingsOnClick, this, 3));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("選擇", "font2.ttc", 100, 1750, 1325, 255,255,255, 255, 0.5, 0.5));

    btn = new Engine::ImageButton("block2.png", "block5.png", 1000, 1250, 300,150);
    btn->SetOnClickCallback(std::bind(&AnqiSettingScene::SettingsOnClick, this, 2));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("選擇", "font2.ttc", 100, 1150, 1325, 255,255,255, 255, 0.5, 0.5));

    btn = new Engine::ImageButton("block2.png", "block5.png", 2250, 400, 300,150);
    btn->SetOnClickCallback(std::bind(&AnqiSettingScene::SettingsOnClick, this, 4));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("刺激", "font2.ttc", 100, 2400, 475, 255,255,255, 255, 0.5, 0.5));

    btn = new Engine::ImageButton("block2.png", "block5.png", 2250, 800, 300,150);
    btn->SetOnClickCallback(std::bind(&AnqiSettingScene::SettingsOnClick, this, 5));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("柔和", "font2.ttc", 100, 2400, 875, 255,255,255, 255, 0.5, 0.5));

    btn = new Engine::ImageButton("block2.png", "block5.png", 2250, 1200, 300,150);
    btn->SetOnClickCallback(std::bind(&AnqiSettingScene::SettingsOnClick, this, 6));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("流行", "font2.ttc", 100, 2400, 1275, 255,255,255, 255, 0.5, 0.5));

    
    // ALLEGRO_MOUSE_STATE mouse;
    // al_get_mouse_state(&mouse);
    // bool mouseNowDown = al_mouse_button_down(&mouse, 1);
    // mouseJustClicked = (mouseNowDown && !lastMouseDown);  // Rising edge
    // lastMouseDown = mouseNowDown;

    btn = new Engine::ImageButton("block2.png", "block5.png", 2466, 1400, 200, 100);
    btn->SetOnClickCallback(std::bind(&AnqiSettingScene::BackOnClick, this, 1));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("開始", "font2.ttc", 50, 2566, 1450, 255,255,255 , 255, 0.5, 0.5));
    bgmId = AudioHelper::PlayBGM("setting_music.mp3");
}
void AnqiSettingScene::Terminate() {
    AudioHelper::StopBGM(bgmId);
    IScene::Terminate();
}
void AnqiSettingScene::SettingsOnClick(int id) {
    if(id<=3)
        decide = id;
    else
        decide2 = id-3;
    
}
void AnqiSettingScene::BackOnClick(int stage) {
    Engine::GameEngine::GetInstance().ChangeScene("mode_select");
}