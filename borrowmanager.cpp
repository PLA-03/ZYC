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

bool BorrowManager::checkOutBook(const QString& bookId, const QString& readerId)
{
    // Get database instance
    DatabaseManager& db = DatabaseManager::instance();
    
    // Verify book exists and has stock > 0
    QSqlQueryModel* bookModel = db.getBookById(bookId);
    if (!bookModel || bookModel->rowCount() == 0) {
        qDebug() << "Checkout failed: Book with ID" << bookId << "does not exist";
        delete bookModel;
        return false;
    }
    
    int stock = bookModel->record(0).value("stock").toInt();
    delete bookModel;
    
    if (stock <= 0) {
        qDebug() << "Checkout failed: Book with ID" << bookId << "is out of stock";
        return false;
    }
    
    // Verify reader exists
    QSqlQuery checkReaderQuery;
    checkReaderQuery.prepare("SELECT * FROM readers WHERE reader_id = :readerId");
    checkReaderQuery.bindValue(":readerId", readerId.toInt());
    
    if (!(checkReaderQuery.exec() && checkReaderQuery.next())) {
        qDebug() << "Checkout failed: Reader with ID" << readerId << "does not exist";
        return false;
    }
    
    // Set borrow date to today and due date to 30 days from today
    QDate borrowDate = QDate::currentDate();
    QDate dueDate = borrowDate.addDays(30);
    
    // Add borrow record
    if (!db.addBorrow(bookId, readerId, borrowDate, dueDate)) {
        qDebug() << "Checkout failed: Could not add borrow record";
        return false;
    }
    
    return true;
}

bool BorrowManager::checkInBook(const QString& borrowId)
{
    // Get database instance
    DatabaseManager& db = DatabaseManager::instance();
    QSqlDatabase database = db.getDatabase();
    
    // Start transaction
    database.transaction();
    
    // Verify borrow record exists and is not returned
    QSqlQuery checkBorrowQuery;
    checkBorrowQuery.prepare("SELECT * FROM borrows WHERE borrow_id = :borrowId AND return_date IS NULL");
    checkBorrowQuery.bindValue(":borrowId", borrowId.toInt());
    
    if (!(checkBorrowQuery.exec() && checkBorrowQuery.next())) {
        qDebug() << "Checkin failed: Borrow record with ID" << borrowId << "does not exist or is already returned";
        database.rollback();
        return false;
    }
    
    // Get book ID from borrow record
    QString bookId = QString::number(checkBorrowQuery.value("book_id").toInt());
    
    // Update return date
    QSqlQuery updateBorrowQuery;
    updateBorrowQuery.prepare("UPDATE borrows SET return_date = :returnDate WHERE borrow_id = :borrowId");
    updateBorrowQuery.bindValue(":returnDate", QDate::currentDate().toString("yyyy-MM-dd"));
    updateBorrowQuery.bindValue(":borrowId", borrowId.toInt());
    
    if (!updateBorrowQuery.exec()) {
        qDebug() << "Checkin failed: Could not update return date" << updateBorrowQuery.lastError().text();
        database.rollback();
        return false;
    }
    
    // Increase book stock
    QSqlQuery updateStockQuery;
    updateStockQuery.prepare("UPDATE books SET stock = stock + 1 WHERE book_id = :bookId");
    updateStockQuery.bindValue(":bookId", bookId.toInt());
    
    if (!updateStockQuery.exec()) {
        qDebug() << "Checkin failed: Could not update book stock" << updateStockQuery.lastError().text();
        database.rollback();
        return false;
    }
    
    // Commit transaction
    database.commit();
    return true;
}

QList<QMap<QString, QVariant>> BorrowManager::getOverdueRecords()
{
    QList<QMap<QString, QVariant>> overdueRecords;
    QSqlDatabase database = DatabaseManager::instance().getDatabase();
    
    // Get current date
    QDate currentDate = QDate::currentDate();
    QString currentDateStr = currentDate.toString("yyyy-MM-dd");
    
    // Query for overdue records
    QSqlQuery query;
    query.prepare("SELECT b.borrow_id, b.book_id, b.reader_id, b.borrow_date, b.due_date, "
                  "bo.book_name, r.name as reader_name "
                  "FROM borrows b "
                  "JOIN books bo ON b.book_id = bo.book_id "
                  "JOIN readers r ON b.reader_id = r.reader_id "
                  "WHERE b.return_date IS NULL AND b.due_date < :currentDate");
    query.bindValue(":currentDate", currentDateStr);
    
    if (!query.exec()) {
        qDebug() << "Failed to get overdue records:" << query.lastError().text();
        return overdueRecords;
    }
    
    // Process results
    while (query.next()) {
        QMap<QString, QVariant> record;
        record["borrow_id"] = query.value("borrow_id");
        record["book_id"] = query.value("book_id");
        record["reader_id"] = query.value("reader_id");
        record["borrow_date"] = query.value("borrow_date");
        record["due_date"] = query.value("due_date");
        record["book_name"] = query.value("book_name");
        record["reader_name"] = query.value("reader_name");
        overdueRecords.append(record);
    }
    
    return overdueRecords;
}

QSqlQueryModel* BorrowManager::getAllBorrows()
{
    return DatabaseManager::instance().getBorrows();
}

QSqlQueryModel* BorrowManager::getActiveBorrows()
{
    return DatabaseManager::instance().getActiveBorrows();
}

bool BorrowManager::exportBorrows(const QString& filePath)
{
    return DatabaseManager::instance().exportToCSV("borrows", filePath);
}

bool BorrowManager::exportToCSV(const QString& filePath)
{
    return exportBorrows(filePath);
}
