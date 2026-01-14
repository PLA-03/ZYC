#include "registerdialog.h"
#include "ui_registerdialog.h"
#include "databasemanager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QMessageBox>
#include <QString>

RegisterDialog::RegisterDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::RegisterDialog)
{
    ui->setupUi(this);
    
    // Remove automatic connections to avoid errors
    disconnect(ui->usernameEdit, &QLineEdit::textChanged, this, nullptr);
    disconnect(ui->passwordEdit, &QLineEdit::textChanged, this, nullptr);
    disconnect(ui->confirmPasswordEdit, &QLineEdit::textChanged, this, nullptr);
    disconnect(ui->registerButton, &QPushButton::clicked, this, nullptr);
    disconnect(ui->cancelButton, &QPushButton::clicked, this, nullptr);
    
    // Set up manual connections
    connect(ui->usernameEdit, &QLineEdit::textChanged, this, &RegisterDialog::on_usernameEdit_textChanged);
    connect(ui->passwordEdit, &QLineEdit::textChanged, this, &RegisterDialog::on_passwordEdit_textChanged);
    connect(ui->confirmPasswordEdit, &QLineEdit::textChanged, this, &RegisterDialog::on_confirmPasswordEdit_textChanged);
    connect(ui->registerButton, &QPushButton::clicked, this, &RegisterDialog::on_registerButton_clicked);
    connect(ui->cancelButton, &QPushButton::clicked, this, &RegisterDialog::on_cancelButton_clicked);
    
    // Set initial state
    ui->registerButton->setEnabled(false);
    ui->usernameEdit->setFocus();
}

RegisterDialog::~RegisterDialog()
{
    delete ui;
}

void RegisterDialog::on_registerButton_clicked()
{
    if (validateRegistration()) {
        QString username = ui->usernameEdit->text().trimmed();
        QString password = ui->passwordEdit->text();
        
        if (registerUser(username, password)) {
            QMessageBox::information(this, tr("注册成功"), tr("用户注册成功！"));
            emit registrationSuccessful(username);
            accept();
        } else {
            QMessageBox::critical(this, tr("注册失败"), tr("用户注册失败，请重试！"));
        }
    }
}

void RegisterDialog::on_cancelButton_clicked()
{
    reject();
}

void RegisterDialog::on_usernameEdit_textChanged(const QString &text)
{
    bool usernameValid = !text.trimmed().isEmpty();
    bool passwordValid = ui->passwordEdit->text().length() >= 6;
    bool passwordsMatch = ui->passwordEdit->text() == ui->confirmPasswordEdit->text();
    
    ui->registerButton->setEnabled(usernameValid && passwordValid && passwordsMatch);
}

void RegisterDialog::on_passwordEdit_textChanged(const QString &text)
{
    bool usernameValid = !ui->usernameEdit->text().trimmed().isEmpty();
    bool passwordValid = text.length() >= 6;
    bool passwordsMatch = text == ui->confirmPasswordEdit->text();
    
    ui->registerButton->setEnabled(usernameValid && passwordValid && passwordsMatch);
}

void RegisterDialog::on_confirmPasswordEdit_textChanged(const QString &text)
{
    bool usernameValid = !ui->usernameEdit->text().trimmed().isEmpty();
    bool passwordValid = ui->passwordEdit->text().length() >= 6;
    bool passwordsMatch = ui->passwordEdit->text() == text;
    
    ui->registerButton->setEnabled(usernameValid && passwordValid && passwordsMatch);
}

bool RegisterDialog::validateRegistration()
{
    QString username = ui->usernameEdit->text().trimmed();
    QString password = ui->passwordEdit->text();
    QString confirmPassword = ui->confirmPasswordEdit->text();
    
    // Check username
    if (username.isEmpty()) {
        QMessageBox::warning(this, tr("注册失败"), tr("用户名不能为空！"));
        ui->usernameEdit->setFocus();
        return false;
    }
    
    if (username.length() < 3) {
        QMessageBox::warning(this, tr("注册失败"), tr("用户名长度不能少于3个字符！"));
        ui->usernameEdit->setFocus();
        return false;
    }
    
    // Check password
    if (password.isEmpty()) {
        QMessageBox::warning(this, tr("注册失败"), tr("密码不能为空！"));
        ui->passwordEdit->setFocus();
        return false;
    }
    
    if (password.length() < 6) {
        QMessageBox::warning(this, tr("注册失败"), tr("密码长度不能少于6个字符！"));
        ui->passwordEdit->setFocus();
        return false;
    }
    
    // Check password confirmation
    if (password != confirmPassword) {
        QMessageBox::warning(this, tr("注册失败"), tr("两次输入的密码不一致！"));
        ui->confirmPasswordEdit->setFocus();
        return false;
    }
    
    // Check if username is available
    if (!isUsernameAvailable(username)) {
        QMessageBox::warning(this, tr("注册失败"), tr("该用户名已被注册！"));
        ui->usernameEdit->setFocus();
        return false;
    }
    
    return true;
}

bool RegisterDialog::registerUser(const QString &username, const QString &password)
{
    // Ensure users table exists (this will create it if it doesn't)
    QSqlQuery createTableQuery;
    if (!createTableQuery.exec(
        "CREATE TABLE IF NOT EXISTS users (" 
        "user_id INTEGER PRIMARY KEY AUTOINCREMENT, " 
        "username TEXT NOT NULL UNIQUE, " 
        "password TEXT NOT NULL, " 
        "role TEXT NOT NULL DEFAULT 'user'" 
        ")"
    )) {
        qDebug() << "Failed to ensure users table exists:" << createTableQuery.lastError().text();
        return false;
    }
    
    // Insert new user
    QSqlQuery insertQuery;
    insertQuery.prepare(
        "INSERT INTO users (username, password, role) " 
        "VALUES (:username, :password, :role)"
    );
    insertQuery.bindValue(":username", username);
    insertQuery.bindValue(":password", password); // In production, use proper password hashing
    insertQuery.bindValue(":role", "user");
    
    if (!insertQuery.exec()) {
        qDebug() << "Failed to insert new user:" << insertQuery.lastError().text();
        return false;
    }
    
    return true;
}

bool RegisterDialog::isUsernameAvailable(const QString &username)
{
    QSqlQuery query;
    query.prepare("SELECT COUNT(*) FROM users WHERE username = :username");
    query.bindValue(":username", username);
    
    if (!query.exec() || !query.next()) {
        qDebug() << "Failed to check username availability:" << query.lastError().text();
        return false;
    }
    
    return query.value(0).toInt() == 0;
}