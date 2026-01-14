#ifndef REGISTERDIALOG_H
#define REGISTERDIALOG_H

#include <QDialog>

namespace Ui {
class RegisterDialog;
}

class RegisterDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RegisterDialog(QWidget *parent = nullptr);
    ~RegisterDialog();

signals:
    void registrationSuccessful(const QString &username);

public slots:
    void on_registerButton_clicked();
    void on_cancelButton_clicked();
    void on_usernameEdit_textChanged(const QString &text);
    void on_passwordEdit_textChanged(const QString &text);
    void on_confirmPasswordEdit_textChanged(const QString &text);

private:
    Ui::RegisterDialog *ui;
    bool validateRegistration();
    bool registerUser(const QString &username, const QString &password);
    bool isUsernameAvailable(const QString &username);
};

#endif // REGISTERDIALOG_H