#include "LoginWindow.h"
#include <QApplication>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    LoginWindow w;
    if (w.exec() == QDialog::Accepted) {
        MainWindow mainWindow(w.getCurrentUserId());
        mainWindow.show();
        return a.exec();
    }
    return 0;
}
