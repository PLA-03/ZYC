#include "mainwindow.h"
#include "databasemanager.h"

#include <QApplication>
#include <QMessageBox>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    
    // Initialize database connection
    if (!DatabaseManager::instance().openDatabase()) {
        QMessageBox::critical(nullptr, QObject::tr("Database Error"), 
                              QObject::tr("Failed to connect to database!\nThe application will exit."));
        return -1;
    }
    
    MainWindow w;
    w.show();
    
    int result = a.exec();
    
    // Cleanup database connection
    DatabaseManager::instance().disconnect();
    
    return result;
}