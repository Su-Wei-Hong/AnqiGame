#include"scoreboard_scene.hpp"
#include "Engine/GameEngine.hpp"
#include "UI/Component/ImageButton.hpp"
#include "UI/Component/Label.hpp"
#include "Engine/AudioHelper.hpp"
#include "Engine/Resources.hpp"
#include <fstream>
#include <string>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <map>
#include <vector>
#include <tuple>
std::map<std::string,int> record;
std::vector<std::pair<std::string,int>> data;
std::vector<std::tuple<Engine::Label *,Engine::Label*,Engine::Label*>> delete_array;
int cur_page = 1;
int max_page = 0;
int w,h,halfW,halfH;
float lineSpacing = 100; 
float startX = 300;  // Adjust this to center or align left
float startY = 1050;  // Starting Y position
Engine::Image *golden;
Engine::Image *silver;
Engine::Image *bronze;
void ScoreBoardScene::Initialize(){
    cur_page = 1;
    delete_array.clear();
    data.clear();
    record.clear();
    w = Engine::GameEngine::GetInstance().GetScreenSize().x;
    h = Engine::GameEngine::GetInstance().GetScreenSize().y;
    halfW = w / 2;
    halfH = h / 2;
    
    AddNewObject(new Engine::Label("<< 排行榜 >>", "font2.ttc", 150, halfW, 125, 51, 153,255, 255, 0.5, 0.5));
    Engine::ImageButton *btn;
    std::ifstream fin("Resource/scoreboard.txt");
    std::string name;
    int score;
    
    int maxline = 6;
    if (!fin.is_open()) {
        std::cout << "Failed to open scoreboard.txt\n";
        return;
    }
    
    while (fin>>name>>score) {
        data.push_back({name,score});
        // std::stringstream ss;
        // ss << std::left << std::setw(20) << name << std::right << std::setw(4) << score;
        // Create a label for each line and add it to the scene
    
    }
    
    for(int j=data.size()-1;j>0;j--){
        for(int i=0;i<j;i++){
            if(data[i].second<data[i+1].second){
                auto a = data[i];
                data[i] = data[i+1];
                data[i+1] = a;
            }
        }
    }
    
    Engine::Label * nameLabel;
    // int sz = data.size()-3>6?6:data.size();
    // max_page = data.size()/10;
    if(data.size()>=1){
        
        golden = new Engine::Image("golden.png",1333,700,400,400, 0.5,1);
        AddNewObject(golden);
        for (int i = 0; i < 5; i++) {
            float t = i / 4.0f;
            // 線性插值新的黃金漸層
            unsigned char r = (1 - t) * 255 + t * 180;
            unsigned char g = (1 - t) * 240 + t * 120;
            unsigned char b = (1 - t) * 130 + t * 20;
            unsigned char a = 255 - 10 * i;

            nameLabel = new Engine::Label(data[0].first, "font2.ttc", 100,1333 + 1.5 * i, 800,r, g, b, a, 0.5, 1);
            AddNewObject(nameLabel);
        }
        for (int i = 0; i < 5; i++) {
            float t = i / 4.0f;
            // 線性插值新的黃金漸層
            unsigned char r = (1 - t) * 255 + t * 180;
            unsigned char g = (1 - t) * 240 + t * 120;
            unsigned char b = (1 - t) * 130 + t * 20;
            unsigned char a = 255 - 10 * i;

            nameLabel = new Engine::Label(std::to_string(data[0].second), "font2.ttc", 100,1333 + 1.5 * i, 950,r, g, b, a, 0.5, 1);
            AddNewObject(nameLabel);
        }
    }

    // c = new Engine::Label(std::to_string(data[0].second), "font2.ttc", 200,halfW+350, startY + i * lineSpacing,255,215,0, 255, 0.5, 1);
    // AddNewObject(c);
    if(data.size()>=2){
        silver = new Engine::Image("silver.png",500,700,370,370, 0.5,1);
        AddNewObject(silver);
        for (int i = 0; i < 5; i++) {
            // float t = i / 4.0f;
            // unsigned char r = (1 - t) * 220 + t * 130;
            // unsigned char g = (1 - t) * 220 + t * 130;
            // unsigned char b = (1 - t) * 220 + t * 140;
            float t = i / 4.0f;
            unsigned char r = (1 - t) * 255 + t * 80;
            unsigned char g = (1 - t) * 255 + t * 80;
            unsigned char b = (1 - t) * 255 + t * 100;
            unsigned char a = 255 - 40 * i;

            
            

            nameLabel = new Engine::Label(data[1].first, "font2.ttc", 85,500 + 1.5 * i, 800,r, g, b, a,0.5,1);
            AddNewObject(nameLabel);
        }
        for (int i = 0; i < 5; i++) {
        float t = i / 4.0f;
        unsigned char r = (1 - t) * 255 + t * 80;
        unsigned char g = (1 - t) * 255 + t * 80;
        unsigned char b = (1 - t) * 255 + t * 100;
        unsigned char a = 255 - 40 * i;

        nameLabel = new Engine::Label(std::to_string(data[1].second), "font2.ttc", 85,500 + 1.5 * i, 950,r, g, b, a,0.5, 1);
        AddNewObject(nameLabel);
    }
        // AddNewObject(new Engine::Label(data[1].first, "font2.ttc", 50,halfW-300, startY + i * lineSpacing,255,255,255, 255, 0.0, 0.0));
        // AddNewObject(new Engine::Label(std::to_string(data[1].second), "font2.ttc", 50,halfW+350, startY + i * lineSpacing,255,255,255, 255, 1, 0.0));
    }
    
    
    if(data.size()>=3){
        bronze = new Engine::Image("bronze.png",2150,700,320,320, 0.5,1);
        AddNewObject(bronze);
        for (int i = 0; i < 5; i++) {
        // 深化底色：偏紅黑銅，強對比
            float t = i / 4.0f;
        // 插值：從亮到暗（極致對比）
            unsigned char r = (1 - t) * 255 + t * 40;
            unsigned char g = (1 - t) * 120 + t * 15;
            unsigned char b = (1 - t) * 60  + t * 10;

            unsigned char a = 255 - 60 * i;

            nameLabel = new Engine::Label(data[2].first, "font2.ttc", 70, 2150 + 2 * i, 800, r, g, b, a,0.5, 1);
            AddNewObject(nameLabel);
        }
        for (int i = 0; i < 5; i++) {
            float t = i / 4.0f;
        // 插值：從亮到暗（極致對比）
            unsigned char r = (1 - t) * 255 + t * 40;
            unsigned char g = (1 - t) * 120 + t * 15;
            unsigned char b = (1 - t) * 60  + t * 10;
            unsigned char a = 255 - 60 * i;
            nameLabel = new Engine::Label(std::to_string(data[2].second), "font2.ttc", 70,2150 + 2 * i, 950,r, g, b, a,0.5, 1);
            AddNewObject(nameLabel);
        }
    }    

    if(data.size()<=9)
        max_page = 1;
    else
        max_page = (data.size()-3)/6 + 1;
    
    if(data.size()>3){
        int sz = data.size()-3>6?6:data.size()-3;
        for(int j=0;j<(sz-1)/3 + 1;j++){
            for(int i=0;j*3+i<sz&&i<3;i++){
                Engine::Label * a;
                a = new Engine::Label(std::to_string(j*3+i+4)+". ", "font2.ttc", 40,startX + i*850, startY + j * 150,255,255,255, 255, 1, 0.0);
                AddNewObject(a);
                Engine::Label * b;
                b = new Engine::Label(data[j*3+i+3].first, "font2.ttc", 40,startX + i*850, startY + j * 150,255,255,255, 255, 0.0, 0.0);
                AddNewObject(b);
                Engine::Label * c;
                c = new Engine::Label(std::to_string(data[j*3+i+3].second), "font2.ttc", 40,startX+400 + i*850, startY + j * 150,255,255,255, 255, 1, 0.0);
                AddNewObject(c);
                delete_array.push_back({a,b,c});
            }
        }
    }

  
    btn = new Engine::ImageButton("block2.png", "start/dirt.png", halfW - 125, 1325, 250, 100);
    btn->SetOnClickCallback(std::bind(&ScoreBoardScene::BackOnClick, this, 2));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("返回", "font2.ttc", 50, halfW, 1375,255,255,255, 255, 0.5, 0.5));

    btn = new Engine::ImageButton("block2.png", "start/dirt.png", halfW - 475,1325, 250, 100);
    btn->SetOnClickCallback(std::bind(&ScoreBoardScene::Update, this, -1));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("上一頁", "font2.ttc", 50, halfW - 350, 1375, 255,255,255, 255, 0.5, 0.5));

    btn = new Engine::ImageButton("block2.png", "start/dirt.png", halfW + 225,1325, 250, 100);
    btn->SetOnClickCallback(std::bind(&ScoreBoardScene::Update, this, 1));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("下一頁", "font2.ttc", 50, halfW + 350, 1375, 255,255,255, 255, 0.5, 0.5));
    bgmId = AudioHelper::PlayBGM("kind.mp3");
}

void ScoreBoardScene::Update(int x){
    
    
    if(cur_page+x>0&&cur_page+x<=max_page){
        int y = cur_page<max_page?6:(data.size()-3)%6;
        if(y==0){
            y = 6;
        }
        cur_page += x;
        
        for(int i=0;i<y;i++){
            auto a = std::get<0>(delete_array[i]);
            auto b = std::get<1>(delete_array[i]);
            auto c = std::get<2>(delete_array[i]);
            RemoveObject(a->GetObjectIterator());
            RemoveObject(b->GetObjectIterator());
            RemoveObject(c->GetObjectIterator());
        }
        delete_array.clear();
        y = cur_page<max_page?6:(data.size()-3)%6;
        if(y==0){
            y = 6;
        }
        for(int j=0;j<(y-1)/3+1;j++){
            for(int i=0;j*3+i<y&&i<3;i++){
                int index = cur_page * 6 - 6 + j*3 + i + 3;
                Engine::Label * a;
                a = new Engine::Label(std::to_string(index+1)+". ", "font2.ttc", 40,startX + i*850, startY + j * 150,255,255,255, 255, 1, 0.0);
                AddNewObject(a);
                Engine::Label * b;
                b = new Engine::Label(data[index].first, "font2.ttc", 40,startX + i*850, startY + j * 150,255,255,255, 255, 0.0, 0.0);
                AddNewObject(b);
                Engine::Label * c;
                c = new Engine::Label(std::to_string(data[index].second), "font2.ttc", 40,startX+400 + i*850, startY + j * 150,255,255,255, 255, 1, 0.0);
                AddNewObject(c);
                delete_array.push_back({a,b,c});
            }
        }
        
        return;

    }
}
void ScoreBoardScene::BackOnClick(int stage){
    Engine::GameEngine::GetInstance().ChangeScene("anqi_start");
}

void ScoreBoardScene::Terminate(){
    AudioHelper::StopBGM(bgmId);
    IScene::Terminate();
}