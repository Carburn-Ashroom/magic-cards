#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPainter>
#include <QPaintEvent>
#include <QPixmap>
#include <QTimer>
#include <QMessageBox>
#include <optional>
#include <QRadioButton>

#include "cards.h"

using std::optional;
using namespace std::chrono;

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void paintEvent(QPaintEvent* event);

private slots:
    void frame_update();        // 更新每一帧

    void on_mess_clicked();

    void on_next_step_clicked();

    void on_split_clicked();

    void on_name_ok_clicked();

    void on_step_currentRowChanged(int current_row);

    void on_insert_ok_clicked();

    void on_choose_clicked();

    void on_south_clicked();

    void on_north_clicked();

    void on_area_ok_clicked();

    void on_show_stateChanged(int arg1);

    void on_no_area_clicked();

    void on_male_clicked();

    void on_female_clicked();

    void on_sex_clicked();

    void on_sanae_clicked();

    void on_throw_or_catch_clicked();

    void on_judge_clicked();

    void on_bench_clicked();

    void on_rand_clicked();

    void on_fold_clicked();

    void on_sex_count_valueChanged(int arg1);

private:
    Ui::MainWindow *ui;
    Cards::Cards_admin magician;
    optional<Cards::Card> destined_card;
    QList<Cards::Card> del_cards;
    QPixmap snapshot;
    void update_snapshot();     // 绘制这一帧
    QTimer timer;
    QList<QWidget*> enable_widgets;
    QList<QRadioButton*> chooses;
    void set_enable_widgets();      // 设置在牌运动时应当禁用的组件
    bool enabled {true};
    int step {-1};
    void shift_step(int index);     // 转换并初始化这一步
    QList<Cards::Cards_admin> step_magicians {QList<Cards::Cards_admin>(step_count)};
    void clear_radio_checked(QRadioButton* except =nullptr);        // 取消除except外所有单选按钮的选中
    void call(QAbstractButton* button);     // 安全按下button按钮
    void show_or_hide(bool show);       // 明牌或隐牌
    int sanae;
    bool check_anim {};
    void judge_success();       // 判断是否成功
    optional<complex<QString>> judge_message;
    bool cut {};
};

#endif // MAINWINDOW_H
