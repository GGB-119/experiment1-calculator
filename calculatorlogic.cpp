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
    return QString();
}

void CalculatorLogic::inputDigit(int digit)
{
    if (digit < 0 || digit > 9)
        return;

    if (m_entry == QStringLiteral("0")) {
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
    // 同一个操作数只允许出现一个小数点
    if (m_entry.contains(QLatin1Char('.')))
        return;

    m_entry.append(QLatin1Char('.'));
    emit changed();
}

void CalculatorLogic::backspace()
{
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
