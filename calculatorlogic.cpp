#include "calculatorlogic.h"

#include <QtGlobal>
#include <QtMath>

namespace {
constexpr int kMaxDigits = 15;  // 单个操作数最多输入的字符数
}

CalculatorLogic::CalculatorLogic(QObject *parent)
    : QObject(parent)
{
}

QString CalculatorLogic::displayText() const
{
    return m_error ? m_errorText : m_entry;
}

QString CalculatorLogic::expressionText() const
{
    return m_expression;
}

bool CalculatorLogic::hasError() const
{
    return m_error;
}

void CalculatorLogic::inputDigit(int digit)
{
    if (digit < 0 || digit > 9)
        return;

    // 出错后输入数字直接重新开始，不必先按 C
    if (m_error)
        resetAll();

    if (m_waitingForOperand || m_resultShown) {
        if (m_resultShown)
            m_expression.clear();  // 上一轮的结果已经作废
        startNewEntry(QString::number(digit));
    }
    else if (m_entry == QStringLiteral("0"))
        m_entry = QString::number(digit);  // 首位 0 被新数字替换
    else if (m_entry == QStringLiteral("-0"))
        m_entry = QStringLiteral("-") + QString::number(digit);
    else if (m_entry.size() < kMaxDigits)
        m_entry.append(QChar('0' + digit));

    m_waitingForOperand = false;
    m_resultShown = false;
    emit changed();
}

void CalculatorLogic::inputDot()
{
    if (m_error)
        resetAll();

    if (m_waitingForOperand || m_resultShown) {
        if (m_resultShown)
            m_expression.clear();
        startNewEntry(QStringLiteral("0."));
        m_waitingForOperand = false;
        m_resultShown = false;
        emit changed();
        return;
    }

    // 同一个操作数只允许出现一个小数点
    if (m_entry.contains(QLatin1Char('.')) || m_entry.size() >= kMaxDigits)
        return;

    m_entry.append(QLatin1Char('.'));
    emit changed();
}

void CalculatorLogic::inputOperator(Operator op)
{
    if (m_error || op == Operator::None)
        return;

    if (m_pendingOp != Operator::None) {
        if (m_waitingForOperand) {
            // 连续按两个运算符：只替换运算符，例如 5 + × 视为 5 ×
            m_pendingOp = op;
            m_expression = formatNumber(m_accumulator) + QLatin1Char(' ') + operatorSymbol(op);
            emit changed();
            return;
        }

        // 连续运算：先算出中间结果（2 + 3 + 相当于 (2+3)+）
        if (!applyOperation(currentValue(), m_pendingOp))
            return;
    } else {
        m_accumulator = currentValue();
    }

    m_pendingOp = op;
    m_waitingForOperand = true;
    m_resultShown = false;
    m_expression = formatNumber(m_accumulator) + QLatin1Char(' ') + operatorSymbol(op);
    emit changed();
}

void CalculatorLogic::inputEquals()
{
    if (m_error)
        return;

    if (m_pendingOp == Operator::None) {
        // 没有待运算：若刚算完，重复上一次运算（2+3= 再按 = 得到 8）
        if (m_resultShown && m_hasLastOperation) {
            const double lhs = currentValue();
            const double rhs = m_lastOperand;
            if (!applyOperation(rhs, m_lastOp))
                return;
            m_expression = formatNumber(lhs) + QLatin1Char(' ') + operatorSymbol(m_lastOp)
                    + QLatin1Char(' ') + formatNumber(rhs) + QStringLiteral(" =");
            emit changed();
        }
        return;
    }

    // 只输入了第一个操作数就按等号时，用第一个操作数自身补齐
    const double lhs = m_accumulator;
    const double rhs = m_waitingForOperand ? m_accumulator : currentValue();
    const Operator op = m_pendingOp;

    if (!applyOperation(rhs, op))
        return;

    m_expression = formatNumber(lhs) + QLatin1Char(' ') + operatorSymbol(op)
            + QLatin1Char(' ') + formatNumber(rhs) + QStringLiteral(" =");
    m_pendingOp = Operator::None;
    m_waitingForOperand = false;
    m_resultShown = true;
    m_lastOp = op;
    m_lastOperand = rhs;
    m_hasLastOperation = true;
    emit changed();
}

void CalculatorLogic::backspace()
{
    if (m_error)
        return;

    // 还没开始输入第二个操作数时退格没有意义
    if (m_waitingForOperand)
        return;

    // 结果也可以继续退格修改，修改后按普通输入处理
    m_resultShown = false;

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
    resetAll();
    emit changed();
}

void CalculatorLogic::toggleSign()
{
    if (m_error || m_waitingForOperand)
        return;
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
    if (m_error || m_waitingForOperand)
        return;

    bool ok = false;
    const double value = m_entry.toDouble(&ok);
    if (!ok)
        return;

    m_entry = formatNumber(value / 100.0);
    m_resultShown = false;
    emit changed();
}

void CalculatorLogic::resetAll()
{
    m_entry = QStringLiteral("0");
    m_accumulator = 0.0;
    m_pendingOp = Operator::None;
    m_expression.clear();
    m_waitingForOperand = false;
    m_resultShown = false;
    m_error = false;
    m_errorText.clear();
    m_lastOp = Operator::None;
    m_lastOperand = 0.0;
    m_hasLastOperation = false;
}

void CalculatorLogic::startNewEntry(const QString &text)
{
    m_entry = text;
}

void CalculatorLogic::setError(const QString &message)
{
    m_error = true;
    m_errorText = message;
    m_pendingOp = Operator::None;
    m_waitingForOperand = false;
    m_resultShown = false;
    m_hasLastOperation = false;
}

bool CalculatorLogic::applyOperation(double rhs, Operator op)
{
    // 除法必须单独检查除数，避免出现 inf / nan
    if (op == Operator::Divide && qFuzzyIsNull(rhs)) {
        m_expression = formatNumber(m_accumulator) + QLatin1Char(' ')
                + operatorSymbol(op) + QLatin1Char(' ') + formatNumber(rhs) + QStringLiteral(" =");
        setError(QStringLiteral("除数不能为零"));
        emit changed();
        return false;
    }

    m_accumulator = compute(m_accumulator, rhs, op);
    m_entry = formatNumber(m_accumulator);
    return true;
}

double CalculatorLogic::currentValue() const
{
    return m_entry.toDouble();
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
    case Operator::Divide:   return rhs != 0.0 ? lhs / rhs : 0.0;
    case Operator::None:     break;
    }
    return lhs;
}
