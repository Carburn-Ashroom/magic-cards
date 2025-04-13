#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "calc.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setFixedSize(size());
    connect(&timer, SIGNAL(timeout()), this, SLOT(frame_update()));
    set_enable_widgets();
    ui->step->setCurrentRow(0);
    ui->show->setChecked(card_show);
    timer.start(1000/wnd_fps);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::paintEvent(QPaintEvent* event)
{
    event->accept();
    QPainter pa {this};
    pa.drawPixmap(0, 0, snapshot);
}

void MainWindow::frame_update()
{
    update_snapshot();
    magician.update();
    if (magician.busy() and ui->step->isEnabled()) {
        for (auto x : enable_widgets)
            x->setEnabled(false);
        for (auto x : chooses)
            x->setEnabled(false);
    }
    else if (not magician.busy() and not ui->step->isEnabled()) {
        for (auto x : enable_widgets)
            x->setEnabled(true);
        for (auto x : chooses)
            x->setChecked(false);
        if (step == static_cast<int>(Step::luck)) {
            if (magician.count() == 1)
                ui->throw_or_catch->setEnabled(false);
            else
                ui->next_step->setEnabled(false);
        }
        else if (step == static_cast<int>(Step::sanae)) {
            ui->next_step->setEnabled(sanae==oth_grammar.length());
            ui->sanae->setEnabled(sanae!=oth_grammar.length());
        }
        if (step == static_cast<int>(Step::cut)) {
            ui->split->setEnabled(not cut);
            ui->next_step->setEnabled(cut);
            ui->fold->setEnabled(not cut);
        }
        ui->statusbar->clearMessage();
    }
    if (destined_card.has_value()) {
        destined_card->update();
        if (check_anim and not destined_card->moving()) {
            check_anim = false;
            judge_success();
            ui->judge->setEnabled(true);
        }
    }
    for (auto& c : del_cards)
        c.update();
}

void MainWindow::update_snapshot()
{
    QPixmap snap {size()};
    QPainter pa {&snap};
    pa.fillRect(QRect{QPoint{},size()}, wnd_back);
    pa.end();
    magician.draw(&snap);
    if (destined_card.has_value())
        destined_card->draw(&snap);
    for (const auto& c : del_cards)
        c.draw(&snap);
    snapshot = std::move(snap);
    update();
    if (judge_message.has_value()) {
        QMessageBox::information(this, judge_message->real(), judge_message->imag());
        judge_message.reset();
    }
}

void MainWindow::set_enable_widgets()
{
    enable_widgets << ui->step << ui->next_step;
    enable_widgets << ui->mess;
    enable_widgets << ui->fold << ui->split;
    enable_widgets << ui->name_ok;
    chooses << ui->south << ui->north << ui->no_area;
    enable_widgets << ui->area_ok << ui->south << ui->north << ui->no_area;
    chooses << ui->male << ui->female;
    enable_widgets << ui->sanae;
    enable_widgets << ui->throw_or_catch;
    for (auto x : chooses)
        x->setAutoExclusive(false);
}

void MainWindow::shift_step(int index)
{
    if (index <= static_cast<int>(Step::destined))
        destined_card.reset();
    if (index < step)
        magician = step_magicians[index];
    else
        step_magicians[index] = magician;
    if (step!=static_cast<int>(Step::destined) and destined_card.has_value())
        destined_card->go_to(card_destined);
    check_anim = false;
    step = index;
    ui->pages->setCurrentIndex(step);
    show_or_hide(ui->show->isChecked());
    switch (static_cast<Step>(step)) {
    case Step::cut:
        ui->split->setEnabled(true);
        ui->fold->setEnabled(true);
        ui->next_step->setEnabled(false);
        cut = false;
        break;
    case Step::insert:
        magician.select(3);
        ui->insert_ok->setEnabled(true);
        ui->next_step->setEnabled(false);
        break;
    case Step::destined:
        magician.select(1);
        ui->choose->setEnabled(true);
        ui->next_step->setEnabled(false);
        break;
    case Step::sex:
        ui->male->setEnabled(true);
        ui->female->setEnabled(true);
        ui->sex->setEnabled(true);
        ui->next_step->setEnabled(false);
        ui->sex_count->setMaximum(magician.count()-2);
        ui->sex_count->setEnabled(true);
        ui->sex_count->setValue(0);
        magician.select(0);
        break;
    case Step::sanae:
        ui->next_step->setEnabled(false);
        ui->sanae->setEnabled(true);
        ui->sanae->setText(oth_grammar.front());
        sanae = 0;
        break;
    case Step::luck:
        ui->next_step->setEnabled(false);
        ui->throw_or_catch->setEnabled(true);
        ui->throw_or_catch->setText(tr("好运留下来"));
        break;
    case Step::wonder:
        ui->next_step->setEnabled(false);
        ui->judge->setEnabled(true);
        break;
    default:
        clear_radio_checked();
        ui->next_step->setEnabled(true);
        break;
    }
}

void MainWindow::clear_radio_checked(QRadioButton* except)
{
    for (auto x : chooses)
        if (x != except)
            x->setChecked(false);
    if (except)
        except->setChecked(true);
}

void MainWindow::call(QAbstractButton* button)
{
    if (button->isEnabled())
        emit button->clicked();
    else
        ui->statusbar->showMessage("不要尝试使用当前灰化的功能");
}

void MainWindow::show_or_hide(bool show)
{
    if (show)
        magician.show_cards();
    else
        magician.hide_cards();
    if (destined_card.has_value()) {
        if (show)
            destined_card->show();
        else
            destined_card->hide();
    }
}

void MainWindow::on_mess_clicked()
{
    magician.rand();
}

void MainWindow::on_next_step_clicked()
{
    ui->step->setCurrentRow(step+1);
}

void MainWindow::on_split_clicked()
{
    magician.split();
    cut = true;
}

void MainWindow::on_name_ok_clicked()
try
{
    magician.roll(ui->name_length->text().toInt());
}
catch (Cards::Negative_error) {
    QMessageBox::critical(this, tr("错误"), tr("你名字负数个数？"));
}

void MainWindow::on_step_currentRowChanged(int current_row)
{
    if (current_row == -1)
        ui->step->setCurrentRow(step);
    else if (current_row>step+1 or (current_row==step+1 and not ui->next_step->isEnabled())) {
        ui->step->setCurrentRow(step);
        QMessageBox::critical(this, tr("禁止跳步"), tr("您跳得有亿点远"));
        ui->step->setCurrentRow(step);
    }
    else if (current_row != step)
        shift_step(current_row);
}

void MainWindow::on_insert_ok_clicked()
try
{
    magician.insert(3, ui->insert_pos->text().toInt());
    ui->insert_ok->setEnabled(false);
}
catch (Cards::Little_error) {
    QMessageBox::critical(this, tr("数字太小"), tr("不能放在最前面"));
}
catch (Cards::Large_error) {
    QMessageBox::critical(this, tr("数字太大"), tr("不能放在最后面，魔术会失败的"));
}

void MainWindow::on_choose_clicked()
{
    destined_card = magician.take_first();
    destined_card->move_to(card_destined);
    destined_card->mirror(true);
    ui->choose->setEnabled(false);
    ui->next_step->setEnabled(true);
}

void MainWindow::on_south_clicked()
{
    clear_radio_checked(ui->south);
    magician.select(1);
}

void MainWindow::on_north_clicked()
{
    clear_radio_checked(ui->north);
    magician.select(2);
}

void MainWindow::on_area_ok_clicked()
try
{
    int locat {ui->insert_area->text().toInt()};
    if (ui->north->isChecked())
        magician.insert(2, locat);
    else if (ui->south->isChecked())
        magician.insert(1, locat);
    else if (ui->no_area->isChecked())
        magician.insert(3, locat);
}
catch (Cards::Little_error) {
    // 什么也不做
}
catch (Cards::Large_error) {
    if (QMessageBox::question(this, tr("失败的复刻"), tr("插到最后会失败的\n小尼失败的原因就是：在这一步将一张牌插入到最后，剩下的牌插入到中间\n是否复刻小尼失败的原因？")) == QMessageBox::Yes) {
        int card_count {};
        if (ui->north->isChecked())
            card_count = 1;
        else if (ui->no_area->isChecked())
            card_count = 2;
        if (card_count != 0) {
            int locat {Calc::rand_num(1, magician.count()-1-card_count)};
            magician.insert(card_count, locat);
        }
        magician.insert_back(1);
    }
}

void MainWindow::on_show_stateChanged(int arg1)
{
    show_or_hide(arg1==Qt::Checked);
}

void MainWindow::on_no_area_clicked()
{
    clear_radio_checked(ui->no_area);
    magician.select(3);
}

void MainWindow::on_male_clicked()
{
    clear_radio_checked(ui->male);
    magician.select(1);
    ui->sex_count->setValue(1);
}

void MainWindow::on_female_clicked()
{
    clear_radio_checked(ui->female);
    magician.select(2);
    ui->sex_count->setValue(2);
}

void MainWindow::on_sex_clicked()
{
    int count {ui->sex_count->value()};
    if (count == 0)
        ui->next_step->setEnabled(true);
    else {
        del_cards = magician.del(count);
        for (auto& d : del_cards)
            d.del();
    }
    ui->sex->setEnabled(false);
    ui->male->setEnabled(false);
    ui->female->setEnabled(false);
    ui->sex_count->setEnabled(false);
}

void MainWindow::on_sanae_clicked()
{
    ++sanae;
    if(ui->sanae->text() != oth_grammar.back())
        ui->sanae->setText(oth_grammar[sanae]);
    magician.roll(1);
}

void MainWindow::on_throw_or_catch_clicked()
{
    QString cmd {ui->throw_or_catch->text()};
    if (cmd == tr("好运留下来")) {
        ui->throw_or_catch->setText(tr("烦恼丢出去"));
        magician.roll(1);
        return;
    }
    Cards::Card taken {magician.take_first()};
    taken.del();
    del_cards.append(std::move(taken));
    ui->throw_or_catch->setText(tr((magician.count()==1) ? "点下一步查看结果" : "好运留下来"));
}

void MainWindow::judge_success()
{
    bool success {true};
    if (magician.count() != 1)
        success = false;
    if (magician.see() != *destined_card)
        success = false;
    judge_message = (success) ? complex<QString>{"恭喜","你成功了"} : complex<QString>{"抱歉","你失败了"};
}

void MainWindow::on_bench_clicked()
{
    auto start = high_resolution_clock::now();
    int obj {ui->calc_times->value()};
    int real {Calc::calc(obj)};
    auto end = high_resolution_clock::now();
    double time {duration_cast<nanoseconds>(end-start).count()/1e9};
    QString message {"%1次魔术成功%2次，成功率：%3%，用时%4秒"};
    message = message.arg(obj).arg(real).arg(real*100ULL/obj).arg(time);
    ui->statusbar->showMessage(message);
    QMessageBox::information(this, "计算完毕", message);
}

void MainWindow::on_rand_clicked()
{
    int buffer;
    switch (static_cast<Step>(step)) {
    case Step::rand:
        call(ui->mess);
        break;
    case Step::cut:
        call(ui->split);
        break;
    case Step::name:
        buffer = Calc::rand_num(rand_name);
        ui->name_length->setText(tr("").setNum(buffer));
        call(ui->name_ok);
        break;
    case Step::insert:
        buffer = Calc::rand_num(1, magician.count()-4);
        ui->insert_pos->setText(tr("").setNum(buffer));
        call(ui->insert_ok);
        break;
    case Step::destined:
        call(ui->choose);
        break;
    case Step::area:
        buffer = Calc::rand_num(1, 3);
        switch (buffer) {
        case 1:
            call(ui->south);
            break;
        case 2:
            call(ui->north);
            break;
        default:
            call(ui->no_area);
            break;
        }
        buffer = Calc::rand_num(1, magician.count()-buffer-1);
        ui->insert_area->setText(tr("").setNum(buffer));
        call(ui->area_ok);
        break;
    case Step::sex:
        buffer = Calc::rand_num(0, magician.count()-2);
        ui->sex_count->setValue(buffer);
        emit ui->sex_count->valueChanged(buffer);
        call(ui->sex);
        break;
    case Step::sanae:
        call(ui->sanae);
        break;
    case Step::luck:
        call(ui->throw_or_catch);
        break;
    case Step::wonder:
        call(ui->judge);
        break;
    }
}

void MainWindow::on_judge_clicked()
{
    ui->show->setCheckState(Qt::Checked);
    ui->judge->setEnabled(false);
    check_anim = true;
    destined_card->move_to(magician.front().half_pos());
}

void MainWindow::on_fold_clicked()
{
    magician.fold();
}

void MainWindow::on_sex_count_valueChanged(int arg1)
{
    switch (arg1) {
    case 1:
        call(ui->male);
        break;
    case 2:
        call(ui->female);
        break;
    default:
        magician.select(arg1);
        clear_radio_checked();
        break;
    }
}
