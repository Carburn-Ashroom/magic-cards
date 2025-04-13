/**
 * 实现可移动的扑克牌
 * 创建者：Carburn Ashroom
 * 2024.3.7
 */

#include "cards.h"
#include "calc.h"

namespace Cards {

    void Card::del(int y)
    {
        to_pos = QPointF{p.x(),static_cast<double>(y)};
        choose = false;
    }

    void Card::update()
    {
        if (p == to_pos)
            choose = false;
        if (not moving())
            return;
        if (p.y()==to_pos.y())
            move_with_speed(sp);
        else
            move_with_speed(quick);
        if (fold_angle < 0)
            fold_angle = 0;
        else if (fold_angle > 0) {
            if (fold_angle > 180)
                fold_speed = -fold_speed;
            fold_angle += fold_speed;
        }
    }
    
    void Card::draw(QPixmap* dest, bool fold) const
    {
        QImage src {(display) ? imag : hide_imag};
        if (dpos == Card_display::bottom)
            src.mirror(true, true);
        QRectF src_rect {QPointF{},src.size()};
        QRectF dest_rect {p,imag.size()*ratio};
        if (half) {
            src_rect.setHeight(src_rect.height()/2);
            dest_rect.setHeight(dest_rect.height()/2);
        }
        QPainter pa {dest};
        if (fold_angle > 0) {
            draw_fold(dest_rect, src_rect, src, pa, fold);
            return;
        }
        else if (mirrored) {
            src.mirror(true, true);
            src_rect.moveTop(src_rect.top()+src_rect.height());
        }
        pa.drawImage(dest_rect, src, src_rect);
    }

    void Card::go_to(QPoint pos)
    {
        p = pos;
        to_pos = pos;
    }

    void Card::select(bool chosen)
    {
        if (not chosen)
            choose = false;
        else if (not choose) {
            choose = true;
            go_to(p.toPoint()+QPoint{0,choose_mov});
        }
    }

    void Card::cancel_select()
    {
        if (choose) {
            choose = false;
            go_to(p.toPoint()-QPoint{0,choose_mov});
        }
    }

    void Card::move_with_speed(int speed)
    {
        double x_dist {to_pos.x()-p.x()};
        double y_dist {to_pos.y()-p.y()};
        double dist {sqrt(x_dist*x_dist+y_dist*y_dist)};
        if (dist <= speed) {
            p = to_pos;
            return;
        }
        double move_x {x_dist*speed/dist};
        double move_y {y_dist*speed/dist};
        p += QPointF{move_x,move_y};
    }

    void Card::draw_fold(QRectF dest_rect, QRectF src_rect, const QImage& srce, QPainter& pa, bool only_fold) const
    {
        dest_rect.setHeight(dest_rect.height()/2);
        dest_rect.moveTop(dest_rect.top()+dest_rect.height());
        src_rect.setHeight(src_rect.height()/2);
        src_rect.moveTop(src_rect.top()+src_rect.height());
        if (not only_fold)
            pa.drawImage(dest_rect, srce, src_rect);
        QImage src {srce};
        if (radius() < 0) {
            const QImage& hide_i {(display) ? hide_imag : imag};
            src = hide_i.mirrored();
            src_rect = QRectF{QPointF{0,src.height()/2.},src.size()};
            src_rect.setHeight(src.height()/2);
            dest_rect.setHeight(dest_rect.height()*(-radius()));
        }
        else {
            src_rect.moveTop(0);
            dest_rect.setHeight(dest_rect.height()*radius());
            dest_rect.moveBottom(dest_rect.top());
        }
        pa.drawImage(dest_rect, src, src_rect);
    }
    
    void Cards_admin::split(bool half)
    {
        auto count = cards.count();
        for (int i{}; i!=count; ++i) {
            if (half)
                cards[i].cut();
            Card new_card {cards[i]};
            new_card.bottom();
            if (half)
                new_card.go_to(new_card.half_pos());
            cards.append(new_card);
        }
    }

    void Cards_admin::rand()
    {
        vector<int> new_pos;
        int same;
        do {
            same = 0;
            new_pos = Calc::rand_order();
            for (unsigned i{}; i!=new_pos.size(); ++i)
                if (new_pos[i] == static_cast<int>(i))
                    ++same;
        } while (same == cards.count());
        QList<Card> new_cards {cards};
        for (int i{}; i!=cards.count(); ++i) 
            new_cards[i] = std::move(cards[new_pos[i]]);
        cards = std::move(new_cards);
    }

    void Cards_admin::roll(int times)
    {
        if (times < 0)
            throw Negative_error{};
        roll_times = times;
    }
        
    void Cards_admin::tolower()
    {
        Card op {take_first()};
        op.select();
        cards.append(std::move(op));
    }

    bool Cards_admin::moving() const
    {
        bool moving {};
        for (const auto& c : cards)
            moving |= c.moving();
        return moving;
    }
    
    void Cards_admin::insert(int count, int index)
    {
        if (index < 1)
            throw Little_error{};
        else if (index >= cards.count()-count)
            throw Large_error{};
        QList<Card> ins_cards {del(count)};
        for (auto i=ins_cards.rbegin(); i!=ins_cards.rend(); ++i) {
            i->select(false);
            cards.insert(index, std::move(*i));
        }
    }

    void Cards_admin::insert_back(int count)
    {
        QList<Card> ins_cards {del(count)};
        for (auto i=ins_cards.rbegin(); i!=ins_cards.rend(); ++i) {
            i->select(false);
            cards.append(std::move(*i));
        }
    }
    
    QList<Card> Cards_admin::del(int count)
    {
        QList<Card> result;
        for (int i{}; i!=count; ++i) 
            result.append(take_first());
        return result;
    }
    
    void Cards_admin::update()
    {
        for (int i{}; i!=cards.count(); ++i) {
            cards[i].move_to(card_pos(i));
            cards[i].update();
        }
        if (roll_times!=0 and not moving()) {
            --roll_times;
            tolower();
        }
    }
    
    void Cards_admin::draw(QPixmap* dest) const
    {
        for (const auto& c : cards)
            c.draw(dest);
        if (cards.front().folding())
            for (auto i=cards.rbegin(); i!=cards.rend(); ++i)
                i->draw(dest, true);
    }

    void Cards_admin::select(int count)
    {
        for (int i{}; i!=cards.count(); ++i) {
            cards[i].cancel_select();
            if (i < count)
                cards[i].select();
        }
    }

    void Cards_admin::cancel_select()
    {
        for (auto& c : cards)
            c.select(false);
    }

    void Cards_admin::show_cards()
    {
        for (auto& c : cards)
            c.show();
    }

    void Cards_admin::hide_cards()
    {
        for (auto& c : cards)
            c.hide();
    }

    void Cards_admin::fold()
    {
        for (auto& c : cards)
            c.fold();
    }

    void Cards_admin::spawn_card(bool show)
    {
        vector<int> id {Calc::spawn_id()};
        for (unsigned i{}; i!=id.size(); ++i)
            cards.append(Card{card_pos(i),id[i],show});
    }

}
