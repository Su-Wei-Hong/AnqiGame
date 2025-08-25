#include <allegro5/allegro_audio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <functional>
#include <tuple>
#include <memory>
#include <string>
#include <ostream>
#include <iostream>
#include "Engine/AudioHelper.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Point.hpp"
#include "Engine/Resources.hpp"

#include "anqi_scene.hpp"
#include "UI/Component/ImageButton.hpp"
#include "UI/Component/Label.hpp"
#include "UI/Component/Slider.hpp"
#include "Chess/chess.hpp"
#include "anqi_setting_scene.hpp"
#include "level_select.hpp"

#include <algorithm>  // for std::shuffle
#include <random>     // for std::default_random_engine
#include <chrono>
#include <vector>
#include <cmath>
#include <set>
#include <deque>


Engine::Point pos;
int correspond[2][7] = {
    {1,2,4,6,8,10,12},{17,18,20,22,24,26,28}
};

std::vector<Chess*> chessRecord(33);
std::vector<std::pair<int,int>> direction = {
        {-1,0},//up
        {1,0},//down
        {0,-1},//left
        {0,1}///right
};

std::vector<int> value = {
    0, 800, 1000, 1000, 600 ,600,300,300,200,200,700,700,100,100,100,100,100,
    800,1000,1000,600,600,300,300,200,200,700,700,100,100,100,100,100

};
Action *bestAction = new Action();
bool lastMouseDown = false;
bool mouseJustClicked = false;
int unflip = 32;
int harmony_step = 0;
int score = 0;
int layer = 0;
int temp_x;
int temp_y;
int AI_y;
int AI_x;
int AI_op;
int dark[15];
int color = -1;
int exchange_flag = 1;
float pi = ALLEGRO_PI;
float rx,bx,p1_x,p2_x,t1_x,t2_x;
Chess* temp = nullptr;
Engine::Image* pointer;
Engine::Image* peace_window;
enum Turn{
    UNKNOWNED,
    RED_TURN,
    BLACK_TURN,
};
std::deque<AnqiRecord*> anqi_record;
Turn turn = UNKNOWNED;
Turn first_turn;
Turn computer_turn;
std::vector<std::vector<Chess*>> chessPositions(4, std::vector<Chess*>(8, nullptr));
std::vector<std::vector<int>> chessBoard(4,std::vector<int>(8,0));
std::deque<std::vector<std::vector<int>>> history;
Manage* red = new Manage();
Manage* black = new Manage();
Manage *player = nullptr,*computer = nullptr;
Tick* tick = new Tick();
Flag* flag = new Flag();
Sign* sign = new Sign();
std::vector<std::vector<AI_Chess*>> AI_map(4,std::vector<AI_Chess*>(8, nullptr));

void AnqiScene::CalculateScore(ChessColor color){
    int state_flag = 0;
    int base_score[17] = {
        0, 5000, 1200, 1200, 800, 800, 400, 400,
        200, 200, 1000, 1000, 100, 100, 100, 100, 100
    };
    int start = (color == BLACK) ? 0 : 16;
    for(int i = 1; i < 17; i++){
        if(i%2==0&&i<=12)
            state_flag = 0;
        if(chessRecord[i+start]){
            int score_to_add = base_score[i];

            if(i<12&&state_flag&&i%2==1&&i!=1){
                score_to_add = static_cast<int>(score_to_add * 1.5);
            }
            
            if(i == 16 && state_flag)
                score_to_add = 600;

            score += score_to_add;
            
            if(i == 12 || (i % 2 == 0 && i < 12))
                state_flag = 1;
        }

        else if(i>12){
            state_flag = 0;
        }
    }
}

int minimax_flag = 0;
void AnqiScene::Initialize() {
    history.clear();
    minimax_flag = 0;
    exchange_flag = 1;
    value = {
        0, 800, 1000, 1000, 600 ,600,300,300,200,200,700,700,100,100,100,100,100,
        800,1000,1000,600,600,300,300,200,200,700,700,100,100,100,100,100
    };

    int times = level/2;
    int parameter = 1;
    for(int h = 0 ; h < times ; h++)
        parameter *= 2;
    PieceInfo pieceTable[32] = {
        {BLACK, KING, 0, false},        // 1
        {BLACK, KNIGHT, 0, false},     // 2
        {BLACK, KNIGHT, 0, false},     // 3
        {BLACK, ELEPHANT, 0, false},   // 4
        {BLACK, ELEPHANT, 0, false},   // 5
        {BLACK, CAR, 0, false},        // 6
        {BLACK, CAR, 0, false},        // 7
        {BLACK, HORSE, 0, false},      // 8
        {BLACK, HORSE, 0, false},      // 9
        {BLACK, CANNON, 0, true},      // 10
        {BLACK, CANNON, 0, true},      // 11
        {BLACK, SOLDIER, 0, false},    // 12
        {BLACK, SOLDIER, 0, false},    // 13
        {BLACK, SOLDIER, 0, false},    // 14
        {BLACK, SOLDIER, 0, false},    // 15
        {BLACK, SOLDIER, 0, false},    // 16
        {RED, KING, -16, false},       // 17
        {RED, KNIGHT, -16, false},     // 18
        {RED, KNIGHT, -16, false},     // 19
        {RED, ELEPHANT, -16, false},   // 20
        {RED, ELEPHANT, -16, false},   // 21
        {RED, CAR, -16, false},        // 22
        {RED, CAR, -16, false},        // 23
        {RED, HORSE, -16, false},      // 24
        {RED, HORSE, -16, false},      // 25
        {RED, CANNON, -16, true},      // 26
        {RED, CANNON, -16, true},      // 27
        {RED, SOLDIER, -16, false},    // 28
        {RED, SOLDIER, -16, false},    // 29
        {RED, SOLDIER, -16, false},    // 30
        {RED, SOLDIER, -16, false},    // 31
        {RED, SOLDIER, -16, false},    // 32
    };

    for(int i=1;i<=14;i++){
        if(i == 1 || i == 8)
            dark[i] = 1;
        else if(i == 7 || i == 14)
            dark[i] = 5;
        else
            dark[i] = 2;
            
    }
    layer = 0;
    AI_op = 0;
    unflip = 32;
    player = nullptr;
    computer = nullptr;
    score  = 0;
    red->cannon1 = nullptr;
    red->cannon2 = nullptr;
    black->cannon1 = nullptr;
    black->cannon2 = nullptr;
    red->prolong_time = 0;
    red->remain = 16;
    red->regret_time = 0;
    red->request_time = 0;
    red->attack_step = 0;
    red->attack_target.clear();
    red->attacker = nullptr;

    black->prolong_time = 0;
    black->remain = 16;
    black->regret_time = 0;
    black->regret_time = 0;
    black->attack_step = 0;
    black->attack_target.clear();
    red->attacker = nullptr;
    harmony_step = 0;
    
    //tick
    tick->thinking_tick = 0;
    tick->show_chess_tick = 0;
    tick->preDelay_tick = 0;
    tick->AI_tick = 0;
    tick->regret_invalid_tick = 0;
    tick->regret_tick = 0;

    if(is_single_player)
        tick->turn_tick = 7259/times;
    else
        tick->turn_tick = 1259;

    tick->pointer_tick = -30;
    tick->prolong_tick = 0;
    tick->peace_tick = 0;
    tick->attack_tick = 0;
    tick->harmony_tick = 0;
    //flag
    flag->change_scene_flag = 0;
    flag->preDelay_flag = 0;
    flag->AI_actionFlag = 0;
    flag->AI_moveFlag = 0;
    flag->attack_mark = 0;
    flag->attack_flag = 0;
    flag->harmony_flag = 0;
    
    flag->attack_mark = 0;
    flag->turn_flag = -1;
    color = -1;
    
    anqi_record.clear();
    turn = UNKNOWNED;
    int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
    int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
    int halfW = w / 2;
    int halfH = h / 2;
    //Engine::ImageButton *btn;
    //al_init_image_addon();

    
    
    
    //image->Position = Point(halfW - 400, halfH - 200);
    AddNewObject(new Engine::Image("chess_board.png", 333, 300 , 2000 , 1200 , 0, 0));
    AddNewObject(ChessGroup = new Group());
    AddNewObject(UIGroup = new Group());
    Chess* chess;

    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::vector<int> order(32);
    for (int i = 0; i < 32; ++i) {
        order[i] = i+1;
    }
    std::shuffle(order.begin(), order.end(), std::default_random_engine(seed));
    sign->harmony_warn = new Engine::Label("空步判和", "font2.ttc", 100, halfW, 140, 255 , 255 ,255 , 0, 0.5, 0.5);
    AddNewObject(sign->harmony_warn);
    sign->attack_warn = new Engine::Label("長捉禁手", "font2.ttc", 100, halfW, 140, 255 , 255 ,255 , 0, 0.5, 0.5);
    AddNewObject(sign->attack_warn);
    sign->prolong_warn = new Engine::Label("已使用一次延長!", "font2.ttc", 100, halfW, 140, 255 , 255 ,255 , 0, 0.5, 0.5);
    AddNewObject(sign->prolong_warn);
    // sign->peace_warn = new Engine::Label("求和次數耗盡!", "font2.ttc", 100, halfW, 140, 255 , 255 ,255 , 0, 0.5, 0.5);
    // AddNewObject(sign->peace_warn);
    // sign->regret_invalid= new Engine::Label("無法悔棋!", "font2.ttc", 100, halfW, 140, 255 , 255 ,255 , 0, 0.5, 0.5);
    // AddNewObject(sign->regret_invalid);
    // sign->regret_warn = new Engine::Label("悔棋次數耗盡!", "font2.ttc", 100, halfW, 140, 255 , 255 ,255 , 0, 0.5, 0.5);
    // AddNewObject(sign->regret_warn);
    
    //sign->peace_ask->Color = al_map_rgba(255,255,255,255);

    
    
    for(int j=0; j<4; j++){
        for(int i=0; i<8; i++){
            int temp = order[j*8 + i];
            PieceInfo info = pieceTable[temp - 1];

            std::string s = GetImage(info.color, info.type);
            Chess* chess = new Chess(s, 635 + i * 200, 600 + j * 200, info.type, info.color, temp);
            AI_Chess* ai_chess = new AI_Chess(info.color,info.type,false,temp);
            AI_map[j][i] = ai_chess;
            ChessGroup->AddNewObject(chess);
            chessPositions[j][i] = chess;
            chessRecord[temp] = chess;
            if(info.isCannon){
                if(info.color == BLACK){
                    if(temp == 10){
                        black->cannon1 = chess;
                    }
                    else if(temp == 11){
                        black->cannon2 = chess;
                    }
                } 
                else { // RED
                    if(temp == 26){
                        red->cannon1 = chess;
                    }
                    else if(temp == 27){
                        red->cannon2 = chess;
                    }
                }
            }
        }
    }


    if(decide2==1)
        bgmId = AudioHelper::PlayBGM("play_music.ogg");
    else if(decide2==2)
        bgmId = AudioHelper::PlayBGM("relax.mp3");
    else
        bgmId = AudioHelper::PlayBGM("rock.mp3");
}

void RecordStep(AI_Chess* eater,std::pair<int,int> start,std::pair<int,int> destination,AI_Chess* prey,int operation){
    AnqiRecord* a = new AnqiRecord();
    a->eater = eater;
    a->operation = operation;
    a->start = start;
    a->destination = destination;
    a->prey = prey;
    anqi_record.push_back(a);
}

bool AnqiScene::Edible_greater(ChessType eater,ChessType prey){
    if(eater==CANNON)
        return false;
    else if(eater == SOLDIER && prey == KING)
        return true;
    else if(eater > prey&&!(eater == KING && prey == SOLDIER))
        return true;
    else
        return false;
}

bool AnqiScene::Edible(ChessType eater,ChessType prey){
    if(eater==CANNON)
        return false;
    else if(eater == SOLDIER && prey == KING)
        return true;
    else if(eater >= prey && !(eater == KING && prey == SOLDIER))
        return true;
    else
        return false;
}

bool AnqiScene::CannonValidEating(int j,int i,int temp_y,int temp_x){
    if((j==temp_y)==(i==temp_x))
        return false;

    int min,max;
    
    int count = 0;
    if (j == temp_y) {
        min = std::min(i, temp_x);
        max = std::max(i, temp_x);
        for (int k = min + 1; k < max; k++)
            if (chessPositions[j][k] != nullptr)
                count++;
    } else {
        min = std::min(j, temp_y);
        max = std::max(j, temp_y);
        for (int k = min + 1; k < max; k++)
            if (chessPositions[k][i] != nullptr)
                count++;
    }
    return count == 1;
}
void AnqiScene::ConstructUI(ChessColor color) {
    
    int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
    int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
    int halfH = h / 2;

    Engine::ImageButton *btn;
    
    rx = color==RED ? 0: w-312.5;
    bx = color==BLACK ? 0:w-290;
    p2_x = color==BLACK ? w-312.5/2-100:w-145-100;
    p1_x = color==BLACK ? 45:312.5/2-100;
    t2_x = color==BLACK ? w-312.5-200:w-290-200;
    t1_x = color==BLACK ? 490:512.5;
    AddNewObject(new Engine::Image("black_win.png", bx, 0 , 290 , 290 , 0, 0));
    AddNewObject(new Engine::Image("red_win.png", rx, 0 , 312.5 , 300 , 0, 0));
    pointer = new Engine::Image("pointer.png", p2_x, 325 , 200 , 200 , 0, 0);
    AddNewObject(pointer);
    if(!is_single_player){
        sign->timer = new Engine::Label("20", "font2.ttc", 150, t2_x, 150, 223, 21, 22, 255, 0.5, 0.5);
        AddNewObject(sign->timer);

        Engine::ImageButton* btn = new Engine::ImageButton("block2.png", "start/dirt.png", 0, halfH+400, 300,150);
        btn->SetOnClickCallback(std::bind(&AnqiScene::ButtonClick, this, 1));
        AddNewControlObject(btn);

        Engine::Image* img = new Engine::Image("block2.png", w-300, halfH+400, 300, 150);
        AddNewObject(img);

        Engine::Label* label = new Engine::Label("投降", "font2.ttc", 60, 150, halfH + 475, 255, 255, 255, 255, 0.5, 0.5);
        AddNewObject(label);

        label = new Engine::Label("延長0/3", "font2.ttc", 60, w - 150, halfH + 475, 255, 255, 255, 255, 0.5, 0.5);
        AddNewObject(label);

        sign->prolong = label;

    }
    else{
        sign->timer = new Engine::Label(std::to_string(7259/(level*60)), "font2.ttc", 150, t1_x, 150, 223, 21, 22, 0, 0.5, 0.5);
        AddNewObject(sign->timer);

        Engine::ImageButton* btn = new Engine::ImageButton("block2.png", "start/dirt.png", 0, halfH+400, 300,150);
        btn->SetOnClickCallback(std::bind(&AnqiScene::ButtonClick, this, 1));
        AddNewControlObject(btn);

        Engine::Label* label = new Engine::Label("投降", "font2.ttc", 60, 150, halfH + 475, 255, 255, 255, 255, 0.5, 0.5);
        AddNewObject(label);

        if(level>2){
            Engine::Image* img = new Engine::Image("block2.png", w-300, halfH+400, 300, 150);
            AddNewObject(img);

            Engine::Label* label = new Engine::Label("延長0/"+std::to_string(4*level-14), "font2.ttc", 60, w - 150, halfH + 475, 255, 255, 255, 255, 0.5, 0.5);
            AddNewObject(label);

            sign->prolong = label;
        }


    }

    int p_black = first_turn == BLACK_TURN ? 150 : w-150;
    int p_red = first_turn == RED_TURN ? 150 : w-150;
    sign->remain_black = new Engine::Label("剩餘:"+std::to_string(black->remain), "font2.ttc", 60, p_black,halfH+75, 255, 255, 255, 255, 0.5, 0.5);
    AddNewObject(sign->remain_black);

    sign->remain_red = new Engine::Label("剩餘:"+ std::to_string(red->remain), "font2.ttc", 60, p_red, halfH + 75, 255, 255, 255, 255, 0.5, 0.5);;
    AddNewObject(sign->remain_red);

    if(is_single_player){
        sign->thinking = new Engine::Label("思考中","font2.ttc",100,w-700,150,255,255,51,0,0.5,0.5);
        AddNewObject(sign->thinking);
    }
}

void AnqiScene::InitializeRound(ChessColor color){
    turn = color==RED ? BLACK_TURN : RED_TURN;
    first_turn = (color==RED)?RED_TURN:BLACK_TURN;
    ConstructUI(color);
    if(is_single_player)
        flag->turn_flag = 1;
}

void AnqiScene::ChangeRound(){
    if(!is_single_player){
        tick->turn_tick = 1259;
        turn = (turn == RED_TURN) ? BLACK_TURN : RED_TURN;
        color = (turn == RED_TURN) ? RED : BLACK;
    }
    else{
        flag->turn_flag = !flag->turn_flag;

        if(!flag->turn_flag){
            int times = level / 2;
            int parameter = 1;
            for(int h = 0 ; h < times ; h++)
                parameter *= 2;

            tick->turn_tick = 7259/parameter;
        }
        else
            tick->thinking_tick = 0;
    }
}

void AnqiScene::RecordTemp(Chess* chess,int i,int j){
    chess->Click();
    temp = chess;
    temp_x = i;
    temp_y = j;
}


void AnqiScene::Go(Engine::Point pos,int j,int i){                         
    temp->Move(pos);
    chessPositions[j][i] = temp;
    chessPositions[temp_y][temp_x] = nullptr;
    if(is_single_player){
        temp->ClickCancel();
        temp = nullptr;
    }    
    ChangeRound();
}

void AnqiScene::StopSign(){
    sign->harmony_warn->Color = al_map_rgba(255,255,255,0);
    sign->attack_warn->Color = al_map_rgba(255,255,255,0);
    //sign->peace_warn->Color = al_map_rgba(255,255,255,0);
    sign->prolong_warn->Color = al_map_rgba(255,255,255,0);
    // sign->regret_invalid->Color = al_map_rgba(255,255,255,0);
    // sign->regret_warn->Color = al_map_rgba(255,255,255,0);
}


void CheckEatCannon(Chess* chess) {
    if (chess->getType() != CANNON) return;

    bool isRed = chess->getColor() == RED;
    bool isFirst;
    if(chess->getNumber() == 10 && !isRed || chess->getNumber()==26 && isRed)
        isFirst = true;
    else 
        isFirst = false;

    if (isRed)
        (isFirst ? red->cannon1 : red->cannon2) = nullptr;
    else
        (isFirst ? black->cannon1 : black->cannon2) = nullptr;
}

void update_chessboard(int j,int i,int y,int x,int op){
    if(!op)
        chessBoard[j][i] = chessPositions[j][i]->getNumber();
    else{
        chessBoard[y][x] = chessBoard[j][i];
        chessBoard[j][i] = -1;
    }
}

void update_information(int j,int i,int y,int x,int op){//op = 0 flip //op = 1 eat //op = 2 move
    if(!op)
        AI_map[j][i]->open = true;
    else{
        if(op == 1)
            delete AI_map[y][x];

        AI_map[y][x] = AI_map[j][i];
        AI_map[j][i] = nullptr;
    }
}

void AnqiScene::AI_flip(int j,int i){
    //temp->ClickCancel();
    chessPositions[j][i]->Open();
    RecordTemp(chessPositions[j][i],i,j);
    unflip--;
    update_information(j,i,-1,-1,0);
    update_chessboard(j,i,-1,-1,0);
    harmony_step = 0;
    Update_dark(chessPositions[j][i]->getNumber(),-1);
}

void AnqiScene::show_all_chess(){
    for(int j=0;j<4;j++){
        for(int i=0;i<8;i++){
            if(chessPositions[j][i] && !chessPositions[j][i]->Check_open()){
                chessPositions[j][i]->Open();
            }
        }
    }
}

void AnqiScene::AI_eat(int j,int i,int y,int x){
    
    chessRecord[chessPositions[y][x]->getNumber()] = nullptr;
    pos.x = 635+x*200;
    pos.y = 600+y*200;
    RecordTemp(chessPositions[j][i],i,j);
    AI_y = y;
    AI_x = x;
    AI_op = 1;
    flag->AI_moveFlag = 1;
    tick->AI_tick = 0;
    harmony_step = 0;
    CheckEatCannon(chessPositions[y][x]);
    update_information(j,i,y,x,1);
    update_chessboard(j,i,y,x,1);
}


void AnqiScene::AI_move(int j,int i,int y,int x,int op){
    pos.x = 635+x*200;
    pos.y = 600+y*200;
    RecordTemp(chessPositions[j][i],i,j);
    AI_y = y;
    AI_x = x;
    AI_op = op;
    flag->AI_moveFlag = 1;
    tick->AI_tick = 0;
    update_information(j,i,y,x,2);
    update_chessboard(j,i,y,x,2);
}

void AnqiScene::Recover(){
    AnqiRecord* step = anqi_record.back();
    anqi_record.pop_back();
   
    if(step->operation == 0){
        AI_map[step->start.first][step->start.second]->open = false;
        unflip++;
        Update_dark(step->eater->num,1);
    }

    else{
        AI_map[step->start.first][step->start.second] = AI_map[step->destination.first][step->destination.second];
        AI_map[step->destination.first][step->destination.second] = nullptr;
        if(step->operation == 1){
            AI_map[step->destination.first][step->destination.second] = step->prey;
        }
    }

    
    delete step;
    step = nullptr;
}

bool AnqiScene::advantageous_exchange(int current_value,ChessColor prey_color,int j,int i){
    //exchange_flag = 1; initial attacker 
    //exchange_falg = -1; defender
    int local_exchange_flag = exchange_flag;
    int local_current_value = current_value;
    ChessColor eater_color;

    for(auto dir:direction){//general eating
        int dj = dir.first;
        int di = dir.second;
        if(j+dj >= 0 && j+dj < 4 && i+di >= 0 && i+di <8){
            if(AI_map[j+dj][i+di] && AI_map[j+dj][i+di]->open && AI_map[j+dj][i+di]->color != prey_color){
                eater_color = AI_map[j+dj][i+di]->color;
                if(Edible(AI_map[j+dj][i+di]->type,AI_map[j][i]->type)){
                    current_value += local_exchange_flag * value[AI_map[j][i]->num];
                    Apply_Move(AI_map[j+dj][i+di],j+dj,i+di,j,i,AI_map[j][i],1);
                    exchange_flag *= -1;
                    if(advantageous_exchange(current_value,eater_color,j,i)){
                        Recover();
                        return true;
                    }
                    Recover();
                }
            }
        }
        exchange_flag = local_exchange_flag;
        current_value = local_current_value;
    }

    for(auto dir:direction){
        int flag = 0;
        int dj = dir.first;
        int di = dir.second;
        for(int k=1;k<8;k++){
            if(j+k*dj >= 0 && j+k*dj < 4 && i+k*di >= 0 && i+k*di <8){
                AI_Chess* AI_temp = AI_map[j+k*dj][i+k*di];
                if(AI_temp){
                    if(!flag)
                        flag = 1;
                    else{
                        if(AI_temp->open && AI_temp->type == CANNON && AI_temp->color != prey_color){
                            eater_color = AI_map[j+k*dj][i+k*di]->color;
                            current_value += local_exchange_flag * value[AI_map[j][i]->num];
                            Apply_Move(AI_map[j+k*dj][i+k*di],j+k*dj,i+k*di,j,i,AI_map[j][i],1);
                            exchange_flag *= -1;
                            if(advantageous_exchange(current_value,eater_color,j,i)){
                                Recover();
                                return true;
                            }
                            Recover();
                        }
                    }
                }
            }
            else
                break;
        }
        exchange_flag = local_exchange_flag;
        current_value = local_current_value;
    }


    if(current_value > 0)
        return true;
    else 
        return false;
}

bool AnqiScene::Safe_Eating(int j,int i,int y, int x){//j,i to y,x
    //check defend vaild
    ChessColor prey_color = AI_map[y][x]->color;
    ChessColor eater_color = AI_map[j][i]->color;
    int current_value = value[AI_map[y][x]->num];
    int local_current_value = current_value;
    Apply_Move(AI_map[j][i],j,i,y,x,AI_map[y][x],1);

    for(auto dir:direction){//general defend
        int dy = dir.first;
        int dx = dir.second;
        if(y+dy >= 0 && y+dy < 4 && x+dx >= 0 && x+dx <8){
            if(AI_map[y+dy][x+dx] && AI_map[y+dy][x+dx]->open && AI_map[y+dy][x+dx]->color == prey_color){
                if(Edible(AI_map[y+dy][x+dx]->type,AI_map[y][x]->type)){
                    current_value -= value[AI_map[y][x]->num];
                    Apply_Move(AI_map[y+dy][x+dx],y+dy,x+dx,y,x,AI_map[y][x],1);
                    if(!advantageous_exchange(current_value,prey_color,y,x)){
                        Recover();
                        Recover();
                        return false;
                    }
                    Recover();
                            
                }
            }
        }
        current_value = local_current_value;
    }


    for(auto dir:direction){//cannon defend
        int flag = 0;
        int dy = dir.first;
        int dx = dir.second;
        for(int k=1;k<8;k++){
            if(y+k*dy >= 0 && y+k*dy < 4 && x+k*dx >= 0 && x+k*dx <8){
                AI_Chess* AI_temp = AI_map[y+k*dy][x+k*dx];
                if(AI_temp){
                    if(!flag)
                        flag = 1;
                    else{
                        if(AI_temp->open && AI_temp->type == CANNON && AI_temp->color == prey_color){
                            Apply_Move(AI_map[y+k*dy][x+k*dx],y+k*dy,x+k*dx,y,x,AI_map[y][x],1);
                            if(!advantageous_exchange(current_value,prey_color,y,x)){
                                Recover();
                                Recover();
                                return false;
                            }
                            Recover();
                        }
                    }
                }
            }
            else
                break;
        }
        current_value = local_current_value;
    }
    Recover();
    return true;
}

bool AnqiScene::Safe_Moving(int j, int i,int y,int x){//j,i to y,x 
    Apply_Move(AI_map[j][i],j,i,y,x,nullptr,2);
    //judge y,x safe or not
    for(auto dir:direction){//check defender from four directions
        int dy = dir.first;
        int dx = dir.second;
        if(y+dy>=0 && y+dy<4 && x+dx>=0 && x+dx<8 ){//general defend
            if(AI_map[y+dy][x+dx] && AI_map[y+dy][x+dx]->open && AI_map[y+dy][x+dx]->color != AI_map[y][x]->color) {
                if(Edible(AI_map[y+dy][x+dx]->type,AI_map[y][x]->type) && Safe_Eating(y+dy,x+dx,y,x) ){
                    Recover();
                    return false;
                }
            }
        }
        int flag = 0;
        for(int k=1;k<8;k++){//cannon defend
            flag = 0;
            if(y+k*dy>=0 && y+k*dy<4 && x+k*dx>=0 && x+k*dx<8){
                AI_Chess* AI_temp = AI_map[y+k*dy][x+k*dx];
                if(AI_temp){
                    if(!flag)
                        flag = 1;
                    else{
                        if(AI_temp->open && AI_temp->type == CANNON && AI_temp->color != AI_map[y][x]->color){
                            if(Safe_Eating(y+k*dy,x+k*dx,y,x)){
                                Recover();
                                return false;
                            }
                        }
                    }
                }
            }
            else
                break;
        }
            
        
    }
    Recover();
    return true;
}
bool AnqiScene::Can_Escape(ChessType attackerType,int j,int i,int y,int x,int depth){//eater at j,i prey at y,x
    if(depth == 10)
        return true;

    if(AI_map[y][x]->type == CANNON)//target is cannon
        return Can_Escape_Cannon(attackerType, j, i, y, x,depth);

    //std::cout << "depth: " << depth << " 500\n";
    //if(!AI_map[y][x]) std::cout << "depth: " << depth << "error1\n";
    //if(!AI_map[j][i]) std::cout << "depth: " << depth << "error2\n";
    for(auto dir:direction){//target is not cannon
        int dy = dir.first;
        int dx = dir.second;
        if(y+dy>=0 && y+dy<4 && x+dx>=0 && x+dx<8){
            if(AI_map[y+dy][x+dx] && AI_map[y+dy][x+dx]->open && AI_map[y][x]->color != AI_map[y+dy][x+dx]->color 
                && Edible(AI_map[y][x]->type,AI_map[y+dy][x+dx]->type) && Safe_Eating(y,x,y+dy,x+dx)){
                //if(!AI_map[y][x]) std::cout << "depth: " << depth << " error3\n";
                //if(!AI_map[j][i]) std::cout << "depth: " << depth << "error4\n";
                Apply_Move(AI_map[y][x],y,x,y+dy,x+dx,AI_map[y+dy][x+dx],1); 
                //if(!AI_map[j][i]) std::cout << "depth: " << depth << "error5\n";
                if(Safe_Moving(j,i,y,x)){//chase
                    //if(!AI_map[j][i]) std::cout << "depth: " << depth << "error6\n";
                    Apply_Move(AI_map[j][i],j,i,y,x,nullptr,2);
                    //if(!AI_map[y][x]) std::cout << "depth: " << depth << "error7\n";
                    if(Can_Escape(attackerType,y,x,y+dy,x+dx,depth+1)){
                        Recover();
                        Recover();
                        //std::cout << "depth: " << depth << " true1\n";
                        return true;
                    }
                    Recover();
                    Recover();
                }
                else{
                    Recover();
                    //std::cout << "depth: " << depth << " true2\n";
                    return true;
                }
            }
            else if(!AI_map[y+dy][x+dx] && Safe_Moving(y,x,y+dy,x+dx)){
                //std::cout << "depth: " << depth << " 600\n";
                
                //if(!AI_map[j][i]) std::cout << "depth: " << depth << " error8\n";
                //if(!AI_map[y][x]) std::cout << "depth: " << depth << " error9\n";
                Apply_Move(AI_map[y][x],y,x,y+dy,x+dx,nullptr,2);
                
                //if(!AI_map[j][i]) std::cout << "depth: " << depth << " error10\n";
                if(Safe_Moving(j,i,y,x)){//chase
                    
                    //if(!AI_map[j][i]) std::cout << "depth: " << depth << " error11\n";
                    Apply_Move(AI_map[j][i],j,i,y,x,nullptr,2);
                    
                    //if(!AI_map[y][x]) std::cout << "depth: " << depth << " error12\n";
                    if(Can_Escape(attackerType,y,x,y+dy,x+dx,depth+1)){
                        Recover();
                        Recover();
                        //std::cout << "depth: " << depth << " true3\n";
                        return true;
                    }
                    Recover();
                    Recover();
                }
                else{
                    Recover();
                    //std::cout << "depth: " << depth << " true4\n";
                    return true;
                }
            }
        }
        
    }
    //std::cout << "depth: " << depth << " false\n";
    return false;
}

bool AnqiScene::Can_Escape_Cannon(ChessType attackerType,int j,int i,int y,int x,int depth){//eater at j,i prey at y,x
    if(depth==10)
        return true;
    //std::cout << "1000\n";
    
    //if(!AI_map[y][x]) std::cout << "depth: " << depth << " error13\n";
                
    //if(!AI_map[y][x]) std::cout << "depth: " << depth << " error14\n";
    for(auto dir:direction){
        int dy = dir.first;
        int dx = dir.second;
        if(y+dy>=0 && y+dy<4 && x+dx>=0 && x+dx<8){
            if(!AI_map[y+dy][x+dx] && Safe_Moving(y,x,y+dy,x+dx)){
                //if(!AI_map[y][x]) std::cout << "depth: " << depth << " error15\n";
                //if(!AI_map[j][i]) std::cout << "depth: " << depth << " error16\n";
                Apply_Move(AI_map[y][x],y,x,y+dy,x+dx,nullptr,2);
                //if(!AI_map[j][i]) std::cout << "depth: " << depth << " error17\n";
                if(Safe_Moving(j,i,y,x)){//chase
                    //if(!AI_map[j][i]) std::cout << "depth: " << depth << " error18\n";
                    Apply_Move(AI_map[j][i],j,i,y,x,nullptr,2);
                    //if(!AI_map[y][x]) std::cout << "depth: " << depth << " error19\n";
                    if(Can_Escape_Cannon(attackerType,y,x,y+dy,x+dx,depth+1)){
                        Recover();
                        Recover();
                        //std::cout << "true\n";
                        return true;
                    }
                    Recover();
                    Recover();
                }
                else{
                    Recover();
                    //std::cout << "true\n";
                    return true;
                }
            }
        }
        int flag = 0;
        for(int k=1;k<8;k++){
            if(y+k*dy>=0 && y+k*dy<4 && x+k*dx>=0 && x+k*dx<8){
                AI_Chess* AI_temp = AI_map[y+k*dy][x+k*dx];
                if(AI_temp){
                    if(!flag)
                        flag = 1;
                    else{
                        if(AI_temp->open && AI_temp->color != AI_map[y][x]->color){
                            if(Safe_Eating(y,x,y+k*dy,x+k*dx)){
                                //std::cout << "true\n";
                                return true;
                            }
                        }
                        break;
                    }
                }
            }
            else
                break;
        }
    }
    //std::cout << "false\n";
    return false;
}

double AnqiScene::evaluate(){
    double AI_score = 0;
    double computer_score = 0,player_score = 0;
    int flag = 0;
    for(int j=0;j<4;j++){
        for(int i=0;i<8;i++){
            
            AI_Chess* AI_temp = AI_map[j][i];
            if(!AI_temp)
                continue;
            else if(!AI_temp->open){
                if(AI_temp->color == color)
                    AI_score -= 9*value[AI_temp->num];
                else
                    AI_score += 9*value[AI_temp->num];
                continue;
            }

            else if(AI_temp->color != color){//computer chess
                AI_score += 10*value[AI_temp->num];
                if(AI_temp->type == CANNON){//cannon
                    for(auto dir:direction){
                        flag = 0;
                        int dj = dir.first;
                        int di = dir.second;
                        for(int k=1;k<8;k++){
                            if(j+k*dj >= 0 && j+k*dj < 4 && i+k*di >= 0 && i+k*di < 8){
                                if(AI_map[j+k*dj][i+k*di]){
                                    if(!flag)
                                        flag = 1;
                                    else{
                                        double p_score = 0,c_score = 0;
                                        if(AI_map[j+k*dj][i+k*di]->color == color && AI_map[j+k*dj][i+k*di]->open){
                                            c_score = value[AI_map[j+dj*k][i+di*k]->num];
                                            if(!Safe_Eating(j,i,j+dj*k,i+di*k)){
                                                p_score = value[10];
                                            }
                                            AI_score += std::max(0.0,c_score-p_score);
                                        }
                                        break;
                                    }
                                }
                            }
                            else
                                break;
                        }
                    }
                }
                else{//not cannon
                    for(auto dir:direction){
                        int dj = dir.first;
                        int di = dir.second;
                        if(j+dj>=0 && j+dj<4 && i+di>=0 && i+di<8){
                            double p_score = 0,c_score = 0;
                            AI_Chess* AI_chess = AI_map[j+dj][i+di];
                            if(AI_chess && AI_chess->open && AI_temp->color != AI_chess->color){
                                if(Edible(AI_temp->type,AI_chess->type) ){
                                    c_score = value[AI_chess->num];
                                    if(!Safe_Eating(j,i,j+dj,i+di)){
                                        p_score = value[AI_temp->num];
                                    }
                                }
                            }
                            AI_score += std::max(0.0,c_score-p_score);
                        }
                    }
                }
            }
            else{//player chess
                AI_score -= 10*value[AI_temp->num];
                if(AI_temp->type == CANNON){//cannon
                    for(auto dir:direction){
                        flag = 0;
                        int dj = dir.first;
                        int di = dir.second;
                        for(int k=1;k<8;k++){
                            if(j+dj*k >= 0 && j+dj*k < 4 && i+di*k >= 0 && i+di*k < 8){
                                AI_Chess* AI_chess = AI_map[j+k*dj][i+k*di];
                                if(AI_chess){
                                    if(!flag)
                                        flag = 1;
                                    else{
                                        double p_score = 0,c_score = 0;
                                        //std::cout << "1----------------\n";
                                        if(AI_temp->color != AI_chess->color && AI_chess->open && !Can_Escape(AI_temp->type,j,i,j+k*dj,i+k*di,0)){
                                            p_score = value[AI_chess->num];
                                            if(!Safe_Eating(j,i,j+dj*k,i+di*k)){
                                                c_score = value[10];
                                            }
                                            //std::cout << "700\n";
                                            AI_score -= std::max(0.0,p_score-c_score);
                                        }
                                        break;
                                    }
                                }
                            }
                            else
                                break;
                        }
                    }
                }
                else{//not cannon
                    for(auto dir:direction){
                        int dj = dir.first;
                        int di = dir.second;
                        if(j+dj>=0 && j+dj<4 && i+di>=0 && i+di<8){
                            double p_score = 0,c_score = 0;
                            AI_Chess* AI_chess = AI_map[j+dj][i+di];
                            if(AI_chess && AI_chess->open && AI_temp->color != AI_chess->color){
                                //std::cout << "2----------------\n";
                                if(!Edible(AI_chess->type,AI_temp->type) && !Can_Escape(AI_temp->type,j,i,j+dj,i+di,0)){
                                    p_score = value[AI_chess->num];
                                    if(!Safe_Eating(j,i,j+dj,i+di)){
                                        c_score = value[AI_temp->num];
                                    }
                                }
                                //std::cout << "800\n";
                            }
                            AI_score -= std::max(0.0,p_score-c_score);
                        }
                    }
                }
            }
        }
    }
    return AI_score;
}


void update_action(int j,int i,int y,int x,int op,int action_score){
    bestAction->j = j;
    bestAction->i = i;
    bestAction->op = op;
    bestAction->y = y;
    bestAction->x = x;
    bestAction->action_score = action_score;
}

void AnqiScene::Apply_Move(AI_Chess* AI_chess,int j,int i,int y,int x,AI_Chess* prey,int op){
    if(!op){
        AI_map[j][i]->open = true;
        RecordStep(AI_chess,{j,i},{y,x},prey,op);
    }
    else{
        RecordStep(AI_chess,{j,i},{y,x},prey,op);
        AI_map[y][x] = AI_map[j][i];
        AI_map[j][i] = nullptr;
    }
    
}

PieceInfo pieces[15] = {
        {BLACK, KING, 0, false},
        {BLACK, KING, 0, false},        // 1
        {BLACK, KNIGHT, 0, false},     // 2
        {BLACK, ELEPHANT, 0, false},   // 3
        {BLACK, CAR, 0, false},        // 4
        {BLACK, HORSE, 0, false},      // 5
        {BLACK, CANNON, 0, true},      // 6
        {BLACK, SOLDIER, 0, false},    // 7
        {RED, KING, -16, false},       // 8
        {RED, KNIGHT, -16, false},     // 9
        {RED, ELEPHANT, -16, false},   // 10
        {RED, CAR, -16, false},        // 11
        {RED, HORSE, -16, false},      // 12
        {RED, CANNON, -16, true},      // 13
        {RED, SOLDIER, -16, false},    // 14
};


double AnqiScene::Minimax(int depth,bool isMaxPlayer,Action* &bestAction){
    if(depth == level||minimax_flag == 1 && isMaxPlayer){
        minimax_flag = 0;
        return evaluate();
    }
    

    double bestScore = isMaxPlayer ? -1e9 : 1e9;

    AI_Chess* AI_chess;
    AI_Chess* AI_temp;

    for(int j=0;j<4;j++){
        for(int i=0;i<8;i++){
            if(!AI_map[j][i] || AI_map[j][i]->open && AI_map[j][i]->color == color && isMaxPlayer 
            || AI_map[j][i]->open && AI_map[j][i]->color != color && !isMaxPlayer){
                continue;
            }
            if(!AI_map[j][i]->open){//flip
                AI_temp = AI_map[j][i];
                double expectedScore = 0;

                // if(unflip <= 10){
                //     for (int k=1;k<15;k++) {
                //         int count = dark[k];
                //         if (count == 0) continue;

                //         double probability = double(count) / unflip;

                //         // 模擬這個 type 被翻開（建立臨時棋子放進 AI_map）
                //         AI_Chess* simulate = new AI_Chess(pieces[k].color, pieces[k].type,true,correspond[pieces[k].color][pieces[k].type]); // 建構棋子
                //         AI_map[j][i] = simulate;
                //         RecordStep(AI_temp->color,AI_temp->type,0,RED,KING,{j,i},{-1,-1},AI_temp->num);
                //         double score = Minimax(depth + 1, !isMaxPlayer, bestAction);
                //         Recover();
                //         delete simulate;

                //         expectedScore += probability * score;
                //     }
                // }
                //else{ 
                    Apply_Move(AI_temp,j,i,-1,-1,nullptr,0);
                    minimax_flag = 1;
                    expectedScore = Minimax(depth + 1, !isMaxPlayer, bestAction);
                    Recover();

                    // if(isMaxPlayer) expectedScore -= 10000;
                    // else expectedScore += 10000;
                //}
                // 比較期望分數

                if ((isMaxPlayer && expectedScore > bestScore) || (!isMaxPlayer && expectedScore < bestScore)) {
                    bestScore = expectedScore;
                    if (depth == 0) 
                        update_action(j, i, -1, -1, 0, bestScore);
                }
            }

            else{//open
                AI_temp = AI_map[j][i];
                if(AI_temp->type == CANNON){//cannon eat
                    for(int a=0;a<4;a++){
                        for(int b=0;b<8;b++){
                            AI_chess = AI_map[a][b];
                            if(AI_chess && AI_chess->open && AI_chess->color!=AI_temp->color && CannonValidEating(a,b,j,i)){
                                Apply_Move(AI_temp,j,i,a,b,AI_map[a][b],1);
                                double score = Minimax(depth + 1, !isMaxPlayer, bestAction);
                                Recover();
                                if ((isMaxPlayer && score > bestScore) || (!isMaxPlayer && score < bestScore)) {
                                    bestScore = score;
                                    if (depth == 0) 
                                        update_action(j,i,a,b,1,bestScore);
                                }

                            }
                        }
                    }
                }

                else{//general eat
                    for(auto dir:direction){
                        int dj = dir.first;
                        int di = dir.second;
                        if(j+dj>=0 && j+dj<4 && i+di>=0 && i+di<8){
                            AI_chess = AI_map[j+dj][i+di];
                            if(AI_chess && AI_chess->open && AI_chess->color!=AI_temp->color && Edible(AI_temp->type,AI_chess->type)){
                                Apply_Move(AI_temp,j,i,j+dj,i+di,AI_map[j+dj][i+di],1);
                                double score = Minimax(depth + 1, !isMaxPlayer, bestAction);
                                Recover();
                                if ((isMaxPlayer && score > bestScore) || (!isMaxPlayer && score < bestScore)) {
                                    bestScore = score;
                                    if (depth == 0) 
                                        update_action(j,i,j+dj,i+di,1,bestScore);
                                }
                            }
                        }
                    }
                }
                //move
                for(auto dir:direction){
                    int mark = 0;
                    int dj = dir.first;
                    int di = dir.second;
                    if(j+dj>=0 && j+dj<4 && i+di>=0 && i+di<8){
                        AI_chess = AI_map[j+dj][i+di];
                        int& attack_step = computer->attack_step;
                        if(!AI_chess){
                            update_chessboard(j,i,j+dj,i+di,2);
                            if(Target(j,i) && Repeat()){
                                update_chessboard(j+dj,i+di,j,i,2);
                                if(attack_step == 5)
                                    continue;
                                else
                                    mark = 1;
                            }
                            Apply_Move(AI_temp,j,i,j+dj,i+di,nullptr,2);
                            double score = Minimax(depth + 1, !isMaxPlayer, bestAction);
                            Recover();
                            if ((isMaxPlayer && score > bestScore) || (!isMaxPlayer && score < bestScore)) {
                                bestScore = score;
                                if (depth == 0) 
                                    update_action(j,i,j+dj,i+di,2+mark,bestScore);
                            }
                        }
                    }
                }
            }
        }
    }
    return bestScore;
    
}


void AnqiScene::AI_Act(){

    
    
    Minimax(0,true,bestAction);


    if(bestAction->op==0){
        harmony_step = 0;
        AI_flip(bestAction->j,bestAction->i);
        ChangeRound();
    }
    else if(bestAction->op==1){
        harmony_step = 0;
        AI_eat(bestAction->j,bestAction->i,bestAction->y,bestAction->x);
    }
    else{
        AI_move(bestAction->j,bestAction->i,bestAction->y,bestAction->x,bestAction->op);
        harmony_step++;
    }

    //std::cout << "op: "<< bestAction->op << " j: "<< bestAction->j << " i: "<< bestAction->i << " y: "<<bestAction->y << " x: "<< bestAction->x << "\n";
    
    layer = 0;
    
    flag->preDelay_flag = 0;

}
void AnqiScene::Update_dark(int num,int op){
    int flag = 0;
    int k;
    if(num > 16){
        num -= 16;
        flag = 1;
    }
    if(num == 1)    
        k = 1;
    else if(num >= 12)
        k = 7;
    else
        k = (num+2)/2;

    int index = flag*7 + k;
        dark[index] += op;
}

bool AnqiScene::Repeat(){
    int repeat_flag;
    for(auto h:history){
        repeat_flag = 1;
        for(int j=0;j<4 && repeat_flag;j++){
            for(int i=0;i<8;i++){
                if(h[j][i] != chessBoard[j][i]){
                    repeat_flag = 0;
                    break;
                }
            }
        }
        if(repeat_flag){
            return true;
        }
    }

    history.push_back(chessBoard);
    return false;
}

void AnqiScene::Update(float deltaTime) {

    if(is_single_player)
        Update_single(deltaTime);
    else
        Update_double(deltaTime);
}
void AnqiScene::Update_single(float deltaTime){
    if(player && player->remain==0 && !flag->change_scene_flag){
        if(unflip){
            show_all_chess();
            tick->show_chess_tick = 0;
        }
        
        CalculateScore((color==RED)?BLACK:RED);
        flag->change_scene_flag = 1;
    }
    else if(computer && computer->remain==0 && !flag->change_scene_flag){
        if(unflip){
            show_all_chess();
            tick->show_chess_tick = 0;
        }
            
        CalculateScore((color==RED)?RED:BLACK);
        flag->change_scene_flag = 2;
    }
    if(tick->pointer_tick>30)
        tick->pointer_tick = -30;

    tick->pointer_tick++;

    if(!flag->turn_flag && tick->turn_tick < 60){
        int &prolong_time = player->prolong_time;
        int max_prolong_time = is_single_player ? 4*level-14 : 3 ;

        if(level > 2 && prolong_time < max_prolong_time){
            prolong_time++;
            tick->turn_tick += 600;
            StopSign();
            if(prolong_time == max_prolong_time)
                sign->prolong_warn->Text = "延長次數已用完";
            else
                sign->prolong_warn->Text = "已使用" + std::to_string(prolong_time) + "次延長";
            
            sign->prolong_warn->Color = al_map_rgba(255,255,255,255);
            
            tick->prolong_tick = 0;
        }
        else{
            CalculateScore((color==RED)?BLACK:RED);
            Engine::GameEngine::GetInstance().ChangeScene("computer_win");
        }
        
    }

    if(turn!=UNKNOWNED){
        sign->remain_red->Text = "剩餘:" + std::to_string(red->remain);
        sign->remain_black->Text = "剩餘:" + std::to_string(black->remain);
        if(level>2)
            sign->prolong->Text = "延長" + std::to_string(player->prolong_time) + "/" + std::to_string(4*level-14);

        pointer->Position.y = 325 + 10*sin(2*pi*tick->pointer_tick/30);
        pointer->Position.x = !flag->turn_flag ? p1_x : p2_x;

        if(!flag->turn_flag){
            tick->turn_tick--;
            sign->thinking->Color = al_map_rgba(255,255,51,0);
            sign->timer->Text = std::to_string(tick->turn_tick/60);
            sign->timer->Color = al_map_rgba(223, 21, 22,255);
        }
        else{
            tick->thinking_tick++;
            if(tick->thinking_tick > 119)
                tick->thinking_tick = 0;
            sign->timer->Color = al_map_rgba(223,21,22,0);
            sign->thinking->Color = al_map_rgba(255,255,51,255);
            std::string think = "思考中";
            for(int i=0;i<tick->thinking_tick/30;i++)
                think += ".";
            sign->thinking->Text = think;
        }
    }
    
    if(tick->prolong_tick > 45)
        sign->prolong_warn->Color = al_map_rgba(255,255,255,0);

    if(harmony_step == 50 && tick->harmony_tick > 30)
        Engine::GameEngine::GetInstance().ChangeScene("harmony");

    
    if(tick->show_chess_tick > 60){
        if(flag->change_scene_flag == 1){
            Engine::GameEngine::GetInstance().ChangeScene("computer_win");
            flag->change_scene_flag = 5;
        }
        else if(flag->change_scene_flag == 2){
            Engine::GameEngine::GetInstance().ChangeScene("player_win");
            flag->change_scene_flag = 5;
        }
    }
    tick->prolong_tick++;
    tick->show_chess_tick++;
    tick->AI_tick++;
    tick->preDelay_tick++;
    tick->harmony_tick++;
    tick->attack_tick++;


    if(tick->attack_tick > 45)
        sign->attack_warn->Color = al_map_rgba(255,255,255,0);

    if(tick->harmony_tick > 45)
        sign->harmony_warn->Color = al_map_rgba(255,255,255,0);

    if(tick->prolong_tick > 10000000)
        tick->prolong_tick = 0;

    if(tick->show_chess_tick > 10000000)
        tick->show_chess_tick = 0;

    if(tick->AI_tick > 10000000)
        tick->AI_tick = 0;

    if(tick->preDelay_tick > 10000000)
        tick->preDelay_tick = 0;

    if(tick->harmony_tick>10000000)
        tick->harmony_tick = 0;

    if(tick->attack_tick>10000000)
        tick->attack_tick = 0;

    if(flag->AI_moveFlag && tick->AI_tick > 30){
        if(AI_op == 1){
            ChessGroup->RemoveObject(chessPositions[AI_y][AI_x]->GetObjectIterator());
            AI_op = 0;
            player->remain--;
        }
        else if(AI_op == 3){
            sign->attack_warn->Text = "長捉禁手" + std::to_string(computer->attack_step) + "/5";
            sign->attack_warn->Color = al_map_rgba(255,255,255,255);
            tick->attack_tick = 0;
        }
        Go(pos,AI_y,AI_x);
        RecordTemp(chessPositions[AI_y][AI_x],AI_x,AI_y);
        flag->AI_moveFlag = 0;
    }

    if(tick->preDelay_tick > 30 && flag->preDelay_flag && !flag->change_scene_flag){
        AI_Act();
    }
    
    ALLEGRO_MOUSE_STATE mouse;
    al_get_mouse_state(&mouse);
    bool mouseNowDown = al_mouse_button_down(&mouse, 1); // Left mouse button

    mouseJustClicked = (mouseNowDown && !lastMouseDown);  // Rising edge
    lastMouseDown = mouseNowDown;
        for(int j=0;j<4;j++){
            for(int i=0;i<8;i++){
                Chess* chess = chessPositions[j][i];
                Engine::Point pos;
                
                if(!chess){
                    pos.x = 635+i*200;
                    pos.y = 600+j*200;
                }
                if(!flag->AI_moveFlag && !flag->preDelay_flag && chess && ClickCheck(chess->Position)&& mouseJustClicked && harmony_step!=50){
                    if(!chess->Check_open()){
                        update_chessboard(j,i,-1,-1,0);
                        harmony_step = 0;
                        chess->Open();
                        unflip--;
                        Update_dark(chess->getNumber(),-1);
                        if(turn==UNKNOWNED){
                            color = chess->getColor();
                            player = (color==RED)?red:black;
                            computer = (color==RED)?black:red;
                            InitializeRound(chess->getColor());
                        }
                        else{
                            player->attack_step = 0;
                            ChangeRound();
                            temp->ClickCancel();
                            temp = nullptr;
                        }
                        update_information(j,i,-1,-1,0);
                        flag->preDelay_flag = 1;
                        tick->preDelay_tick = 0;
                    }
                    else{
                        if(temp && harmony_step!=50){//have select
                            if(chess->getColor()==color){
                                temp->ClickCancel();
                                RecordTemp(chess,i,j);
                            }
                            else if(temp->getType()==CANNON && temp->getColor()==color){//diff color
                                if(CannonValidEating(j,i,temp_y,temp_x)){
                                    update_chessboard(temp_y,temp_x,j,i,1);
                                    CheckEatCannon(chess);
                                    int& attack_step = player->attack_step;
                                    attack_step = 0;
                                    pos = chess->Position;
                                    computer->remain--;
                                    chessRecord[chess->getNumber()] = nullptr;
                                    ChessGroup->RemoveObject(chess->GetObjectIterator());
                                    Go(pos,j,i);
                                    update_information(temp_y,temp_x,j,i,1);
                                    flag->preDelay_flag = 1;
                                    tick->preDelay_tick = 0;
                                    harmony_step = 0;
                                }
                            }
                            else if(abs(temp_y-j)+abs(temp_x-i)==1 && temp->getColor()==color){//diff color
                                if(Edible(temp->getType(),chess->getType())){
                                    update_chessboard(temp_y,temp_x,j,i,1);
                                    CheckEatCannon(chess);
                                    int& attack_step = player->attack_step;
                                    attack_step = 0;
                                    pos = chess->Position;
                                    computer->remain--;
                                    chessRecord[chess->getNumber()] = nullptr;
                                    ChessGroup->RemoveObject(chess->GetObjectIterator());
                                    Go(pos,j,i);
                                    update_information(temp_y,temp_x,j,i,1);
                                    flag->preDelay_flag = 1;
                                    tick->preDelay_tick = 0;
                                    harmony_step = 0;
                                }
                                
                                
                            }
                            
                        }
                        else{//have not select
                            if(!flag->preDelay_flag && chess->getColor() == color){
                                RecordTemp(chess,i,j);
                            }
                        }
                    }
                }
                else if(temp && temp->getColor() == color && !flag->AI_moveFlag && !flag->preDelay_flag && (!chess) && ClickCheck(pos) && mouseJustClicked && harmony_step!=50){
                    int &attack_step = player->attack_step;
                    if(abs(temp_y-j)+abs(temp_x-i) == 1){
                        std::cout << "100\n";
                        update_chessboard(temp_y,temp_x,j,i,2);
                        if(Target(j,i) && Repeat()){
                            std::cout << "200\n";
                            if(attack_step == 5){
                                sign->attack_warn->Text = "長捉禁手5/5,無效移動!!!";
                                update_chessboard(j,i,temp_y,temp_x,2);
                            }
                            else{
                                std::cout << "300\n";
                                attack_step++;
                                sign->attack_warn->Text = "長捉禁手" + std::to_string(attack_step) + "/5";
                                std::cout << "400\n";
                                Go(pos,j,i);//move
                                std::cout << "500\n";
                                harmony_step++;
                                update_information(temp_y,temp_x,j,i,2);
                                flag->preDelay_flag = 1;
                                tick->preDelay_tick = 0;
                            }
                            StopSign();
                            sign->attack_warn->Color = al_map_rgba(255,255,255,255);
                            tick->attack_tick = 0;
                        }
                        else{
                            std::cout << "600\n";
                            Go(pos,j,i);//move
                            std::cout << "700\n";
                            harmony_step++;
                            attack_step = 0;
                            update_information(temp_y,temp_x,j,i,2);
                            std::cout << "800\n";
                            flag->preDelay_flag = 1;
                            tick->preDelay_tick = 0;
                            if(harmony_step >= 20){
                                StopSign();
                                sign->harmony_warn->Color = al_map_rgba(255,255,255,255);
                                sign->harmony_warn->Text = "空步判和" + std::to_string(harmony_step) + "/50";
                                tick->harmony_tick = 0;
                            }
                        }
                    }
                }
            }  
        }
    
    
}

void AnqiScene::Update_double(float deltaTime) {
    if(turn!=UNKNOWNED){
        sign->remain_red->Text = "剩餘:" + std::to_string(red->remain);
        sign->remain_black->Text = "剩餘:" + std::to_string(black->remain);
    }

    if(tick->show_chess_tick > 60){
        if(flag->change_scene_flag == 3){
            flag->change_scene_flag = 5;
            Engine::GameEngine::GetInstance().ChangeScene("red_win");
        }
        else if(flag->change_scene_flag == 4){
            flag->change_scene_flag = 5;
            Engine::GameEngine::GetInstance().ChangeScene("black_win");
        }
    }
    if (tick->turn_tick == 59) {
        int &prolong_time = (turn==RED_TURN) ? red->prolong_time : black->prolong_time;
        if(prolong_time < 3){
            prolong_time++;
            tick->turn_tick += 600;
            StopSign();
            if(prolong_time==3)
                sign->prolong_warn->Text = "延長次數已用完";
            else
                sign->prolong_warn->Text = "已使用" + std::to_string(prolong_time) + "次延長";
            
            sign->prolong_warn->Color = al_map_rgba(255,255,255,255);
            
            tick->prolong_tick = 0;
        }
        else{
            Engine::GameEngine::GetInstance().ChangeScene(turn == RED_TURN ? "black_win" : "red_win");
            CalculateScore(turn == RED_TURN ? BLACK : RED);
        }
    }

    if(tick->prolong_tick > 45){
        sign->prolong_warn->Color = al_map_rgba(255,255,255,0);
    }

    if(turn!=UNKNOWNED){
        tick->turn_tick--;
        auto& player = (turn == RED_TURN) ? *red : *black;
        //sign->request_peace->Text = "求和" + std::to_string(player.request_time) + "/2";
        sign->prolong->Text = "延長" + std::to_string(player.prolong_time) + "/3";
        //sign->regret->Text = "悔棋" + std::to_string(player.regret_time) + "/3";
        pointer->Position.y = 325 + 10*sin(2*pi*tick->pointer_tick/30);
        sign->timer->Text = std::to_string(tick->turn_tick/60);
        bool isFirstTurn = (turn == first_turn);
        pointer->Position.x = isFirstTurn ? p1_x : p2_x;
        sign->timer->Position.x = isFirstTurn ? t1_x : t2_x;
    }
    
    
    if(tick->pointer_tick>30)
        tick->pointer_tick = -30;

    tick->show_chess_tick++;
    tick->preDelay_tick++;
    tick->peace_tick++;
    tick->pointer_tick++;
    tick->attack_tick++;
    tick->harmony_tick++;
    tick->prolong_tick++;

    if(tick->harmony_tick > 10000000)
        tick->harmony_tick = 0;

    if(tick->attack_tick > 10000000)
        tick->attack_tick = 0;

    if(tick->prolong_tick > 10000000)
        tick->prolong_tick = 0;

    if(tick->show_chess_tick > 10000000)
        tick->show_chess_tick = 0;

    if(tick->attack_tick > 45)
        sign->attack_warn->Color = al_map_rgba(255,255,255,0);
    
    if(tick->harmony_tick > 45)
        sign->harmony_warn->Color = al_map_rgba(255,255,255,0);
        
    if(harmony_step == 50 && tick->harmony_tick > 30)
        Engine::GameEngine::GetInstance().ChangeScene("harmony");

    if(black->remain==0 && !flag->change_scene_flag){
        if(unflip)
            show_all_chess();
        CalculateScore(RED);
        tick->show_chess_tick = 0;
        flag->change_scene_flag = 3;
    }
    else if(red->remain==0 && !flag->change_scene_flag){
        if(unflip)
            show_all_chess();
        CalculateScore(BLACK);
        tick->show_chess_tick = 0;
        flag->change_scene_flag = 4;
    }
    // Update all chess pieces
    ALLEGRO_MOUSE_STATE mouse;
    al_get_mouse_state(&mouse);
    bool mouseNowDown = al_mouse_button_down(&mouse, 1); // Left mouse button

    mouseJustClicked = (mouseNowDown && !lastMouseDown);  // Rising edge
    lastMouseDown = mouseNowDown;
    
    
    for(int j=0;j<4;j++){
        for(int i=0;i<8;i++){
            Chess* chess = chessPositions[j][i];
            Engine::Point pos;
            
            if(!chess){// move's position
                pos.x = 635+i*200;
                pos.y = 600+j*200;
            }
            if(chess && ClickCheck(chess->Position) && mouseJustClicked && harmony_step != 50){//flip
                if(!chess->Check_open()){
                    update_chessboard(j,i,-1,-1,0);
                    harmony_step = 0;
                    int& attack_step = (turn==RED_TURN)?red->attack_step:black->attack_step;
                    attack_step = 0;
                    chess->Open();
                    unflip--;
                    if(turn==UNKNOWNED)
                        InitializeRound(chess->getColor());
                    else{
                        ChangeRound();
                        temp->ClickCancel();
                    }

                    RecordTemp(chess,i,j);
                }
                else{
                    if(chess->getColor() == color){//change select
                        temp->ClickCancel();
                        RecordTemp(chess,i,j);
                    }
                    else if(temp->getColor() == color && chess->getColor() != color && harmony_step != 50){
                        
                        if(temp->getType()==CANNON){//diff color
                            if(CannonValidEating(j,i,temp_y,temp_x)){
                                CheckEatCannon(chess);
                                int& attack_step = (turn==RED_TURN)?red->attack_step:black->attack_step;
                                attack_step = 0;
                                pos = chess->Position;
                                update_chessboard(temp_y,temp_x,j,i,1);
                                if(chess->getColor()==RED)
                                    red->remain--;
                                else
                                    black->remain--;
                                chessRecord[chess->getNumber()] = nullptr;
                                ChessGroup->RemoveObject(chess->GetObjectIterator());
                                Go(pos,j,i);
                                harmony_step = 0;
                            }
                        }
                        else if(abs(temp_y-j)+abs(temp_x-i) == 1){//diff color
                            if(Edible(temp->getType(),chess->getType())){
                                CheckEatCannon(chess);
                                int& attack_step = (turn==RED_TURN)?red->attack_step:black->attack_step;
                                attack_step = 0;
                                pos = chess->Position;
                                update_chessboard(temp_y,temp_x,j,i,1);
                                if(chess->getColor()==RED)
                                    red->remain--;
                                else
                                    black->remain--;
                                chessRecord[chess->getNumber()] = nullptr;
                                ChessGroup->RemoveObject(chess->GetObjectIterator());

                                Go(pos,j,i);
                                harmony_step = 0;
                            }
                               
                            
                        }
                        
                    }
                    //have not select
                        
                    
                }
            }
            else if(temp && temp->getColor() == color && (!chess) && ClickCheck(pos) && mouseJustClicked && harmony_step != 50){
                int &attack_step = (temp->getColor()==RED)?red->attack_step:black->attack_step;
                if(abs(temp_y-j)+abs(temp_x-i)==1){//!(attack_step==10&&temp==attacker)
                    update_chessboard(temp_y,temp_x,j,i,2);
                    if(Target(j,i) && Repeat()){
                        if(attack_step == 5){
                            sign->attack_warn->Text = "長捉禁手5/5,無效移動!!!";
                            update_chessboard(j,i,temp_y,temp_x,2);
                        }
                        else{
                            attack_step++;
                            sign->attack_warn->Text = "長捉禁手" + std::to_string(attack_step) + "/5";
                            Go(pos,j,i);//move
                            harmony_step++;
                        }
                        StopSign();
                        sign->attack_warn->Color = al_map_rgba(255,255,255,255);
                        tick->attack_tick = 0;
                    }
                    else{
                        Go(pos,j,i);//move
                        harmony_step++;
                        attack_step = 0;
                        if(harmony_step >= 20){
                            StopSign();
                            sign->harmony_warn->Color = al_map_rgba(255,255,255,255);
                            sign->harmony_warn->Text = "空步判和" + std::to_string(harmony_step) + "/50";
                            tick->harmony_tick = 0;
                        }
                    }
                        
                    
                    
                    
                }
            }
        }  
    }

}
bool CheckCannon2(int j,int i,int dj,int di);
bool CheckCannon1(int op,int j,int i,int dj,int di,int des_y,int des_x);

bool AnqiScene::CannonTarget(int des_y,int des_x,bool isCannon){

    ChessColor color = temp->getColor();

    Chess* cannon1 = (color==RED)? red->cannon1 : black->cannon1;
    Chess* cannon2 = (color==RED) ? red->cannon2 : black->cannon2;
    std::vector<Chess*> cannon = {cannon1,cannon2};

    if(!isCannon){//temp not cannon
        for(auto k:cannon){
            if(k && k->Check_open()){
                int j = (k->Position.y - 600) / 200;//cannon y
                int i = (k->Position.x - 635) / 200;//cannon x
                if(temp_y==j && des_y!=j){//move away
                    int di = (des_x-i>0)?1:-1;
                    if(CheckCannon1(0,j,i,0,di,des_y,des_x))
                        return true;
                }
                else if(temp_x==i && des_x!=i){//move away
                    int dj = (des_y-j>0)?1:-1;
                    if(CheckCannon1(0,j,i,dj,0,des_y,des_x))
                        return true;
                }
                else if(des_y==j && temp_y!=j){//move in
                    int di = (des_x-i>0)?1:-1;
                    if(CheckCannon1(1,j,i,0,di,des_y,des_x))
                        return true;
                }
                else if(des_x==i && temp_x!=i){//move in
                    int dj = (des_y-j>0)?1:-1;
                    if(CheckCannon1(1,j,i,dj,0,des_y,des_x))
                        return true;
                }

            }
        }
    }

    else{//temp is cannon
        for(auto k:cannon){
            if(k && k->Check_open()){
                int j = (k->Position.y - 600) / 200;//cannon y
                int i = (k->Position.x - 635) / 200;//cannon x
                if(temp==k){
                    for(auto dir:direction){
                        int dj = dir.first;
                        int di = dir.second;
                        if(CheckCannon2(des_y,des_x,dj,di))
                            return true;
                    }
                }
                else if(temp_y==j && des_y!=j){//move away
                    int di = (des_x-i>0)?1:-1;
                    if(CheckCannon1(0,j,i,0,di,des_y,des_x))
                        return true;
                }
                else if(temp_x==i && des_x!=i){//move away
                    int dj = (des_y-j>0)?1:-1;
                    if(CheckCannon1(0,j,i,dj,0,des_y,des_x))
                        return true;
                }
                else if(des_y==j && temp_y!=j){//move in
                    int di = (des_x-i>0)?1:-1;
                    if(CheckCannon1(1,j,i,0,di,des_y,des_x))
                        return true;
                }
                else if(des_x==i && temp_x!=i){//move in
                    int dj = (des_y-j>0)?1:-1;
                    if(CheckCannon1(1,j,i,dj,0,des_y,des_x))
                        return true;
                }

            }
        }
    }
    return false;
    

}

bool CheckCannon1(int op,int j,int i,int dj,int di,int des_y,int des_x){
    //op==0:move away, op==1:move in
    //temp is not cannon
    int flag = 0;
    if(dj!=0){//j change
        for(int k=1 ;j+dj*k>=0 && j+dj*k<4 ;k++){
            Chess* chess = chessPositions[j+dj*k][i];
            if(op && (chess||j+dj*k == des_y) || !op && chess && chess!=temp){
                if(flag && chess->Check_open()){
                    return true;
                }
                flag = 1;
            }
        }
    }
    else{//i change
        for(int k=1;i+di*k>=0&&i+di*k<8;k++){
            Chess* chess = chessPositions[j][i+di*k];
            if(op && (chess||i+di*k == des_x) || !op && chess && chess!=temp){
                if(flag && chess->Check_open()){
                    return true;
                }
                flag = 1;
            }
        }
    }
    return false;
        
}

bool CheckCannon2(int j,int i,int dj,int di){//temp==cannon
    //temp is cannon
    int flag = 0;
    if(dj!=0){//change j
        for(int k=1 ;j+dj*k>=0 && j+dj*k<4 ;k++){
            Chess* chess = chessPositions[j+dj*k][i];
            if(chess && chess!=temp){
                if(flag){
                    if(chess->Check_open())
                        return true;
                }
                flag = 1;
            }
        }
    }
    else{//change i
        for(int k=1;i+di*k>=0 && i+di*k<8;k++){
            Chess* chess = chessPositions[j][i+di*k];
            if(chess && chess!=temp){
                if(flag){
                    if(chess->Check_open())
                        return true;
                }
                flag = 1;
            }
        }
    }
    return false;

}

bool AnqiScene::Target(int j,int i){
    std::cout << "1000\n";
    ChessColor color = temp->getColor();
    
    if(color==RED && black->remain==1)
        return false;

    else if(color==BLACK && red->remain==1)
        return false;

    std::cout << "1100\n";
    if(temp->getType()!=CANNON){//temp not cannon
        for(auto dir:direction){
            int dj = dir.first;
            int di = dir.second;
            if(j+dj>=0 && j+dj<4 && i+di>=0 && i+di<8){
                Chess* chess = chessPositions[j+dj][i+di];
                if(chess && chess->Check_open() && chess->getColor() != color && Edible(temp->getType(),chess->getType()))
                    return true;
            }
        }
        std::cout << "1400\n";

        if(CannonTarget(j,i,false))
            return true;
        std::cout << "1300\n";
    }
    else{//temp is cannon
        if(CannonTarget(j,i,true))
            return true;
    }

    return false;
}


void AnqiScene::Terminate() {
    AudioHelper::StopBGM(bgmId);
    IScene::Terminate();
}

bool AnqiScene::ClickCheck(Engine::Point pos){
    // Check if the chess piece is clicked
    ALLEGRO_MOUSE_STATE mouse;
    al_get_mouse_state(&mouse);
    if(al_mouse_button_down(&mouse, 1)){
        if (pos.x - 90 <= Engine::GameEngine::GetInstance().GetMousePosition().x &&
        Engine::GameEngine::GetInstance().GetMousePosition().x <= pos.x + 90 &&
        pos.y - 90 <= Engine::GameEngine::GetInstance().GetMousePosition().y &&
        Engine::GameEngine::GetInstance().GetMousePosition().y <= pos.y + 90) {
            return true;
        }
        else{
            return false;
        }
    }
    return false;
}

std::string AnqiScene::GetImage(ChessColor color,ChessType type){
    std::string s = "";
    if(color==RED)
        s += "red_";
    else 
        s += "black_";

    if(type==KING)
        s += "king";
    else if(type==KNIGHT)
        s += "knight";
    else if(type==ELEPHANT)
        s += "elephant";
    else if(type==CAR)
        s += "car";
    else if(type==HORSE)
        s += "horse";
    else if(type==CANNON)
        s += "cannon";
    else
        s += "soldier";
    if(decide)
        s += std::to_string(decide);
    s += ".png";

    return s;

}

void AnqiScene::ButtonClick(int id){
    if(id==1){
        if(is_single_player){
            CalculateScore((color==RED)?BLACK:RED);
            Engine::GameEngine::GetInstance().ChangeScene("computer_win");
        }
        else if(turn == RED_TURN){
            CalculateScore(BLACK);
            Engine::GameEngine::GetInstance().ChangeScene("black_win");
        }
        else{
            CalculateScore(RED);
            Engine::GameEngine::GetInstance().ChangeScene("red_win");
        }
            
    }
    
}