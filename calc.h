#ifndef CALC_H
#define CALC_H

#include <vector>
#include <random>
#include <future>

#include "config.h"

namespace Calc {        // 该名字空间负责纯数字模拟运算

    using std::vector;
    using std::mt19937_64;
    using std::future;
    using std::random_device;
    
    extern mt19937_64 egn;      // 界面用的全局随机数引擎
    extern vector<mt19937_64> egns;     // 多线程随机引擎池

    void init_engines(int count =th_count);     // 初始化多线程引擎池，置入count个线程。count必须是正数
    vector<int> rand_order(mt19937_64& engine =egn, int count =cards_count);        // 用egn引擎生成[0,count)之间count个不重复随机数。所有参数必须合理有效
    vector<int> spawn_id(mt19937_64& engine =egn, int count =cards_count);      // 用egn和全局的rand生成count个不重复的卡牌id
    bool calc_once(int th_index);       // 使用第th_index个随机引擎变一次魔术
    inline int rand_num(int min, int max, mt19937_64& engine =egn) { return uniform_int_distribution<>{min,max}(engine); }
    inline int rand_num(const complex<int>& range, mt19937_64& engine =egn) { return rand_num(range.real(), range.imag(), engine); }
    void roll(vector<int>& cards, int times);       // 将第一张牌放到最后
    void insert(vector<int>& cards, int count, int pos);        // 将前count张卡牌拿出来，然后插入pos位置。count+pos要小于cards容量
    int calc_sometimes(int times, int th_index);        // 使用第th_index个随机引擎在单线程内变times次魔术
    int calc(int times, int threads =th_count);     // 使用threads个随机引擎在threads个线程中变共计times次魔术
    
}

#endif // CALC_H
