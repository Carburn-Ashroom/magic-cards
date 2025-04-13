#ifndef CARDS_H
#define CARDS_H

#include <QPixmap>
#include <QPainter>
#include <QPoint>
#include <vector>

#include "config.h"

namespace Cards {       // 该名字空间负责操作扑克牌

    using std::array;
    using std::vector;

    struct Negative_error {};       // 次数为负数
    struct Little_error {};     // 数字非正
    struct Large_error {};      // 数字过大

    class Card {        // Card类是一张可移动的扑克牌
    public:
        Card(QPoint pos, int id, bool show, int speed =card_speed, int quick_speed =card_sland_speed, int width =card_width, const QString& hide =card_hide_imag, int select_mov =card_select, int fspeed =card_fold_speed) : c_id{id}, imag{card_file.arg(id)}, hide_imag{hide}, p{pos}, to_pos{pos}, dpos{Card_display::top}, sp{speed}, quick{quick_speed}, ratio{static_cast<double>(width)/imag.width()}, display{show}, half{}, choose_mov{select_mov}, choose{}, fold_angle{}, fold_speed{fspeed}, mirrored{} { }
        void del(int y =card_delete);       // 删除这张卡牌
        bool moving() const { return (choose and p.x()==to_pos.x()) ? false : p!=to_pos or fold_angle!=0; }
        void update();      // 更新卡牌位置
        void draw(QPixmap* dest, bool fold =false) const;       // 绘制
        void move_to(QPoint pos) { to_pos = pos; }
        QPoint half_pos() const { return (p+QPointF{0,imag.height()*ratio/2}).toPoint(); }
        void cut() { half = true; }
        void go_to(QPoint pos);     // 超时空传送（没有动画）
        void select(bool chosen =true);     // 选择卡牌并移位，若chosen为false则保持移位取消选择
        void cancel_select();       // 不仅取消选择，还取消移位
        void show() { display = true; }
        void hide() { display = false; }
        void bottom() { dpos = Card_display::bottom; }
        void mirror(bool mir) { mirrored = mir; }
        bool operator!=(const Card& obj) const { return c_id!=obj.c_id; }
        void fold() { fold_angle = (fold_speed=abs(fold_speed)); }
        bool folding() const { return radius()<0; }
    private:
        int c_id;
        QImage imag;
        QImage hide_imag;
        QPointF p;
        QPointF to_pos;
        Card_display dpos;
        int sp;
        int quick;
        double ratio;
        bool display;
        bool half;
        int choose_mov;
        bool choose;
        int fold_angle;
        int fold_speed;
        bool mirrored;
        void move_with_speed(int speed);        // 以对应速度移动
        void draw_fold(QRectF dest_rect, QRectF src_rect, const QImage& srce, QPainter& pa, bool only_fold) const;      // 绘制正在折叠的卡牌
        double radius() const { return cos(oth_pi*fold_angle/180); }
    };
    
    class Cards_admin {     // Cards_admin类是一个魔术师
    public:
        explicit Cards_admin(QPoint pos =cards_pos, bool show =card_show, int interval =cards_gap) : p{pos}, gap{interval}, roll_times{} { spawn_card(show); }
        void split(bool half =card_cut);        // 撕开
        void rand();        // 随机排列卡牌
        Card take_first() { return cards.takeFirst(); }
        void roll(int times);       // 将第一张牌放到最后循环times次
        void insert(int count, int index);      // 将count张卡牌拿出来，然后插在index处。index不能在最后
        void insert_back(int count);        // 强制将count张卡牌插入最后
        QList<Card> del(int count);     // 拿出前count张卡牌
        const Card& see() const { return cards[0]; }
        void update();      // 更新所有牌的状态
        void draw(QPixmap* dest) const;     // 绘制
        bool busy() const { return (roll_times!=0) ? true : moving(); }
        void select(int count);     // 选择最开始的count张牌，取消其他牌的选择和移位
        void cancel_select();       // 取消选择、清除选择，不取消移位
        void show_cards();      // 展示牌
        void hide_cards();      // 隐藏牌
        int count() const { return cards.count(); }
        const Card& front() const { return cards.front(); }
        void fold();        // 折叠所有牌
    private:
        QList<Card> cards;
        QPoint p;
        int gap;
        int roll_times;
        void spawn_card(bool show);     // 随机生成初始卡牌
        QPoint card_pos(int index) const { return p+QPoint{gap*index,0}; }
        void tolower();     // 将第一张牌放到最后
        bool moving() const;        // 返回是否有牌正在移动
    };

}

#endif // CARDS_H
