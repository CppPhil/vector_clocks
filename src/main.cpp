#include "main_window.hpp"

#include <QApplication>

int main(int argc, char* argv[])
{
    QApplication application(argc, argv);
    vc::main_window main_window;

    main_window.show();

    return application.exec();
}
