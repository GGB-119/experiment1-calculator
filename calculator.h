#ifndef CALCULATOR_H
#define CALCULATOR_H

#include <QHash>
#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class Calculator; }
class QPushButton;
QT_END_NAMESPACE

class CalculatorLogic;

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
    // 所有按键动作的统一编号，鼠标按钮和键盘按键都转换成这里的取值
    enum class Action {
        Digit0, Digit1, Digit2, Digit3, Digit4,
        Digit5, Digit6, Digit7, Digit8, Digit9,
        Dot, Add, Subtract, Multiply, Divide,
        Equals, Backspace, Clear, Sign, Percent
    };

    void setupButtons();               // 绑定按钮与动作
    void loadStyleSheet();
    void setupLayoutStretch();
    void applyAction(Action action);   // 唯一的动作处理入口
    void updateDisplay();
    void flashButton(Action action);   // 键盘输入时给对应按钮按下反馈
    static bool actionForKey(int key, Qt::KeyboardModifiers modifiers, Action *action);

    Ui::Calculator *ui;
    CalculatorLogic *m_logic;
    QHash<Action, QPushButton *> m_buttons;  // 动作 -> 对应按钮，供键盘输入时高亮
};

#endif // CALCULATOR_H
