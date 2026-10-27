#ifndef CALCULATOR_H
#define CALCULATOR_H

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class Calculator; }
QT_END_NAMESPACE

// 计算器主窗口：只负责界面展示与输入事件分发，计算规则由 CalculatorLogic 完成
class Calculator : public QWidget
{
    Q_OBJECT

public:
    explicit Calculator(QWidget *parent = nullptr);
    ~Calculator() override;

protected:
    void keyPressEvent(QKeyEvent *event) override;

private:
    Ui::Calculator *ui;
};

#endif // CALCULATOR_H
