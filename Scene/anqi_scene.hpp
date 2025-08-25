#ifndef PlaySCENE_HPP
#define PlaySCENE_HPP


#include <allegro5/allegro_audio.h>
#include <list>
#include <memory>
#include <utility>
#include <vector>
#include <set>
#include <string>
#include "Engine/IScene.hpp"
#include "Engine/Point.hpp"
#include "UI/Component/Label.hpp"
#include "Chess/chess.hpp"
extern bool mouseJustClicked;
extern bool lastMouseDown;
extern bool is_single_player;
extern float pi;
extern int score;

struct PieceInfo {
    ChessColor color;
    ChessType type;
    int id_adjust; // order裡面的ID要調整多少給Chess建構子
    bool isCannon;
};

class Action{
    public:
        int action_score;
        Chess* chess;
        int j;//current row
        int i;//current column
        int y;//future row
        int x;//future column
        int op;
        //op = 0 flip
        //op = 1 eat
        //op = 2 move
};

class Manage{
    public:
        int attack_step;
        int remain;
        int request_time;
        int regret_time;
        int prolong_time;
        Chess* attacker;
        Chess* cannon1;
        Chess* cannon2;
        std::set<Chess*> attack_target;

};
class Tick{
    public:
        int show_chess_tick;
        int preDelay_tick;
        int AI_tick;
        int attack_tick;
        int turn_tick;
        int prolong_tick;
        int harmony_tick;
        int pointer_tick;
        int peace_tick;
        int regret_tick;
        int regret_invalid_tick;
        int thinking_tick;
};
class Flag{
    public:
        int change_scene_flag;
        int preDelay_flag;
        int AI_actionFlag;
        int AI_moveFlag;
        int harmony_flag;
        int attack_flag;
        int attack_mark;
        int turn_flag;
};
class Sign{
    public:
        Engine::Label* remain_red;
        Engine::Label* remain_black;
        Engine::Label* harmony_warn;
        Engine::Label* attack_warn;
        Engine::Label* prolong_warn;
        Engine::Label* regret_warn;
        Engine::Label* timer;
        Engine::Label* prolong;
        Engine::Label* thinking;
};
class AnqiRecord{
    public:
        AI_Chess* eater;
        int operation;
        std::pair<int,int> start;
        std::pair<int,int> destination;
        AI_Chess* prey;
};
namespace Engine {
    class Group;
    class Image;
    class Label;
    class Sprite;
}   // namespace Engine



class AnqiScene final : public Engine::IScene {
public:
    ALLEGRO_SAMPLE_ID bgmId;
    Group* ChessGroup;
    Group* UIGroup;
    Engine::Label *UIBlack;
    Engine::Label *UIRed;
    explicit AnqiScene() = default;
    void Initialize() override;
    void Terminate() override;
    void Update(float deltaTime) override;
    void Update_single(float deltatime);
    void Update_double(float deltatime);
    bool ClickCheck(Engine::Point pos);
    void ChangeRound();
    void InitializeRound(ChessColor color);
    void RecordTemp(Chess* chess,int i,int j);
    bool CannonValidEating(int j,int i,int temp_y,int temp_x);
    void Go(Engine::Point pos,int j,int i);
    void ConstructUI(ChessColor color);
    void ButtonClick(int id);
    bool Target(int j,int i);
    bool Edible(ChessType eater,ChessType prey);
    bool Edible_greater(ChessType eater,ChessType prey);
    void Recover();
    std::string GetImage(ChessColor color,ChessType type);
    bool CannonTarget(int j,int i,bool isCannon);
    void StopSign();
    void CalculateScore(ChessColor color);
    void AI_eat(int j,int i,int y,int x);
    void AI_move(int j,int i,int y,int x,int op);
    void AI_flip(int j,int i);
    void AI_Act();
    double Minimax(int depth,bool isMaxPlayer,Action* &bestAction);
    void show_all_chess();
    void Apply_Move(AI_Chess* AI_chess,int j,int i,int y,int x,AI_Chess* prey,int op);
    void Update_dark(int num,int op);
    bool Can_Escape(ChessType type,int j,int i,int y,int x,int depth);
    double evaluate();
    bool Safe_Eating(int j,int i,int y, int x);
    bool advantageous_exchange(int current_value,ChessColor prey_color,int j,int i);
    bool Safe_Moving(int j, int i,int y,int x);
    bool Can_Escape_Cannon(ChessType attackerType,int j,int i,int y,int x,int depth);
    bool Repeat();

    // void PlayOnClick(int stage);
    // void SettingsOnClick(int stage);
};
#endif


