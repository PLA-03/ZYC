#include "readermanager.h"
#include "databasemanager.h"
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QDebug>
#include <QStringConverter>

ReaderManager::ReaderManager(QObject* parent) : QObject(parent)
{
}

ReaderManager::~ReaderManager()
{
}

bool ReaderManager::addReader(const QString& readerId, const QString& name, const QString& phone, const QString& gender)
{
    return DatabaseManager::instance().addReader(readerId, name, phone, gender);
}

bool ReaderManager::updateReader(const QString& readerId, const QString& name, const QString& phone, const QString& gender)
{
    return DatabaseManager::instance().updateReader(readerId, name, phone, gender);
}

bool ReaderManager::deleteReader(const QString& readerId)
{
    return DatabaseManager::instance().deleteReader(readerId);
}

QSqlQueryModel* ReaderManager::getReaders()
{
    return DatabaseManager::instance().getReaders();
}

bool ReaderManager::exportToCSV(const QString& filePath)
{
    QSqlQueryModel* model = getReaders();
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
    QMessageBox::information(nullptr, "导出成功", "读者数据已成功导出到CSV文件！");
    return true;
}

bool ReaderManager::importFromCSV(const QString& filePath)
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

        if (fields.size() >= 3) {
            QString readerId = fields[0];
            QString name = fields[1];
            QString phone = fields[2];

            // Try to add reader
            if (addReader(readerId, name, phone)) {
                importedCount++;
            } else {
                // Reader ID already exists, skip and log
                qDebug() << "跳过导入：读者ID" << readerId << "已存在";
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
