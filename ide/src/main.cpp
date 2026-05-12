#include <QApplication>
#include "main_window.hpp"

int main(int argc, char* argv[]) {

    QApplication app(argc, argv);

    app.setApplicationName("SennaStudio");
    app.setOrganizationName("MIPT_DREC");

    MainWindow window;
    window.show();

    return app.exec();
}
