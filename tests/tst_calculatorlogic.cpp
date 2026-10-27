#include "calculatorlogic.h"

#include <QtTest>

// 逻辑层单元测试：直接用脚本驱动 CalculatorLogic，检查显示的字符串
class TestCalculatorLogic : public QObject
{
    Q_OBJECT

private:
    // 脚本字符含义：0-9 数字、. 小数点、+ - * / 运算、= 等号、B 退格、C 清除、S 正负号
    static void run(CalculatorLogic &logic, const QString &script)
    {
        for (const QChar &token : script) {
            if (token.isDigit()) {
                logic.inputDigit(token.digitValue());
            } else if (token == QLatin1Char('.')) {
                logic.inputDot();
            } else if (token == QLatin1Char('+')) {
                logic.inputOperator(CalculatorLogic::Operator::Add);
            } else if (token == QLatin1Char('-')) {
                logic.inputOperator(CalculatorLogic::Operator::Subtract);
            } else if (token == QLatin1Char('*')) {
                logic.inputOperator(CalculatorLogic::Operator::Multiply);
            } else if (token == QLatin1Char('/')) {
                logic.inputOperator(CalculatorLogic::Operator::Divide);
            } else if (token == QLatin1Char('=')) {
                logic.inputEquals();
            } else if (token == QLatin1Char('B')) {
                logic.backspace();
            } else if (token == QLatin1Char('C')) {
                logic.clear();
            } else if (token == QLatin1Char('S')) {
                logic.toggleSign();
            } else if (token == QLatin1Char('%')) {
                logic.percent();
            }
        }
    }

private slots:
    // 数据驱动：脚本 -> 期望显示
    void display_data()
    {
        QTest::addColumn<QString>("script");
        QTest::addColumn<QString>("expected");

        QTest::newRow("加法")             << "2+3="     << "5";
        QTest::newRow("减法得负数")       << "5-8="     << "-3";
        QTest::newRow("乘法")             << "7*6="     << "42";
        QTest::newRow("除法")             << "9/4="     << "2.25";
        QTest::newRow("小数输入")         << "12.5+3="  << "15.5";
        QTest::newRow("浮点误差")         << "0.1+0.2=" << "0.3";
        QTest::newRow("连续小数点")       << "1.2.3"    << "1.23";
        QTest::newRow("首字符小数点")     << ".5"       << "0.5";
        QTest::newRow("连按运算符")       << "5++3="    << "8";
        QTest::newRow("连按运算符取最后") << "5+*3="    << "15";
        QTest::newRow("连续运算")         << "2+3+4="   << "9";
        QTest::newRow("连续运算中间值")   << "2+3+"     << "5";
        QTest::newRow("结果后继续运算")   << "2+3=+1="  << "6";
        QTest::newRow("结果后输入数字")   << "2+3=0.5"  << "0.5";
        QTest::newRow("重复按等号")       << "2+3=="    << "8";
        QTest::newRow("输入中退格")       << "123B"     << "12";
        QTest::newRow("退格到 0")         << "5B"       << "0";
        QTest::newRow("小数点后退格")     << "1.B"      << "1";
        QTest::newRow("清除后重算")       << "2+3=C7*8=" << "56";
        QTest::newRow("正负号")           << "9S+2="    << "-7";
        QTest::newRow("百分号")           << "50%"      << "0.5";
        QTest::newRow("除零")             << "1/0="     << "除数不能为零";
        QTest::newRow("零除零")           << "0/0="     << "除数不能为零";
    }

    void display()
    {
        QFETCH(QString, script);
        QFETCH(QString, expected);

        CalculatorLogic logic;
        run(logic, script);
        QCOMPARE(logic.displayText(), expected);
    }

    // 出错后输入数字可以自动恢复，不必先按清除
    void recoverAfterError()
    {
        CalculatorLogic logic;
        run(logic, "1/0=");
        QVERIFY(logic.hasError());

        run(logic, "5");
        QVERIFY(!logic.hasError());
        QCOMPARE(logic.displayText(), QStringLiteral("5"));
    }

    // 出错后按运算符不应把错误值带进新计算
    void ignoreOperatorWhenError()
    {
        CalculatorLogic logic;
        run(logic, "1/0=+");
        QVERIFY(logic.hasError());
        QCOMPARE(logic.displayText(), QStringLiteral("除数不能为零"));
    }

    // 结果状态下按退格可以继续修改结果
    void backspaceAfterResult()
    {
        CalculatorLogic logic;
        run(logic, "2+3=B");
        QCOMPARE(logic.displayText(), QStringLiteral("0"));
    }

    // 表达式提示行应记录完整算式
    void expressionText()
    {
        CalculatorLogic logic;
        run(logic, "12.5+3=");
        QCOMPARE(logic.expressionText(), QStringLiteral("12.5 + 3 ="));
    }
};

QTEST_APPLESS_MAIN(TestCalculatorLogic)
#include "tst_calculatorlogic.moc"
