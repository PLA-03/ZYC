#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QString>
#include <QDateTime>
#include <QSqlQuery>
#include <QSqlQueryModel> // 解决QSqlQueryModel未识别
#include <QSqlRecord>     // 解决QSqlRecord不完整类型
#include <QSqlField>      // 可选（若用到QSqlField）

class DatabaseManager : public QObject
{
    Q_OBJECT
    
public:
    static DatabaseManager& instance();
    static DatabaseManager& getInstance(); // Alias for instance() to meet user requirement
    
    bool openDatabase(); // Renamed from connect() to meet user requirement
    bool disconnect();
    
    // Books table operations
    bool createBookTable();
    bool addBook(const QString& bookId, const QString& bookName, const QString& author, const QString& category, int stock); // Modified to accept QString bookId
    bool updateBook(const QString& bookId, const QString& bookName, const QString& author, const QString& category, int stock); // Modified to accept QString bookId
    bool deleteBook(const QString& bookId); // Modified to accept QString bookId
    QSqlQueryModel* getBooks();
    QSqlQueryModel* getBookById(const QString& bookId); // Added method to get book by ID
    
    // Readers table operations
    bool createReaderTable();
    bool addReader(const QString& readerId, const QString& name, const QString& phone); // Modified to accept QString readerId
    bool updateReader(const QString& readerId, const QString& name, const QString& phone); // Modified to accept QString readerId
    bool deleteReader(const QString& readerId); // Modified to accept QString readerId
    QSqlQueryModel* getReaders();
    
    // Borrows table operations
    bool createBorrowTable();
    bool addBorrow(const QString& bookId, const QString& readerId, const QDate& borrowDate, const QDate& dueDate); // Modified to accept QString IDs and QDate
    bool returnBook(int borrowId, const QDate& returnDate); // Modified to accept QDate
    QSqlQueryModel* getBorrows();
    QSqlQueryModel* getActiveBorrows();
    
    // CSV import/export
    bool exportToCSV(const QString& tableName, const QString& filePath);
    bool importFromCSV(const QString& tableName, const QString& filePath);
    
    QSqlDatabase getDatabase() const;
    
private:
    DatabaseManager(QObject* parent = nullptr);
    ~DatabaseManager();
    
    QSqlDatabase m_database;
    static DatabaseManager* m_instance;
};

#endif // DATABASEMANAGER_H
