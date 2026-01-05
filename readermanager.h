#ifndef READERMANAGER_H
#define READERMANAGER_H

#include <QObject>
#include <QSqlQueryModel>
#include <QString>

class ReaderManager : public QObject
{
    Q_OBJECT
    
public:
    explicit ReaderManager(QObject* parent = nullptr);
    ~ReaderManager();
    
    bool addReader(const QString& readerId, const QString& name, const QString& phone);
    bool updateReader(const QString& readerId, const QString& name, const QString& phone);
    bool deleteReader(const QString& readerId);
    QSqlQueryModel* getReaders();
    
    bool exportToCSV(const QString& filePath);
    bool importFromCSV(const QString& filePath);
    
private:
    // No direct database access, use DatabaseManager singleton
};

#endif // READERMANAGER_H
