#include <QApplication>
#include "main_window.hpp"

int main(const int argc, const char* argv[]) {

    QApplication app(argc, argv);

    app.setApplicationName("SennaStudio");
    app.setOrganizationName("MIPT_DREC");

    MainWindow window;
    window.show();

    return app.exec();
}
