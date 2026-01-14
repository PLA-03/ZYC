#include "borrowmanager.h"
#include "databasemanager.h"
#include <QDate>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QMap>
#include <QList>

BorrowManager::BorrowManager(QObject* parent) : QObject(parent)
{
}

BorrowManager::~BorrowManager()
{
}

bool BorrowManager::borrowBook(const QString& bookId, const QString& readerId, const QDate& borrowDate, const QDate& dueDate)
{
    return DatabaseManager::instance().addBorrow(bookId, readerId, borrowDate, dueDate);
}

bool BorrowManager::returnBook(int borrowId, const QDate& returnDate)
{
    return DatabaseManager::instance().returnBook(borrowId, returnDate);
}

QSqlQueryModel* BorrowManager::getAllBorrows()
{
    return DatabaseManager::instance().getBorrows();
}

bool BorrowManager::exportToCSV(const QString& filePath)
{
    return DatabaseManager::instance().exportToCSV("borrows", filePath);
}
