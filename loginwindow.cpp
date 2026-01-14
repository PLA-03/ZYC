#include "loginwindow.h"
#include "ui_loginwindow.h"
#include "databasemanager.h"
#include "registerdialog.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QFont>
#include <QPalette>
#include <QPixmap>

LoginWindow::LoginWindow(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::LoginWindow)
    , m_isLoggedIn(false)
{
    ui->setupUi(this);
    setupUI();
}

LoginWindow::~LoginWindow()
{
    delete ui;
}

bool LoginWindow::isLoggedIn() const
{
    return m_isLoggedIn;
}

QString LoginWindow::getCurrentUsername() const
{
    return m_currentUsername;
}

void LoginWindow::setupUI()
{
    // Remove the automatic connections created by the UI file
    disconnect(ui->usernameEdit, &QLineEdit::textChanged, this, nullptr);
    disconnect(ui->passwordEdit, &QLineEdit::textChanged, this, nullptr);
    disconnect(ui->loginButton, &QPushButton::clicked, this, nullptr);
    disconnect(ui->cancelButton, &QPushButton::clicked, this, nullptr);
    
    // Set window properties
    setWindowTitle(tr("图书管理系统 - 登录"));
    setFixedSize(400, 300);
    
    // Set up connections
    setupConnections();
    
    // Set initial state
    ui->loginButton->setEnabled(false);
    ui->usernameEdit->setFocus();
}

void LoginWindow::setupConnections()
{
    // Connect UI signals to slots
    connect(ui->usernameEdit, &QLineEdit::textChanged, this, &LoginWindow::on_usernameEdit_textChanged);
    connect(ui->passwordEdit, &QLineEdit::textChanged, this, &LoginWindow::on_passwordEdit_textChanged);
    connect(ui->loginButton, &QPushButton::clicked, this, &LoginWindow::on_loginButton_clicked);
    connect(ui->registerButton, &QPushButton::clicked, this, &LoginWindow::on_registerButton_clicked);
    connect(ui->cancelButton, &QPushButton::clicked, this, &LoginWindow::on_cancelButton_clicked);
}

void LoginWindow::on_loginButton_clicked()
{
    QString username = ui->usernameEdit->text().trimmed();
    QString password = ui->passwordEdit->text();
    
    if (validateCredentials(username, password)) {
        m_isLoggedIn = true;
        m_currentUsername = username;
        accept();
    } else {
        QMessageBox::warning(this, tr("登录失败"), tr("用户名或密码错误！"));
        ui->passwordEdit->clear();
        ui->passwordEdit->setFocus();
    }
}

void LoginWindow::on_cancelButton_clicked()
{
    reject();
}

void LoginWindow::on_registerButton_clicked()
{
    RegisterDialog registerDialog(this);
    
    // Connect registration success signal
    connect(&registerDialog, &RegisterDialog::registrationSuccessful, 
            this, &LoginWindow::onRegistrationSuccessful);
    
    // Show register dialog
    registerDialog.exec();
}

void LoginWindow::onRegistrationSuccessful(const QString &username)
{
    // Set the username in the login window after successful registration
    ui->usernameEdit->setText(username);
    ui->passwordEdit->setFocus();
}

void LoginWindow::on_usernameEdit_textChanged(const QString &text)
{
    ui->loginButton->setEnabled(!text.isEmpty() && !ui->passwordEdit->text().isEmpty());
}

void LoginWindow::on_passwordEdit_textChanged(const QString &text)
{
    ui->loginButton->setEnabled(!ui->usernameEdit->text().isEmpty() && !text.isEmpty());
}

bool LoginWindow::validateCredentials(const QString &username, const QString &password)
{
    // TODO: Replace with actual database validation
    // For demo purposes, use hardcoded credentials
    if (username == "admin" && password == "admin123") {
        return true;
    }
    
    // Check if users table exists, if not, create it and add default admin user
    QSqlQuery checkTableQuery;
    if (!checkTableQuery.exec("SELECT name FROM sqlite_master WHERE type='table' AND name='users'") || !checkTableQuery.next()) {
        // Create users table
        QSqlQuery createTableQuery;
        if (!createTableQuery.exec(
            "CREATE TABLE IF NOT EXISTS users ("
            "user_id INTEGER PRIMARY KEY AUTOINCREMENT, "
            "username TEXT NOT NULL UNIQUE, "
            "password TEXT NOT NULL, "
            "role TEXT NOT NULL DEFAULT 'user'"
            ")"
        )) {
            qDebug() << "Failed to create users table:" << createTableQuery.lastError().text();
            return false;
        }
        
        // Insert default admin user
        QSqlQuery insertAdminQuery;
        insertAdminQuery.prepare(
            "INSERT INTO users (username, password, role) "
            "VALUES (:username, :password, :role)"
        );
        insertAdminQuery.bindValue(":username", "admin");
        insertAdminQuery.bindValue(":password", "admin123"); // In production, use proper password hashing
        insertAdminQuery.bindValue(":role", "admin");
        
        if (!insertAdminQuery.exec()) {
            qDebug() << "Failed to insert default admin user:" << insertAdminQuery.lastError().text();
            return false;
        }
        
        // Check credentials again for the newly created admin
        return (username == "admin" && password == "admin123");
    }
    
    // Check credentials against database
    QSqlQuery query;
    query.prepare("SELECT * FROM users WHERE username = :username AND password = :password");
    query.bindValue(":username", username);
    query.bindValue(":password", password); // In production, use proper password hashing
    
    if (!query.exec()) {
        qDebug() << "Login query failed:" << query.lastError().text();
        return false;
    }
    
    return query.next();
}