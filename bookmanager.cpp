#include "bookmanager.h"
#include "databasemanager.h"
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QDebug>
#include <QStringConverter>
BookManager::BookManager(QObject* parent) : QObject(parent)
{
}

BookManager::~BookManager()
{
}

bool BookManager::addBook(const QString& bookId, const QString& bookName, const QString& author, const QString& category, int stock)
{
    return DatabaseManager::instance().addBook(bookId, bookName, author, category, stock);
}

bool BookManager::updateBook(const QString& bookId, const QString& bookName, const QString& author, const QString& category, int stock)
{
    return DatabaseManager::instance().updateBook(bookId, bookName, author, category, stock);
}

bool BookManager::deleteBook(const QString& bookId)
{
    return DatabaseManager::instance().deleteBook(bookId);
}

QSqlQueryModel* BookManager::getBooks()
{
    return DatabaseManager::instance().getBooks();
}

bool BookManager::exportToCSV(const QString& filePath)
{
    QSqlQueryModel* model = getBooks();
    if (!model) {
        return false;
    }

    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return false;
    }

    QTextStream out(&file);
    // Set UTF-8 encoding to avoid Chinese garbled characters
    out.setEncoding(QStringConverter::Utf8);

    // Write header
    for (int i = 0; i < model->columnCount(); ++i) {
        out << model->headerData(i, Qt::Horizontal).toString();
        if (i < model->columnCount() - 1) {
            out << ",";
        }
    }
    out << "\n";

    // Write data
    for (int i = 0; i < model->rowCount(); ++i) {
        for (int j = 0; j < model->columnCount(); ++j) {
            out << model->data(model->index(i, j)).toString();
            if (j < model->columnCount() - 1) {
                out << ",";
            }
        }
        out << "\n";
    }

    file.close();
    delete model;

    // Show success message
    QMessageBox::information(nullptr, "导出成功", "图书数据已成功导出到CSV文件！");
    return true;
}

bool BookManager::importFromCSV(const QString& filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return false;
    }

    QTextStream in(&file);
    // Set UTF-8 encoding to avoid Chinese garbled characters
   in.setEncoding(QStringConverter::Utf8);

    QString header = in.readLine(); // Skip header line
    int importedCount = 0;
    int skippedCount = 0;

    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList fields = line.split(",");

        if (fields.size() >= 5) {
            QString bookId = fields[0];
            QString bookName = fields[1];
            QString author = fields[2];
            QString category = fields[3];
            int stock = fields[4].toInt();

            // Try to add book
            if (addBook(bookId, bookName, author, category, stock)) {
                importedCount++;
            } else {
                // Book ID already exists, skip and log
                qDebug() << "跳过导入：图书ID" << bookId << "已存在";
                skippedCount++;
            }
        }
    }

    file.close();

    // Show import result
    QString message = QString("导入完成！\n成功导入：%1条记录\n跳过重复：%2条记录").arg(importedCount).arg(skippedCount);
    QMessageBox::information(nullptr, "导入完成", message);
    return true;
}
