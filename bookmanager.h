#ifndef BOOKMANAGER_H
#define BOOKMANAGER_H

#include <QObject>
#include <QSqlQueryModel>
#include <QString>

class BookManager : public QObject
{
    Q_OBJECT
    
public:
    explicit BookManager(QObject* parent = nullptr);
    ~BookManager();
    
    bool addBook(const QString& bookId, const QString& bookName, const QString& author, const QString& category, int stock);
    bool updateBook(const QString& bookId, const QString& bookName, const QString& author, const QString& category, int stock);
    bool deleteBook(const QString& bookId);
    QSqlQueryModel* getBooks();
    
    bool exportToCSV(const QString& filePath);
    bool importFromCSV(const QString& filePath);
    
private:
    // No direct database access, use DatabaseManager singleton
};

#endif // BOOKMANAGER_H
