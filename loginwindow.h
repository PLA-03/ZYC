#ifndef LOGINWINDOW_H
#define LOGINWINDOW_H

#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QMessageBox>
#include <QIcon>

namespace Ui {
class LoginWindow;
}

class LoginWindow : public QDialog
{
    Q_OBJECT

public:
    explicit LoginWindow(QWidget *parent = nullptr);
    ~LoginWindow();

    bool isLoggedIn() const;
    QString getCurrentUsername() const;

public slots:
    void on_loginButton_clicked();
    void on_cancelButton_clicked();
    void on_registerButton_clicked();
    void on_usernameEdit_textChanged(const QString &text);
    void on_passwordEdit_textChanged(const QString &text);
    void onRegistrationSuccessful(const QString &username);

private:
    Ui::LoginWindow *ui;
    bool m_isLoggedIn;
    QString m_currentUsername;

    void setupUI();
    bool validateCredentials(const QString &username, const QString &password);
    void setupConnections();
};

#endif // LOGINWINDOW_H
