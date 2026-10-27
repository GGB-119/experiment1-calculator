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
    static QString formatNumber(double value);        // 结果的统一格式化
    static QString operatorSymbol(Operator op);       // 运算符的显示符号
    static double compute(double lhs, double rhs, Operator op);

    QString m_entry = QStringLiteral("0");  // 当前输入的字符串
    double m_accumulator = 0.0;             // 第一个操作数
    Operator m_pendingOp = Operator::None;  // 尚未执行的运算符
    QString m_expression;                   // 表达式提示
    bool m_startNewNumber = false;          // 下一个数字是否重新开始输入
};

#endif // CALCULATORLOGIC_H
