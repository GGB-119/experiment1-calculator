#include "calculator.h"

#include <QApplication>
#include <QFont>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // 统一使用系统自带的中文字体，避免不同机器上字体缺失
    QFont appFont(QStringLiteral("Microsoft YaHei UI"));
    appFont.setPointSize(11);
    app.setFont(appFont);

    Calculator window;
    window.show();

    return app.exec();
}
