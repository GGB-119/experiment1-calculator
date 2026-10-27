#include "calculator.h"
#include "ui_calculator.h"

#include <QKeyEvent>

Calculator::Calculator(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Calculator)
{
    ui->setupUi(this);

    // 让按键区域的行列均分窗口剩余空间
    for (int row = 0; row < ui->buttonGrid->rowCount(); ++row)
        ui->buttonGrid->setRowStretch(row, 1);
    for (int col = 0; col < ui->buttonGrid->columnCount(); ++col)
        ui->buttonGrid->setColumnStretch(col, 1);

    // 窗口本身接收键盘事件，按钮不抢占焦点
    setFocusPolicy(Qt::StrongFocus);
    setFocus();
}

Calculator::~Calculator()
{
    delete ui;
}

void Calculator::keyPressEvent(QKeyEvent *event)
{
    QWidget::keyPressEvent(event);
}
