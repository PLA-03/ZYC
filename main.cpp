#include "mainwindow.h"
#include "loginwindow.h"
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
    
    // Show login window first
    LoginWindow loginWindow;
    if (loginWindow.exec() != QDialog::Accepted || !loginWindow.isLoggedIn()) {
        // Login failed or canceled
        DatabaseManager::instance().disconnect();
        return 0;
    }
    
    // Login successful, show main window
    MainWindow w;
    w.show();
    
    int result = a.exec();
    
    // Cleanup database connection
    DatabaseManager::instance().disconnect();
    
    return result;
}