#ifndef CALCULATORLOGIC_H
#define CALCULATORLOGIC_H

#include <QObject>
#include <QString>

// 计算器核心逻辑：只处理数字与运算规则，不依赖任何界面控件
// 界面层（Calculator）负责把鼠标点击和键盘事件转换成对这里的调用
class CalculatorLogic : public QObject
{
    Q_OBJECT

public:
    enum class Operator { None, Add, Subtract, Multiply, Divide };

    explicit CalculatorLogic(QObject *parent = nullptr);

    QString displayText() const;     // 主显示区文字
    QString expressionText() const;  // 表达式提示行文字
    bool hasError() const;

public slots:
    void inputDigit(int digit);        // 输入 0~9
    void inputDot();                   // 输入小数点
    void inputOperator(Operator op);   // 输入 + - × ÷
    void inputEquals();                // 输入 =
    void backspace();                  // 退格
    void clear();                      // 清除
    void toggleSign();                 // 正负号
    void percent();                    // 百分号

signals:
    void changed();

private:
    static QString formatNumber(double value);   // 结果的统一格式化
    static QString operatorSymbol(Operator op);  // 运算符的显示符号
    static double compute(double lhs, double rhs, Operator op);

    void resetAll();                     // 回到初始状态
    void startNewEntry(const QString &text);  // 开始输入新的操作数
    void setError(const QString &message);    // 进入错误状态
    bool applyOperation(double rhs, Operator op);  // 计算并把结果写入显示区
    double currentValue() const;

    // ---- 计算状态 ----
    QString m_entry = QStringLiteral("0");  // 当前显示/输入的数字串
    double m_accumulator = 0.0;             // 第一个操作数
    Operator m_pendingOp = Operator::None;  // 尚未执行的运算符
    QString m_expression;                   // 表达式提示行
    bool m_waitingForOperand = false;       // 刚按下运算符，等待第二个操作数
    bool m_resultShown = false;             // 当前显示的是计算结果
    bool m_error = false;                   // 是否处于错误状态
    QString m_errorText;

    // ---- 重复按等号时重复上一次运算 ----
    Operator m_lastOp = Operator::None;
    double m_lastOperand = 0.0;
    bool m_hasLastOperation = false;
};

#endif // CALCULATORLOGIC_H
