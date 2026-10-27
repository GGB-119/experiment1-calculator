#include "calculator.h"

#include <QtTest>
#include <QLineEdit>
#include <QPushButton>

// 界面层集成测试：同一段脚本分别用鼠标点击和键盘输入执行，结果必须一致
class TestCalculatorWidget : public QObject
{
    Q_OBJECT

private:
    static QString buttonName(QChar ch)
    {
        if (ch.isDigit())
            return QStringLiteral("btn") + ch;
        switch (ch.unicode()) {
        case '.': return QStringLiteral("btnDot");
        case '+': return QStringLiteral("btnAdd");
        case '-': return QStringLiteral("btnSubtract");
        case '*': return QStringLiteral("btnMultiply");
        case '/': return QStringLiteral("btnDivide");
        case '=': return QStringLiteral("btnEquals");
        case 'B': return QStringLiteral("btnBackspace");
        case 'C': return QStringLiteral("btnClear");
        case 'S': return QStringLiteral("btnSign");
        default:  return QString();
        }
    }

    static int keyFor(QChar ch)
    {
        if (ch.isDigit())
            return Qt::Key_0 + ch.digitValue();
        switch (ch.unicode()) {
        case '.': return Qt::Key_Period;
        case '+': return Qt::Key_Plus;
        case '-': return Qt::Key_Minus;
        case '*': return Qt::Key_Asterisk;
        case '/': return Qt::Key_Slash;
        case '=': return Qt::Key_Equal;
        case 'B': return Qt::Key_Backspace;
        case 'C': return Qt::Key_Escape;
        case 'S': return Qt::Key_F9;
        default:  return 0;
        }
    }

    static QString displayText(Calculator &window)
    {
        auto *line = window.findChild<QLineEdit *>(QStringLiteral("displayEdit"));
        return line ? line->text() : QString();
    }

    // 用鼠标点击按钮执行一段脚本
    static void runByMouse(Calculator &window, const QString &script)
    {
        for (const QChar &ch : script) {
            auto *button = window.findChild<QPushButton *>(buttonName(ch));
            QVERIFY2(button, qPrintable(QStringLiteral("找不到按钮: ") + ch));
            QTest::mouseClick(button, Qt::LeftButton);
        }
    }

    // 用键盘执行一段脚本
    static void runByKeyboard(Calculator &window, const QString &script)
    {
        for (const QChar &ch : script) {
            const int key = keyFor(ch);
            QVERIFY2(key != 0, qPrintable(QStringLiteral("没有按键映射: ") + ch));
            QTest::keyClick(&window, Qt::Key(key));
        }
    }

private slots:
    // 同一脚本：鼠标结果 == 键盘结果 == 期望值
    void mouseAndKeyboardAgree_data()
    {
        QTest::addColumn<QString>("script");
        QTest::addColumn<QString>("expected");

        QTest::newRow("小数运算") << "12.5+3="  << "15.5";
        QTest::newRow("连续运算") << "2+3+4="   << "9";
        QTest::newRow("退格")     << "123B"     << "12";
        QTest::newRow("清除")     << "2+3=C7*8=" << "56";
        QTest::newRow("除零")     << "1/0="     << "除数不能为零";
        QTest::newRow("连续小数点") << "1.2.3"  << "1.23";
    }

    void mouseAndKeyboardAgree()
    {
        QFETCH(QString, script);
        QFETCH(QString, expected);

        Calculator mouseWindow;
        mouseWindow.show();
        QVERIFY(QTest::qWaitForWindowExposed(&mouseWindow));
        runByMouse(mouseWindow, script);
        const QString mouseResult = displayText(mouseWindow);

        Calculator keyWindow;
        keyWindow.show();
        QVERIFY(QTest::qWaitForWindowExposed(&keyWindow));
        runByKeyboard(keyWindow, script);
        const QString keyResult = displayText(keyWindow);

        QCOMPARE(mouseResult, expected);
        QCOMPARE(keyResult, expected);
    }

    // 鼠标与键盘混合输入
    void mixedInput()
    {
        Calculator window;
        window.show();
        QVERIFY(QTest::qWaitForWindowExposed(&window));

        runByKeyboard(window, QStringLiteral("12.5"));  // 键盘输入第一个操作数
        QTest::mouseClick(window.findChild<QPushButton *>(QStringLiteral("btnAdd")), Qt::LeftButton);
        runByKeyboard(window, QStringLiteral("3"));
        QTest::mouseClick(window.findChild<QPushButton *>(QStringLiteral("btnEquals")), Qt::LeftButton);

        QCOMPARE(displayText(window), QStringLiteral("15.5"));
    }

    // Esc 等价于清除按钮
    void escapeClears()
    {
        Calculator window;
        window.show();
        QVERIFY(QTest::qWaitForWindowExposed(&window));

        runByMouse(window, QStringLiteral("123"));
        QTest::keyClick(&window, Qt::Key_Escape);
        QCOMPARE(displayText(window), QStringLiteral("0"));
    }

    // 回车等价于等号按钮
    void enterEquals()
    {
        Calculator window;
        window.show();
        QVERIFY(QTest::qWaitForWindowExposed(&window));

        runByMouse(window, QStringLiteral("6*7"));
        QTest::keyClick(&window, Qt::Key_Return);
        QCOMPARE(displayText(window), QStringLiteral("42"));
    }
};

QTEST_MAIN(TestCalculatorWidget)
#include "tst_calculatorwidget.moc"
