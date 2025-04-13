/**
 * 实现纯数字计算
 * 创建者：Carburn Ashroom
 * 2024.3.10
 */

#include "calc.h"

namespace Calc {

    mt19937_64 egn {static_cast<uint64_t>(time(nullptr))};      // 全局随机数引擎
    vector<mt19937_64> egns;        // 多线程引擎池

    vector<int> spawn_id(mt19937_64& engine, int count)
    {
        vector<int> result;
        while (count-- != 0) {
            for ( ; ; ) {
                bool valid {true};
                int id {rand_num(card_range, engine)};
                for (auto r : result)
                    if (r == id) {
                        valid = false;
                        break;
                    }
                if (not valid)
                    continue;
                result.push_back(id);
                break;
            }
        }
        return result;
    }
    
    vector<int> rand_order(mt19937_64& engine, int count)
    {
        vector<int> order, spawn;
        int c {count};
        while (count-- != 0)
            spawn.push_back(count);
        while (c-- != 0) {
            int od {rand_num(0, c, engine)};
            order.push_back(spawn[od]);
            spawn[od] = spawn.back();
            spawn.pop_back();
        }
        return order;
    }
    
    bool calc_once(int th_index)
    {
        mt19937_64& engine {(th_index==0) ? egn : egns[th_index]};
        vector<int> cards_id {spawn_id(engine)};
        vector<int> order {rand_order(engine)};
        vector<int> cards;
        for (auto o : order) 
            cards.push_back(cards_id[o]);
        for (int i{}; i!=cards_count; ++i) 
            cards.push_back(cards[i]);
        roll(cards, rand_num(rand_name, engine));
        insert(cards, 3, rand_num(1, cards.size()-4, engine));
        int chosen_card {cards.front()};
        cards.erase(cards.begin());
        int area {rand_num(1, 3, engine)};
        insert(cards, area, rand_num(1, cards.size()-area-1, engine));
        int sex {rand_num(1, 2, engine)};
        while (sex-- != 0)
            cards.erase(cards.begin());
        roll(cards, 7);
        while (cards.size() != 1) {
            roll(cards, 1);
            cards.erase(cards.begin());
        }
        return cards.front()==chosen_card;
    }
    
    void roll(vector<int>& cards, int times)
    {
        while (times-- != 0) {
            int temp {cards.front()};
            cards.erase(cards.begin());
            cards.push_back(temp);
        }
    }
    
    void insert(vector<int>& cards, int count, int pos)
    {
        pos += count;
        while (count-- != 0) {
            int front {cards.front()};
            cards.insert(cards.begin()+pos, front);
            cards.erase(cards.begin());
        }
    }
    
    void init_engines(int count)
    {
        egns.clear();
        try {
            random_device real;
            for (int i{}; i!=count; ++i)
                egns.push_back(mt19937_64{real()});
        }
        catch (...) {
            egns.clear();
            for (int i{}; i!=count; ++i)
                egns.push_back(mt19937_64{egn()});
        }
    }
    
    int calc_sometimes(int times, int th_index)
    {
        int success {};
        while (times-- != 0)
            if (calc_once(th_index))
                ++success;
        return success;
    }
    
    
    int calc(int times, int threads)
    {
        if (times < threads*13) 
            return calc_sometimes(times, 0);
        vector<future<int>> futures;
        while (threads != 0) {
            int this_tms {times/threads--};
            if (this_tms == 0)
                continue;
            times -= this_tms;
            future<int> th {std::async(calc_sometimes, this_tms, threads)};
            futures.push_back(std::move(th));
        } 
        int success {};
        for (auto& t : futures) 
            success += t.get();
        return success;
    }
    
}
