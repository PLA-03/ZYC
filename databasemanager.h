#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QString>
#include <QDateTime>
#include <QDate>          // 补充QDate头文件（参数用到QDate）
#include <QSqlQueryModel> // 解决QSqlQueryModel未识别
#include <QSqlRecord>     // 解决QSqlRecord不完整类型
#include <QSqlField>      // 可选（若用到QSqlField）
#include <QFile>          // CSV导入导出需要文件操作
#include <QIODevice>      // 文件读写模式依赖

class DatabaseManager : public QObject
{
    Q_OBJECT

public:
    // 单例模式
    static DatabaseManager& instance();
    static DatabaseManager& getInstance() { return instance(); }

    // 数据库连接/断开
    bool openDatabase();
    bool closeDatabase();

    // Books表操作
    bool createBookTable();
    bool addBook(const QString& bookId, const QString& bookName, const QString& author,
                 const QString& category, int stock);
    bool updateBook(const QString& bookId, const QString& bookName, const QString& author,
                    const QString& category, int stock);
    bool deleteBook(const QString& bookId);
    QSqlQueryModel* getBooks();
    QSqlQueryModel* getBookById(const QString& bookId);

    // Readers表操作
    bool createReaderTable();
    bool addReader(const QString& readerId, const QString& name, const QString& phone, const QString& gender = "未知");
    bool updateReader(const QString& readerId, const QString& name, const QString& phone, const QString& gender = "未知");
    bool deleteReader(const QString& readerId);
    QSqlQueryModel* getReaders();

    // Borrows表操作
    bool createBorrowTable();
    bool addBorrow(const QString& bookId, const QString& readerId, const QDate& borrowDate, const QDate& dueDate);
    bool returnBook(int borrowId, const QDate& returnDate);
    QSqlQueryModel* getBorrows();
    QSqlQueryModel* getActiveBorrows();
    QSqlQueryModel* getOverdueBorrows();
    
    // Book search functionality
    QSqlQueryModel* searchBooks(const QString& keyword, const QString& searchBy = "all");

    // CSV导入导出
    bool exportToCSV(const QString& tableName, const QString& filePath);
    bool importFromCSV(const QString& tableName, const QString& filePath);

    QSqlDatabase& getDatabase();
    const QSqlDatabase& getDatabase() const;

private:
    explicit DatabaseManager(QObject* parent = nullptr);
    ~DatabaseManager() override;

    QSqlDatabase m_database;
    static DatabaseManager* m_instance;

    DatabaseManager(const DatabaseManager&) = delete;
    DatabaseManager& operator=(const DatabaseManager&) = delete;
};

#endif // DATABASEMANAGER_H
