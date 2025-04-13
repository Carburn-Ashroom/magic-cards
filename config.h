#ifndef CONFIG_H
#define CONFIG_H

#include <QString>
#include <random>
#include <complex>
#include <QPoint>
#include <QColor>
#include <thread>

using std::uniform_int_distribution;
using std::complex;
using std::thread;

enum class Step { rand, cut, name, insert, destined, area, sex, sanae, luck, wonder };      // 步骤名
enum class Card_display { top, bottom };        // 卡片显示部位

// 扑克牌配置
const QString card_file {"Cards/veryhuo.com_pkp_%1.jpg"};       // 扑克文件名
constexpr complex<int> card_range {1,54};       // 扑克牌id范围
constexpr int card_speed {3};       // 卡牌速度
constexpr int card_sland_speed {6};     // 卡牌长途速度
constexpr int card_delete {800};        // 卡牌删除位置
constexpr int card_width {80};      // 卡牌宽度
constexpr int card_select {36};     // 卡牌被选中下降高度
constexpr bool card_cut {true};     // 卡牌是否会被“撕开”
constexpr QPoint card_destined {577,450};       // 真命天牌位置
constexpr bool card_show {true};        // 默认明牌
const QString card_hide_imag {"Cards/Hide.jpg"};        // 卡牌背面图片
constexpr int card_fold_speed {2};      // 折叠角速度

// 牌堆配置
constexpr QPoint cards_pos {330,80};        // 牌堆位置
constexpr int cards_count {4};      // 卡牌初始数量
constexpr int cards_gap {40};       // 卡牌间隔

// 窗口配置
const QColor wnd_back {"#F0F0F0"};      // 窗口背景颜色
constexpr int wnd_fps {100};        // 窗口帧率

// 步骤配置
constexpr int step_count {10};      // 步骤数

// 线程配置
const int th_count {static_cast<int>(thread::hardware_concurrency())};      // 线程数

// 随机配置
constexpr complex<int> rand_name {1,13};        // 随机名字长度

// 其他配置
const QString oth_grammar {"见证奇迹的时刻"};      // 咒语
constexpr double oth_pi {3.14159265358979323846264338327950288419719939937511};     // 看来花括号不一定能防窄化

#endif // CONFIG_H
