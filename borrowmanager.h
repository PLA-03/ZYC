#ifndef BORROWMANAGER_H
#define BORROWMANAGER_H

#include <QObject>
#include <QSqlQueryModel>
#include <QString>
#include <QDateTime>

class BorrowManager : public QObject
{
    Q_OBJECT
    
public:
    explicit BorrowManager(QObject* parent = nullptr);
    ~BorrowManager();
    bool exportBorrows(const QString& filePath);
    // Original methods for backward compatibility
    bool borrowBook(const QString& bookId, const QString& readerId, const QDate& borrowDate, const QDate& dueDate);
    bool returnBook(int borrowId, const QDate& returnDate);
    
    // New methods as per user requirements
    bool checkOutBook(const QString& bookId, const QString& readerId);
    bool checkInBook(const QString& borrowId);
    QList<QMap<QString, QVariant>> getOverdueRecords();
    
    QSqlQueryModel* getAllBorrows();
    QSqlQueryModel* getActiveBorrows();
    
    bool exportToCSV(const QString& filePath);
    
private:
    // No direct database access, use DatabaseManager singleton
};

#endif // BORROWMANAGER_H
