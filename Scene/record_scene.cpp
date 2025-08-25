#include "record_scene.hpp"
#include "UI/Component/ImageButton.hpp"
#include "UI/Component/Label.hpp"
#include "Engine/GameEngine.hpp"
#include <iostream>
#include <allegro5/allegro.h>
#include <functional>
#include <string>
#include <memory>
#include <ostream>
#include <fstream>
#include <sstream>
#include <map>
#include "Engine/AudioHelper.hpp"   
#include "Engine/Point.hpp"
#include "Engine/Resources.hpp"
#include "anqi_scene.hpp"
#include "Scene/scoreboard_scene.hpp"
Engine::ImageButton *btn;
Engine::ImageButton *btn2;
Engine::Label *text;
Engine::Label *buttontext1;
Engine::Label *buttontext2;
Engine::Label *account;
Engine::Label *password;
Engine::Label *password2;
Engine::Label *result;
std::string name;
std::string secret;
std::string hide;
std::string secret2;
std::string hide2;
std::string show_or_hide;
bool visible;
bool visible2;
int mode = 0;
int small_or_big = 0;
int registering = 0;
int result_tick = 0;
int result_mark = 0;
Engine::Image *img2;
Engine::Label *sign2;
std::map<std::string, std::string> nameList;
void RecordScene::Initialize(){
    result_tick = 0;
    result_mark = 0;
    registering = 0;
    nameList.clear();
    visible = false;
    visible2 = false;
    int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
    small_or_big = 0;
    name = "";
    secret = "";
    hide = "";
    show_or_hide = "顯示";
    Engine::Image *img;
    Engine::Label *sign = new Engine::Label("請輸入帳號密碼", "font2.ttc", 120, 1333, 100, 255 , 255 ,255 , 255, 0.5, 0.5);
    AddNewObject(sign);
    sign  = new Engine::Label("(Ctrl切換大小寫,輸入完畢按Enter)", "font2.ttc", 50, 1333, 250, 255 , 255 ,255 , 255, 0.5, 0.5);
    AddNewObject(sign);
    
    text = new Engine::Label("帳號：", "font2.ttc", 70, 850, 500, 255 , 255 ,255 , 255, 1, 0.5);
    AddNewObject(text);
    //AddNewObject(new Engine::Image("red_win.png", 1333-450 , 550 , 900 , 864 , 0, 0));
    img = new Engine::Image("block.png",1333-420,465,840,70,0,0);
    AddNewObject(img);


    text = new Engine::Label("密碼：", "font2.ttc", 70, 850, 750, 255 , 255 ,255 , 255, 1, 0.5);
    AddNewObject(text);
    img = new Engine::Image("block.png",1333-420,715,840,70,0,0);
    AddNewObject(img);

    

    
    btn = new Engine::ImageButton("start/floor.png", "start/floor.png", 1333+420-100, 715, 100, 70);
    btn->SetOnClickCallback(std::bind(&RecordScene::ButtonClick, this, 0));
    AddNewControlObject(btn);
    buttontext1 = new Engine::Label("顯示", "font2.ttc", 40, 1333+420-50, 750, 0 , 0 ,0 , 255, 0.5, 0.5);
    AddNewObject(buttontext1);

    result = new Engine::Label("", "font2.ttc", 70, 1333, 365, 204 , 0 ,0 , 255, 0.5, 0.5);
    AddNewObject(result);
    

    btn = new Engine::ImageButton("block.png", "start/dirt.png", 2666 - 300, 750 *3 / 2 + 200, 200, 100);
    btn->SetOnClickCallback(std::bind(&RecordScene::BackOnClick, this, 1));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("返回", "font2.ttc", 70, 2666-200, 750 *3 / 2 + 250, 0,0,102 , 255, 0.5, 0.5));

    btn = new Engine::ImageButton("block.png", "start/dirt.png", 1000, 1200, 200, 100);
    btn->SetOnClickCallback(std::bind(&RecordScene::ButtonClick, this, 2));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("登入", "font2.ttc", 70, 1100,1250, 0,0,102 , 255, 0.5, 0.5));

    btn = new Engine::ImageButton("block.png", "start/dirt.png", 1480, 1200, 200, 100);
    btn->SetOnClickCallback(std::bind(&RecordScene::ButtonClick, this, 3));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("註冊", "font2.ttc", 70, 1580, 1250, 0,0,102 , 255, 0.5, 0.5));


    account = new Engine::Label(name, "font2.ttc", 40, 1333-420, 500, 0,0,0 , 255, 0, 0.5);
    AddNewObject(account);
    password = new Engine::Label(hide, "font2.ttc", 40, 1333-420, 750, 0,0,0 , 255, 0, 0.5);
    AddNewObject(password);

    std::ifstream fin("Resource/database.txt");
    std::string passWord;
    if (!fin.is_open()) {
        std::cout << "Failed to open database.txt\n";
        return;
    }

    while (fin>>name>>passWord) {
        nameList[name] = passWord;
    }
    name = "";
    bgmId = AudioHelper::PlayBGM("record_music.ogg");
}
bool Collide(float startX,float startY,float desX,float desY){
    ALLEGRO_MOUSE_STATE mouse;
    al_get_mouse_state(&mouse);
    float x  = Engine::GameEngine::GetInstance().GetMousePosition().x;
    float y  = Engine::GameEngine::GetInstance().GetMousePosition().y;
    if(x>=startX&&x<desX&&y>=startY&&y<=desY)
        return true;
    else
        return false;
}
void RecordScene::OnKeyDown(int keyCode) {
    IScene::OnKeyDown(keyCode);
    if(keyCode==ALLEGRO_KEY_ENTER&&registering&&name.size()>0){
        // Registration successful
        if(name.size()==0){ 
            result->Text = "名稱不可為空白！";
            result_mark = 1;
        }
        else if(secret.size()==0||secret2.size()==0){
            result->Text = "密碼不可為空白！";
            result_mark = 1;
        }
        else if(nameList.find(name) != nameList.end()){
            result->Text = "該名稱已被使用！";
            result_mark = 1;
        }
        else if(secret2.size()<6){
            result->Text = "密碼須大於等於6字元！";
            result_mark = 1;
        }
        else if(secret2==secret){
            result->Text = "註冊成功！";
            result_mark = 1;
            nameList[name] = secret;
            std::ofstream out("Resource/database.txt", std::fstream::app);
            out << std::endl << name << " " << secret2;
            out.close();
            name = "";
            secret = "";
            secret2 = "";
            hide = "";
            hide2 = "";
        }
        else{
            result->Text = "密碼不一致！";
            result_mark = 1;
        }
       
    }
    else if(keyCode==ALLEGRO_KEY_ENTER&&!registering){
        if(name.size()==0){
            result->Text = "名稱不可為空白！";
            result_mark = 1;
        }
        else if(secret.size()==0){
            result->Text = "密碼不可為空白！";
            result_mark = 1;
        }
        else if(secret.size()<6){
            result->Text = "密碼須大於等於6字元！";
            result_mark = 1;
        }
        else if(nameList.find(name) != nameList.end()) {
            if (nameList[name] == secret) {
                data.push_back({name,score});
                std::ofstream out("Resource/scoreboard.txt",std::fstream::app);
                out << std::endl << name << " "<< score ;
                out.close();
                result->Text = "登入成功！";
                result_mark = 2;
                
                
            } else {
                result->Text = "密碼錯誤！";
                result_mark = 1;
            }
        } 
        else {
            result->Text = "帳號不存在！";
            result_mark = 1;
        }
    }
    else if(keyCode==ALLEGRO_KEY_LCTRL||keyCode==ALLEGRO_KEY_RCTRL)
        small_or_big = !small_or_big;
    
    else if(Collide(1000,465,1680,535)){
        if(keyCode==ALLEGRO_KEY_BACKSPACE){
            if(name.size()>0)
                name.pop_back();
            
        }
        else if(name.size()<8){
            if(keyCode==ALLEGRO_KEY_MINUS)
                name+='_';
            else if(keyCode==ALLEGRO_KEY_0)
                name+='0';
            else if(keyCode==ALLEGRO_KEY_1)
                name+='1';
            else if(keyCode==ALLEGRO_KEY_2)
                name+='2';
            else if(keyCode==ALLEGRO_KEY_3)
                name+='3';
            else if(keyCode==ALLEGRO_KEY_4)
                name+='4';
            else if(keyCode==ALLEGRO_KEY_5)
                name+='5';
            else if(keyCode==ALLEGRO_KEY_6)
                name+='6';
            else if(keyCode==ALLEGRO_KEY_7)
                name+='7';
            else if(keyCode==ALLEGRO_KEY_8)
                name+='8';
            else if(keyCode==ALLEGRO_KEY_9)
                name+='9';
            else if(!small_or_big){//small
                if(keyCode==ALLEGRO_KEY_A)
                    name+='a';
                else if(keyCode==ALLEGRO_KEY_B)
                    name+='b';
                else if(keyCode==ALLEGRO_KEY_C)
                    name+='c';
                else if(keyCode==ALLEGRO_KEY_D)
                    name+='d';
                else if(keyCode==ALLEGRO_KEY_E)
                    name+='e';
                else if(keyCode==ALLEGRO_KEY_F)
                    name+='f';
                else if(keyCode==ALLEGRO_KEY_G)
                    name+='g';
                else if(keyCode==ALLEGRO_KEY_H)
                    name+='h';
                else if(keyCode==ALLEGRO_KEY_I)
                    name+='i';
                else if(keyCode==ALLEGRO_KEY_J)
                    name+='j';
                else if(keyCode==ALLEGRO_KEY_K)
                    name+='k';
                else if(keyCode==ALLEGRO_KEY_L)
                    name+='l';
                else if(keyCode==ALLEGRO_KEY_M)
                    name+='m';
                else if(keyCode==ALLEGRO_KEY_N)
                    name+='n';
                else if(keyCode==ALLEGRO_KEY_O)
                    name+='o';
                else if(keyCode==ALLEGRO_KEY_P)
                    name+='p';
                else if(keyCode==ALLEGRO_KEY_Q)
                    name+='q';
                else if(keyCode==ALLEGRO_KEY_R)
                    name+='r';
                else if(keyCode==ALLEGRO_KEY_S)
                    name+='s';
                else if(keyCode==ALLEGRO_KEY_T)
                    name+='t';
                else if(keyCode==ALLEGRO_KEY_U)
                    name+='u';
                else if(keyCode==ALLEGRO_KEY_V)
                    name+='v';
                else if(keyCode==ALLEGRO_KEY_W)
                    name+='w';
                else if(keyCode==ALLEGRO_KEY_X)
                    name+='x';
                else if(keyCode==ALLEGRO_KEY_Y)
                    name+='y';
                else if(keyCode==ALLEGRO_KEY_Z)
                    name+='z';
            }
            else{
                if(keyCode==ALLEGRO_KEY_A)
                    name+='A';
                else if(keyCode==ALLEGRO_KEY_B)
                    name+='B';
                else if(keyCode==ALLEGRO_KEY_C)
                    name+='C';
                else if(keyCode==ALLEGRO_KEY_D)
                    name+='D';
                else if(keyCode==ALLEGRO_KEY_E)
                    name+='E';
                else if(keyCode==ALLEGRO_KEY_F)
                    name+='F';
                else if(keyCode==ALLEGRO_KEY_G)
                    name+='G';
                else if(keyCode==ALLEGRO_KEY_H)
                    name+='H';
                else if(keyCode==ALLEGRO_KEY_I)
                    name+='I';
                else if(keyCode==ALLEGRO_KEY_J)
                    name+='J';
                else if(keyCode==ALLEGRO_KEY_K)
                    name+='K';
                else if(keyCode==ALLEGRO_KEY_L)
                    name+='L';
                else if(keyCode==ALLEGRO_KEY_M)
                    name+='M';
                else if(keyCode==ALLEGRO_KEY_N)
                    name+='N';
                else if(keyCode==ALLEGRO_KEY_O)
                    name+='O';
                else if(keyCode==ALLEGRO_KEY_P)
                    name+='P';
                else if(keyCode==ALLEGRO_KEY_Q)
                    name+='Q';
                else if(keyCode==ALLEGRO_KEY_R)
                    name+='R';
                else if(keyCode==ALLEGRO_KEY_S)
                    name+='S';
                else if(keyCode==ALLEGRO_KEY_T)
                    name+='T';
                else if(keyCode==ALLEGRO_KEY_U)
                    name+='U';
                else if(keyCode==ALLEGRO_KEY_V)
                    name+='V';
                else if(keyCode==ALLEGRO_KEY_W)
                    name+='W';
                else if(keyCode==ALLEGRO_KEY_X)
                    name+='X';
                else if(keyCode==ALLEGRO_KEY_Y)
                    name+='Y';
                else if(keyCode==ALLEGRO_KEY_Z)
                    name+='Z'; 
            }

        }
    }
    else if(Collide(1000,715,1580,785)){
        if(keyCode==ALLEGRO_KEY_BACKSPACE){
            if(secret.size()>0){
                hide.pop_back();
                secret.pop_back();
            }
        }
        else if(secret.size()<20){
            if(keyCode==ALLEGRO_KEY_0){
                hide += '*';
                secret+='0';
            }
            else if(keyCode==ALLEGRO_KEY_1){
                hide += '*';
                secret+='1';
            }
            else if(keyCode==ALLEGRO_KEY_2){
                hide += '*';
                secret+='2';
            }
            else if(keyCode==ALLEGRO_KEY_3){
                hide += '*';                    
                secret+='3';
            }
            else if(keyCode==ALLEGRO_KEY_4){
                hide += '*';
                secret+='4';
            }
            else if(keyCode==ALLEGRO_KEY_5){
                hide += '*';
                secret+='5';
            }
            else if(keyCode==ALLEGRO_KEY_6){
                hide += '*';
                secret+='6';
            }
            else if(keyCode==ALLEGRO_KEY_7){
                hide += '*';
                secret+='7';
            }
            else if(keyCode==ALLEGRO_KEY_8){
                hide += '*';
                secret+='8';
            }
            else if(keyCode==ALLEGRO_KEY_9){
                hide += '*';
                secret+='9';
            }
            else if(!small_or_big){//small
                if(keyCode==ALLEGRO_KEY_A){
                    secret+='a';
                    hide += '*';
                }
                else if(keyCode==ALLEGRO_KEY_B){
                    secret+='b';
                    hide += '*';
                }
                else if(keyCode==ALLEGRO_KEY_C){
                    secret+='c';
                    hide += '*';
                }
                else if(keyCode==ALLEGRO_KEY_D){
                    secret+='d';
                    hide += '*';
                }
                else if(keyCode==ALLEGRO_KEY_E){    
                    secret+='e';
                    hide += '*';
                }
                else if(keyCode==ALLEGRO_KEY_F){
                    secret+='f';
                    hide += '*';
                }
                else if(keyCode==ALLEGRO_KEY_G){
                    secret+='g';
                    hide += '*';
                }
                else if(keyCode==ALLEGRO_KEY_H){
                    secret+='h';
                    hide += '*';
                }
                else if(keyCode==ALLEGRO_KEY_I){
                    secret+='i';
                    hide += '*';
                }
                else if(keyCode==ALLEGRO_KEY_J){
                    secret+='j';
                    hide += '*';
                }
                else if(keyCode==ALLEGRO_KEY_K){
                    secret+='k';
                    hide += '*';
                }
                else if(keyCode==ALLEGRO_KEY_L){
                    secret+='l';
                    hide += '*';
                }
                else if(keyCode==ALLEGRO_KEY_M){
                    secret+='m';
                    hide += '*';
                }
                else if(keyCode==ALLEGRO_KEY_N){
                    secret+='n';
                    hide += '*';
                }
                else if(keyCode==ALLEGRO_KEY_O){
                    secret+='o';
                    hide += '*';
                }
                else if(keyCode==ALLEGRO_KEY_P){
                    secret+='p';
                    hide += '*';
                }
                else if(keyCode==ALLEGRO_KEY_Q){
                    secret+='q';
                    hide += '*';
                }
                else if(keyCode==ALLEGRO_KEY_R){
                    secret+='r';
                    hide += '*';
                }
                else if(keyCode==ALLEGRO_KEY_S){
                    secret+='s';
                    hide += '*';
                }
                else if(keyCode==ALLEGRO_KEY_T){
                    secret+='t';
                    hide += '*';
                }
                else if(keyCode==ALLEGRO_KEY_U){
                    secret+='u';
                    hide += '*';
                }
                else if(keyCode==ALLEGRO_KEY_V){
                    secret+='v';
                    hide += '*';
                }
                else if(keyCode==ALLEGRO_KEY_W){
                    secret+='w';
                    hide += '*';
                }
                else if(keyCode==ALLEGRO_KEY_X){
                    secret+='x';
                    hide += '*';
                }
                else if(keyCode==ALLEGRO_KEY_Y){
                    secret+='y';
                    hide += '*';
                }
                else if(keyCode==ALLEGRO_KEY_Z){
                    secret+='z';
                    hide += '*';
                }
            }
            else{
                if(keyCode==ALLEGRO_KEY_A){
                    hide += '*';
                    secret+='A';
                }
                else if(keyCode==ALLEGRO_KEY_B){
                    hide += '*';
                    secret+='B';
                }
                else if(keyCode==ALLEGRO_KEY_C){
                    hide += '*';
                    secret+='C';
                }
                else if(keyCode==ALLEGRO_KEY_D){
                    hide += '*';
                    secret+='D';
                }
                else if(keyCode==ALLEGRO_KEY_E){
                    hide += '*';
                    secret+='E';
                }
                else if(keyCode==ALLEGRO_KEY_F){
                    hide += '*';
                    secret+='F';
                }
                else if(keyCode==ALLEGRO_KEY_G){
                    hide += '*';
                    secret+='G';
                }
                else if(keyCode==ALLEGRO_KEY_H){
                    hide += '*';
                    secret+='H';
                }
                else if(keyCode==ALLEGRO_KEY_I){
                    hide += '*';
                    secret+='I';
                }
                else if(keyCode==ALLEGRO_KEY_J){
                    hide += '*';
                    secret+='J';
                }
                else if(keyCode==ALLEGRO_KEY_K){
                    hide += '*';
                    secret+='K';
                }
                else if(keyCode==ALLEGRO_KEY_L){
                    hide += '*';
                    secret+='L';
                }
                else if(keyCode==ALLEGRO_KEY_M){
                    hide += '*';
                    secret+='M';
                }
                else if(keyCode==ALLEGRO_KEY_N){
                    hide += '*';
                    secret+='N';
                }
                else if(keyCode==ALLEGRO_KEY_O){
                    hide += '*';
                    secret+='O';
                }
                else if(keyCode==ALLEGRO_KEY_P){
                    hide += '*';
                    secret+='P';
                }
                else if(keyCode==ALLEGRO_KEY_Q){
                    hide += '*';
                    secret+='Q';
                }
                else if(keyCode==ALLEGRO_KEY_R){
                    hide += '*';
                    secret+='R';
                }
                else if(keyCode==ALLEGRO_KEY_S){
                    hide += '*';
                    secret+='S';
                }
                else if(keyCode==ALLEGRO_KEY_T){
                    hide += '*';
                    secret+='T';
                }
                else if(keyCode==ALLEGRO_KEY_U){
                    hide += '*';
                    secret+='U';
                }
                else if(keyCode==ALLEGRO_KEY_V){
                    hide += '*';
                    secret+='V';
                }
                else if(keyCode==ALLEGRO_KEY_W){
                    hide += '*';
                    secret+='W';
                }
                else if(keyCode==ALLEGRO_KEY_X){
                    hide += '*';
                    secret+='X';
                }
                else if(keyCode==ALLEGRO_KEY_Y){
                    hide += '*';
                    secret+='Y';
                }
                else if(keyCode==ALLEGRO_KEY_Z){
                    hide += '*';
                    secret+='Z';
                }
            }
        }
    }
    else if(Collide(1000,965,1580,1035)&&registering){
        if(keyCode==ALLEGRO_KEY_BACKSPACE){
            if(secret2.size()>0){
                hide2.pop_back();
                secret2.pop_back();
            }
        }
        else if(secret2.size()<20){
            if(keyCode==ALLEGRO_KEY_0){
                hide2 += '*';
                secret2+='0';
            }
            else if(keyCode==ALLEGRO_KEY_1){
                hide2 += '*';
                secret2+='1';
            }
            else if(keyCode==ALLEGRO_KEY_2){
                hide2 += '*';
                secret2+='2';
            }
            else if(keyCode==ALLEGRO_KEY_3){
                hide2 += '*';
                secret2+='3';
            }
            else if(keyCode==ALLEGRO_KEY_4){
                hide2 += '*';
                secret2 += '4';
            }
            else if(keyCode==ALLEGRO_KEY_5){
                hide2 += '*';
                secret2 += '5';
            }
            else if(keyCode==ALLEGRO_KEY_6){
                hide2 += '*';
                secret2 += '6';
            }
            else if(keyCode==ALLEGRO_KEY_7){
                hide2 += '*';
                secret2 += '7';
            }
            else if(keyCode==ALLEGRO_KEY_8){
                hide2 += '*';
                secret2 += '8';
            }
            else if(keyCode==ALLEGRO_KEY_9){
                hide2 += '*';
                secret2 += '9';
            }
            else if(!small_or_big){//small
                if(keyCode==ALLEGRO_KEY_A){
                    secret2 += 'a';
                    hide2 += '*';
                }
                else if(keyCode==ALLEGRO_KEY_B){
                    secret2 += 'b';
                    hide2 += '*';
                }
                else if(keyCode==ALLEGRO_KEY_C){
                    secret2 += 'c';
                    hide2 += '*';
                }
                else if(keyCode==ALLEGRO_KEY_D){
                    secret2 += 'd';
                    hide2 += '*';
                }
                else if(keyCode==ALLEGRO_KEY_E){    
                    secret2 += 'e';
                    hide2 += '*';
                }
                else if(keyCode==ALLEGRO_KEY_F){
                    secret2 += 'f';
                    hide2 += '*';
                }
                else if(keyCode==ALLEGRO_KEY_G){
                    secret2 += 'g';
                    hide2 += '*';
                }
                else if(keyCode==ALLEGRO_KEY_H){
                    secret2 += 'h';
                    hide2 += '*';
                }
                else if(keyCode==ALLEGRO_KEY_I){
                    secret2 += 'i';
                    hide2 += '*';
                }
                else if(keyCode==ALLEGRO_KEY_J){
                    secret2 += 'j';
                    hide2 += '*';
                }
                else if(keyCode==ALLEGRO_KEY_K){
                    secret2 += 'k';
                    hide2 += '*';
                }
                else if(keyCode==ALLEGRO_KEY_L){
                    secret2 += 'l';
                    hide2 += '*';
                }
                else if(keyCode==ALLEGRO_KEY_M){
                    secret2 += 'm';
                    hide2 += '*';
                }
                else if(keyCode==ALLEGRO_KEY_N){
                    secret2 += 'n';
                    hide2 += '*';
                }
                else if(keyCode==ALLEGRO_KEY_O){
                    secret2 += 'o';
                    hide2 += '*';
                }
                else if(keyCode==ALLEGRO_KEY_P){
                    secret2 += 'p';
                    hide2 += '*';
                }
                else if(keyCode==ALLEGRO_KEY_Q){
                    secret2 += 'q';
                    hide2 += '*';
                }
                else if(keyCode==ALLEGRO_KEY_R){
                    secret2 += 'r';
                    hide2 += '*';
                }
                else if(keyCode==ALLEGRO_KEY_S){
                    secret2 += 's';
                    hide2 += '*';
                }
                else if(keyCode==ALLEGRO_KEY_T){
                    secret2 += 't';
                    hide2 += '*';
                }
                else if(keyCode==ALLEGRO_KEY_U){
                    secret2 += 'u';
                    hide2 += '*';
                }
                else if(keyCode==ALLEGRO_KEY_V){
                    secret2 += 'v';
                    hide2 += '*';
                }
                else if(keyCode==ALLEGRO_KEY_W){
                    secret2 += 'w';
                    hide2 += '*';
                }
                else if(keyCode==ALLEGRO_KEY_X){
                    secret2 += 'x';
                    hide2 += '*';
                }
                else if(keyCode==ALLEGRO_KEY_Y){
                    secret2 += 'y';
                    hide2 += '*';
                }
                else if(keyCode==ALLEGRO_KEY_Z){
                    secret2 += 'z';
                    hide2 += '*';
                }
            }
            else{
                if(keyCode==ALLEGRO_KEY_A){
                    hide2 += '*';
                    secret2 += 'A';
                }
                else if(keyCode==ALLEGRO_KEY_B){
                    hide2 += '*';
                    secret2 += 'B';
                }
                else if(keyCode==ALLEGRO_KEY_C){
                    hide2 += '*';
                    secret2 += 'C';
                }
                else if(keyCode==ALLEGRO_KEY_D){
                    hide2 += '*';
                    secret2 += 'D';
                }
                else if(keyCode==ALLEGRO_KEY_E){
                    hide2 += '*';
                    secret2 += 'E';
                }
                else if(keyCode==ALLEGRO_KEY_F){
                    hide2 += '*';
                    secret2 += 'F';
                }
                else if(keyCode==ALLEGRO_KEY_G){
                    hide2 += '*';
                    secret2 += 'G';
                }
                else if(keyCode==ALLEGRO_KEY_H){
                    hide2 += '*';
                    secret2 += 'H';
                }
                else if(keyCode==ALLEGRO_KEY_I){
                    hide2 += '*';
                    secret2 += 'I';
                }
                else if(keyCode==ALLEGRO_KEY_J){
                    hide2 += '*';
                    secret2 += 'J';
                }
                else if(keyCode==ALLEGRO_KEY_K){
                    hide2 += '*';
                    secret2 += 'K';
                }
                else if(keyCode==ALLEGRO_KEY_L){
                    hide2 += '*';
                    secret2 += 'L';
                }
                else if(keyCode==ALLEGRO_KEY_M){
                    hide2 += '*';
                    secret2 += 'M';
                }
                else if(keyCode==ALLEGRO_KEY_N){
                    hide2 += '*';
                    secret2 += 'N';
                }
                else if(keyCode==ALLEGRO_KEY_O){
                    hide2 += '*';
                    secret2 += 'O';
                }
                else if(keyCode==ALLEGRO_KEY_P){
                    hide2 += '*';
                    secret2 += 'P';
                }
                else if(keyCode==ALLEGRO_KEY_Q){
                    hide2 += '*';
                    secret2 += 'Q';
                }
                else if(keyCode==ALLEGRO_KEY_R){
                    hide2 += '*';
                    secret2 += 'R';
                }
                else if(keyCode==ALLEGRO_KEY_S){
                    hide2 += '*';
                    secret2 += 'S';
                }
                else if(keyCode==ALLEGRO_KEY_T){
                    hide2 += '*';
                    secret2 += 'T';
                }
                else if(keyCode==ALLEGRO_KEY_U){
                    hide2 += '*';
                    secret2 += 'U';
                }
                else if(keyCode==ALLEGRO_KEY_V){
                    hide2 += '*';
                    secret2 += 'V';
                }
                else if(keyCode==ALLEGRO_KEY_W){
                    hide2 += '*';
                    secret2 += 'W';
                }
                else if(keyCode==ALLEGRO_KEY_X){
                    hide2 += '*';
                    secret2 += 'X';
                }
                else if(keyCode==ALLEGRO_KEY_Y){
                    hide2 += '*';
                    secret2 += 'Y';
                }
                else if(keyCode==ALLEGRO_KEY_Z){
                    hide2 += '*';
                    secret2 += 'Z';
                }
            }
        }
    }
}
void RecordScene::Update(float deltatime){
    result_tick++;
    if(result_tick>10000000)
        result_tick = 0;
    result->Color = al_map_rgba(204, 0, 0, 255*(!!result_mark));
    buttontext1->Text = visible ? "隱藏" : "顯示";
    if(registering){
        buttontext2->Text = visible2 ? "隱藏" : "顯示";
    }
    if(result_mark==2){
        result_mark = 3;
        result_tick = 0;
    }
    else if(result_mark==3&&result_tick>45){
        Engine::GameEngine::GetInstance().ChangeScene("scoreboard");
    }
    //std::cout << name << std::endl;
    if(name.size()<=8){
        account->Text = name;
    }
    if(secret.size()<=20){
        if(visible)
            password->Text = secret;
        else
            password->Text = hide;
    }
    if(secret2.size()<=20&&registering == 1){
        if(visible2)
            password2->Text = secret2;
        else
            password2->Text = hide2;
    }
}



void RecordScene::ButtonClick(int id){
    if(id == 0)
        visible = !visible;
    else if(id == 1)
        visible2 = !visible2;
    else if(id == 2){
        if(registering){
            RemoveControlObject(btn2->controlIterator,btn2->GetObjectIterator());
            RemoveObject(buttontext2->GetObjectIterator());
            RemoveObject(sign2->GetObjectIterator());
            RemoveObject(img2->GetObjectIterator());
            RemoveObject(password2->GetObjectIterator());
            registering = 0;
            name = "";
            secret = "";
            secret2 = "";
            hide = "";
            hide2 = "";
            visible2 = false;
        }
    }
    else if(id == 3){
        if(!registering){
            registering = 1;

            sign2 = new Engine::Label("確認密碼：", "font2.ttc", 70, 850, 1000, 255 , 255 ,255 , 255, 1, 0.5);
            AddNewObject(sign2);
            img2 = new Engine::Image("block.png",1333-420,965,840,70,0,0);
            AddNewObject(img2);

            btn2 = new Engine::ImageButton("start/floor.png", "start/floor.png", 1333+420-100, 965, 100, 70);
            btn2->SetOnClickCallback(std::bind(&RecordScene::ButtonClick, this, 1));
            AddNewControlObject(btn2);
            buttontext2 = new Engine::Label("顯示", "font2.ttc", 40, 1333+420-50, 1000, 0 , 0 ,0 , 255, 0.5, 0.5);
            AddNewObject(buttontext2);

            password2 = new Engine::Label(hide2, "font2.ttc", 40, 1333-420, 1000, 0,0,0 , 255, 0, 0.5);
            AddNewObject(password2);
        }
        
    }

}

void RecordScene::BackOnClick(int stage){
    Engine::GameEngine::GetInstance().ChangeScene("anqi_start");
}

void RecordScene::Terminate(){
    AudioHelper::StopBGM(bgmId);
    IScene::Terminate();
}