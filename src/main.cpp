#include <QApplication>
#include <QStyleFactory>
#include "MainWindow.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    
    app.setApplicationName("Product Calculation System");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("University Course Project");
    
    MainWindow window;
    window.show();
    
    return app.exec();
}

