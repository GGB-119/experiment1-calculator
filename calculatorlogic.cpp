#include "calculatorlogic.h"

#include <QtGlobal>
#include <QtMath>

namespace {
constexpr int kMaxDigits = 15;  // 单个操作数最多输入的位数
}

CalculatorLogic::CalculatorLogic(QObject *parent)
    : QObject(parent)
{
}

QString CalculatorLogic::displayText() const
{
    return m_entry;
}

QString CalculatorLogic::expressionText() const
{
    return m_expression;
}

void CalculatorLogic::inputDigit(int digit)
{
    if (digit < 0 || digit > 9)
        return;

    if (m_startNewNumber) {
        m_entry = QString::number(digit);
        m_startNewNumber = false;
    } else if (m_entry == QStringLiteral("0")) {
        m_entry = QString::number(digit);  // 首位 0 被新数字替换
    } else if (m_entry == QStringLiteral("-0")) {
        m_entry = QStringLiteral("-") + QString::number(digit);
    } else if (m_entry.size() < kMaxDigits) {
        m_entry.append(QChar('0' + digit));
    }
    emit changed();
}

void CalculatorLogic::inputDot()
{
    if (m_startNewNumber) {
        m_entry = QStringLiteral("0.");
        m_startNewNumber = false;
        emit changed();
        return;
    }

    // 同一个操作数只允许出现一个小数点
    if (m_entry.contains(QLatin1Char('.')))
        return;

    m_entry.append(QLatin1Char('.'));
    emit changed();
}

void CalculatorLogic::inputOperator(Operator op)
{
    m_accumulator = m_entry.toDouble();
    m_pendingOp = op;
    m_entry = QStringLiteral("0");
    m_startNewNumber = true;
    m_expression = formatNumber(m_accumulator) + QLatin1Char(' ') + operatorSymbol(op);
    emit changed();
}

void CalculatorLogic::inputEquals()
{
    if (m_pendingOp == Operator::None)
        return;

    const double lhs = m_accumulator;
    const double rhs = m_startNewNumber ? m_accumulator : m_entry.toDouble();
    const double result = compute(lhs, rhs, m_pendingOp);

    m_expression = formatNumber(lhs) + QLatin1Char(' ') + operatorSymbol(m_pendingOp)
            + QLatin1Char(' ') + formatNumber(rhs) + QStringLiteral(" =");
    m_entry = formatNumber(result);
    m_pendingOp = Operator::None;
    m_startNewNumber = true;
    emit changed();
}

void CalculatorLogic::backspace()
{
    if (m_startNewNumber)
        return;  // 结果或新操作数状态下退格不做处理

    // 删除最后一个字符；删到只剩一个字符时恢复为 0
    const bool onlyOneCharLeft = m_entry.size() == 1
            || (m_entry.size() == 2 && m_entry.startsWith(QLatin1Char('-')));
    if (onlyOneCharLeft)
        m_entry = QStringLiteral("0");
    else
        m_entry.chop(1);

    emit changed();
}

void CalculatorLogic::clear()
{
    m_entry = QStringLiteral("0");
    m_accumulator = 0.0;
    m_pendingOp = Operator::None;
    m_expression.clear();
    m_startNewNumber = false;
    emit changed();
}

void CalculatorLogic::toggleSign()
{
    if (m_entry == QStringLiteral("0"))
        return;

    if (m_entry.startsWith(QLatin1Char('-')))
        m_entry.remove(0, 1);
    else
        m_entry.prepend(QLatin1Char('-'));

    emit changed();
}

void CalculatorLogic::percent()
{
    bool ok = false;
    const double value = m_entry.toDouble(&ok);
    if (!ok)
        return;

    m_entry = formatNumber(value / 100.0);
    emit changed();
}

QString CalculatorLogic::formatNumber(double value)
{
    if (!qIsFinite(value))
        return QStringLiteral("错误");
    if (qFuzzyIsNull(value))
        return QStringLiteral("0");

    // 12 位有效数字，既能避开 0.1+0.2 之类的浮点误差，又不会显示过长
    return QString::number(value, 'g', 12);
}

QString CalculatorLogic::operatorSymbol(Operator op)
{
    switch (op) {
    case Operator::Add:      return QStringLiteral("+");
    case Operator::Subtract: return QStringLiteral("−");
    case Operator::Multiply: return QStringLiteral("×");
    case Operator::Divide:   return QStringLiteral("÷");
    case Operator::None:     break;
    }
    return QString();
}

double CalculatorLogic::compute(double lhs, double rhs, Operator op)
{
    switch (op) {
    case Operator::Add:      return lhs + rhs;
    case Operator::Subtract: return lhs - rhs;
    case Operator::Multiply: return lhs * rhs;
    case Operator::Divide:   return lhs / rhs;
    case Operator::None:     break;
    }
    return lhs;
}
