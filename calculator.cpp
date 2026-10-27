#include "calculator.h"
#include "ui_calculator.h"

#include "calculatorlogic.h"

#include <QKeyEvent>
#include <QPushButton>
#include <QTimer>

Calculator::Calculator(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Calculator)
    , m_logic(new CalculatorLogic(this))
{
    ui->setupUi(this);
    setupLayoutStretch();
    setupButtons();

    connect(m_logic, &CalculatorLogic::changed, this, &Calculator::updateDisplay);

    // 窗口本身接收键盘事件，按钮不抢占焦点
    setFocusPolicy(Qt::StrongFocus);
    setFocus();
    updateDisplay();
}

Calculator::~Calculator()
{
    delete ui;
}

void Calculator::setupLayoutStretch()
{
    // 让按键区域的行列均分窗口剩余空间
    for (int row = 0; row < ui->buttonGrid->rowCount(); ++row)
        ui->buttonGrid->setRowStretch(row, 1);
    for (int col = 0; col < ui->buttonGrid->columnCount(); ++col)
        ui->buttonGrid->setColumnStretch(col, 1);
}

void Calculator::setupButtons()
{
    struct Binding {
        QPushButton *button;
        Action action;
    };

    const Binding bindings[] = {
        {ui->btn0, Action::Digit0}, {ui->btn1, Action::Digit1},
        {ui->btn2, Action::Digit2}, {ui->btn3, Action::Digit3},
        {ui->btn4, Action::Digit4}, {ui->btn5, Action::Digit5},
        {ui->btn6, Action::Digit6}, {ui->btn7, Action::Digit7},
        {ui->btn8, Action::Digit8}, {ui->btn9, Action::Digit9},
        {ui->btnDot, Action::Dot},             {ui->btnAdd, Action::Add},
        {ui->btnSubtract, Action::Subtract},   {ui->btnMultiply, Action::Multiply},
        {ui->btnDivide, Action::Divide},       {ui->btnEquals, Action::Equals},
        {ui->btnBackspace, Action::Backspace}, {ui->btnClear, Action::Clear},
        {ui->btnSign, Action::Sign},           {ui->btnPercent, Action::Percent},
    };

    for (const Binding &binding : bindings) {
        connect(binding.button, &QPushButton::clicked,
                this, [this, action = binding.action] { applyAction(action); });
        m_buttons.insert(binding.action, binding.button);
    }
}

void Calculator::applyAction(Action action)
{
    // 数字键共用一个处理分支：Action::Digit0 ~ Action::Digit9 是连续编号
    const int id = static_cast<int>(action);
    if (id >= static_cast<int>(Action::Digit0) && id <= static_cast<int>(Action::Digit9)) {
        m_logic->inputDigit(id - static_cast<int>(Action::Digit0));
        return;
    }

    switch (action) {
    case Action::Dot:       m_logic->inputDot();    break;
    case Action::Backspace: m_logic->backspace();   break;
    case Action::Clear:     m_logic->clear();       break;
    case Action::Sign:      m_logic->toggleSign();  break;
    case Action::Percent:   m_logic->percent();     break;
    case Action::Add:
        m_logic->inputOperator(CalculatorLogic::Operator::Add);
        break;
    case Action::Subtract:
        m_logic->inputOperator(CalculatorLogic::Operator::Subtract);
        break;
    case Action::Multiply:
        m_logic->inputOperator(CalculatorLogic::Operator::Multiply);
        break;
    case Action::Divide:
        m_logic->inputOperator(CalculatorLogic::Operator::Divide);
        break;
    case Action::Equals:    m_logic->inputEquals(); break;
    default:                                        break;
    }
}

void Calculator::updateDisplay()
{
    ui->displayEdit->setText(m_logic->displayText());
    ui->expressionLabel->setText(m_logic->expressionText());
}

void Calculator::keyPressEvent(QKeyEvent *event)
{
    // 键盘事件先翻译成与鼠标按钮相同的 Action，再走同一个 applyAction()
    Action action = Action::Clear;
    if (actionForKey(event->key(), event->modifiers(), &action)) {
        flashButton(action);
        applyAction(action);
        event->accept();
        return;
    }

    QWidget::keyPressEvent(event);
}

bool Calculator::actionForKey(int key, Qt::KeyboardModifiers modifiers, Action *action)
{
    Q_UNUSED(modifiers)

    // 主键盘和小键盘的数字键都映射到 Digit0 ~ Digit9
    if (key >= Qt::Key_0 && key <= Qt::Key_9) {
        *action = static_cast<Action>(static_cast<int>(Action::Digit0) + (key - Qt::Key_0));
        return true;
    }

    switch (key) {
    case Qt::Key_Period:
    case Qt::Key_Comma:      *action = Action::Dot;       return true;
    case Qt::Key_Plus:       *action = Action::Add;       return true;
    case Qt::Key_Minus:      *action = Action::Subtract;  return true;
    case Qt::Key_Asterisk:   *action = Action::Multiply;  return true;
    case Qt::Key_Slash:      *action = Action::Divide;    return true;
    case Qt::Key_Return:
    case Qt::Key_Enter:
    case Qt::Key_Equal:      *action = Action::Equals;    return true;
    case Qt::Key_Backspace:  *action = Action::Backspace; return true;
    case Qt::Key_Escape:
    case Qt::Key_Delete:     *action = Action::Clear;     return true;
    case Qt::Key_Percent:    *action = Action::Percent;   return true;
    case Qt::Key_F9:         *action = Action::Sign;      return true;
    default:                 break;
    }
    return false;
}

void Calculator::flashButton(Action action)
{
    QPushButton *button = m_buttons.value(action, nullptr);
    if (!button)
        return;

    button->setDown(true);
    QTimer::singleShot(80, button, [button] { button->setDown(false); });
}
