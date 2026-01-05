#include "databasemanager.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>

DatabaseManager* DatabaseManager::m_instance = nullptr;

DatabaseManager::DatabaseManager(QObject* parent) : QObject(parent)
{
    // Constructor is private, so we don't connect here
    // Connection will be established when openDatabase() is called explicitly
}

DatabaseManager::~DatabaseManager()
{
    disconnect();
}

DatabaseManager& DatabaseManager::instance()
{
    if (!m_instance) {
        m_instance = new DatabaseManager();
    }
    return *m_instance;
}

DatabaseManager& DatabaseManager::getInstance()
{
    return instance(); // Alias implementation
}

bool DatabaseManager::openDatabase()
{
    if (m_database.isOpen()) {
        return true;
    }
    
    m_database = QSqlDatabase::addDatabase("QSQLITE");
    m_database.setDatabaseName("library.db");
    
    if (!m_database.open()) {
        qDebug() << "Failed to open database:" << m_database.lastError().text();
        return false;
    }
    
    // Create tables if they don't exist
    if (!createBookTable()) {
        qDebug() << "Failed to create books table";
        return false;
    }
    
    if (!createReaderTable()) {
        qDebug() << "Failed to create readers table";
        return false;
    }
    
    if (!createBorrowTable()) {
        qDebug() << "Failed to create borrows table";
        return false;
    }
    
    return true;
}

bool DatabaseManager::disconnect()
{
    if (m_database.isOpen()) {
        m_database.close();
        return true;
    }
    return false;
}

bool DatabaseManager::createBookTable()
{
    QSqlQuery query;
    QString sql = "CREATE TABLE IF NOT EXISTS books (" 
                  "book_id INTEGER PRIMARY KEY AUTOINCREMENT, " 
                  "book_name TEXT NOT NULL, " 
                  "author TEXT NOT NULL, " 
                  "category TEXT NOT NULL, " 
                  "stock INTEGER NOT NULL DEFAULT 0" 
                  ")";
    
    if (!query.exec(sql)) {
        qDebug() << "Failed to create books table:" << query.lastError().text();
        return false;
    }
    return true;
}

bool DatabaseManager::addBook(const QString& bookId, const QString& bookName, const QString& author, const QString& category, int stock)
{
    // Check if book with the same ID already exists
    QSqlQuery checkQuery;
    checkQuery.prepare("SELECT * FROM books WHERE book_id = :bookId");
    checkQuery.bindValue(":bookId", bookId.toInt());
    
    if (checkQuery.exec() && checkQuery.next()) {
        qDebug() << "Failed to add book: Book with ID" << bookId << "already exists";
        return false;
    }
    
    QSqlQuery query;
    query.prepare("INSERT INTO books (book_id, book_name, author, category, stock) VALUES (:bookId, :bookName, :author, :category, :stock)");
    query.bindValue(":bookId", bookId.toInt());
    query.bindValue(":bookName", bookName);
    query.bindValue(":author", author);
    query.bindValue(":category", category);
    query.bindValue(":stock", stock);
    
    if (!query.exec()) {
        qDebug() << "Failed to add book:" << query.lastError().text();
        return false;
    }
    return true;
}

bool DatabaseManager::updateBook(const QString& bookId, const QString& bookName, const QString& author, const QString& category, int stock)
{
    // Check if book exists
    QSqlQuery checkQuery;
    checkQuery.prepare("SELECT * FROM books WHERE book_id = :bookId");
    checkQuery.bindValue(":bookId", bookId.toInt());
    
    if (!(checkQuery.exec() && checkQuery.next())) {
        qDebug() << "Failed to update book: Book with ID" << bookId << "does not exist";
        return false;
    }
    
    QSqlQuery query;
    query.prepare("UPDATE books SET book_name = :bookName, author = :author, category = :category, stock = :stock WHERE book_id = :bookId");
    query.bindValue(":bookName", bookName);
    query.bindValue(":author", author);
    query.bindValue(":category", category);
    query.bindValue(":stock", stock);
    query.bindValue(":bookId", bookId.toInt());
    
    if (!query.exec()) {
        qDebug() << "Failed to update book:" << query.lastError().text();
        return false;
    }
    
    if (query.numRowsAffected() == 0) {
        qDebug() << "No book was updated with ID" << bookId;
        return false;
    }
    
    return true;
}

bool DatabaseManager::deleteBook(const QString& bookId)
{
    // Check if book exists
    QSqlQuery checkQuery;
    checkQuery.prepare("SELECT * FROM books WHERE book_id = :bookId");
    checkQuery.bindValue(":bookId", bookId.toInt());
    
    if (!(checkQuery.exec() && checkQuery.next())) {
        qDebug() << "Failed to delete book: Book with ID" << bookId << "does not exist";
        return false;
    }
    
    QSqlQuery query;
    query.prepare("DELETE FROM books WHERE book_id = :bookId");
    query.bindValue(":bookId", bookId.toInt());
    
    if (!query.exec()) {
        qDebug() << "Failed to delete book:" << query.lastError().text();
        return false;
    }
    
    if (query.numRowsAffected() == 0) {
        qDebug() << "No book was deleted with ID" << bookId;
        return false;
    }
    
    return true;
}

QSqlQueryModel* DatabaseManager::getBooks()
{
    QSqlQueryModel* model = new QSqlQueryModel();
    model->setQuery("SELECT * FROM books");
    
    if (model->lastError().isValid()) {
        qDebug() << "Failed to get books:" << model->lastError().text();
        delete model;
        return nullptr;
    }
    
    // Set header labels
    model->setHeaderData(0, Qt::Horizontal, QObject::tr("图书 ID"));
    model->setHeaderData(1, Qt::Horizontal, QObject::tr("图书名称"));
    model->setHeaderData(2, Qt::Horizontal, QObject::tr("作者"));
    model->setHeaderData(3, Qt::Horizontal, QObject::tr("分类"));
    model->setHeaderData(4, Qt::Horizontal, QObject::tr("库存"));
    
    return model;
}

QSqlQueryModel* DatabaseManager::getBookById(const QString& bookId)
{
    QSqlQueryModel* model = new QSqlQueryModel();
    
    QSqlQuery query;
    query.prepare("SELECT * FROM books WHERE book_id = :bookId");
    query.bindValue(":bookId", bookId.toInt());
    
    if (!query.exec()) {
        qDebug() << "Failed to get book by ID:" << query.lastError().text();
        delete model;
        return nullptr;
    }
    
    model->setQuery(query);
    
    if (model->lastError().isValid()) {
        qDebug() << "Failed to get book by ID:" << model->lastError().text();
        delete model;
        return nullptr;
    }
    
    return model;
}

bool DatabaseManager::createReaderTable()
{
    QSqlQuery query;
    QString sql = "CREATE TABLE IF NOT EXISTS readers (" 
                  "reader_id INTEGER PRIMARY KEY AUTOINCREMENT, " 
                  "name TEXT NOT NULL, " 
                  "phone TEXT NOT NULL" 
                  ")";
    
    if (!query.exec(sql)) {
        qDebug() << "Failed to create readers table:" << query.lastError().text();
        return false;
    }
    return true;
}

bool DatabaseManager::addReader(const QString& readerId, const QString& name, const QString& phone)
{
    // Check if reader with the same ID already exists
    QSqlQuery checkQuery;
    checkQuery.prepare("SELECT * FROM readers WHERE reader_id = :readerId");
    checkQuery.bindValue(":readerId", readerId.toInt());
    
    if (checkQuery.exec() && checkQuery.next()) {
        qDebug() << "Failed to add reader: Reader with ID" << readerId << "already exists";
        return false;
    }
    
    QSqlQuery query;
    query.prepare("INSERT INTO readers (reader_id, name, phone) VALUES (:readerId, :name, :phone)");
    query.bindValue(":readerId", readerId.toInt());
    query.bindValue(":name", name);
    query.bindValue(":phone", phone);
    
    if (!query.exec()) {
        qDebug() << "Failed to add reader:" << query.lastError().text();
        return false;
    }
    return true;
}

bool DatabaseManager::updateReader(const QString& readerId, const QString& name, const QString& phone)
{
    // Check if reader exists
    QSqlQuery checkQuery;
    checkQuery.prepare("SELECT * FROM readers WHERE reader_id = :readerId");
    checkQuery.bindValue(":readerId", readerId.toInt());
    
    if (!(checkQuery.exec() && checkQuery.next())) {
        qDebug() << "Failed to update reader: Reader with ID" << readerId << "does not exist";
        return false;
    }
    
    QSqlQuery query;
    query.prepare("UPDATE readers SET name = :name, phone = :phone WHERE reader_id = :readerId");
    query.bindValue(":name", name);
    query.bindValue(":phone", phone);
    query.bindValue(":readerId", readerId.toInt());
    
    if (!query.exec()) {
        qDebug() << "Failed to update reader:" << query.lastError().text();
        return false;
    }
    
    if (query.numRowsAffected() == 0) {
        qDebug() << "No reader was updated with ID" << readerId;
        return false;
    }
    
    return true;
}

bool DatabaseManager::deleteReader(const QString& readerId)
{
    // Check if reader exists
    QSqlQuery checkQuery;
    checkQuery.prepare("SELECT * FROM readers WHERE reader_id = :readerId");
    checkQuery.bindValue(":readerId", readerId.toInt());
    
    if (!(checkQuery.exec() && checkQuery.next())) {
        qDebug() << "Failed to delete reader: Reader with ID" << readerId << "does not exist";
        return false;
    }
    
    QSqlQuery query;
    query.prepare("DELETE FROM readers WHERE reader_id = :readerId");
    query.bindValue(":readerId", readerId.toInt());
    
    if (!query.exec()) {
        qDebug() << "Failed to delete reader:" << query.lastError().text();
        return false;
    }
    
    if (query.numRowsAffected() == 0) {
        qDebug() << "No reader was deleted with ID" << readerId;
        return false;
    }
    
    return true;
}

QSqlQueryModel* DatabaseManager::getReaders()
{
    QSqlQueryModel* model = new QSqlQueryModel();
    model->setQuery("SELECT * FROM readers");
    
    // Set header labels
    model->setHeaderData(0, Qt::Horizontal, QObject::tr("读者 ID"));
    model->setHeaderData(1, Qt::Horizontal, QObject::tr("姓名"));
    model->setHeaderData(2, Qt::Horizontal, QObject::tr("电话"));
    
    return model;
}

bool DatabaseManager::createBorrowTable()
{
    QSqlQuery query;
    QString sql = "CREATE TABLE IF NOT EXISTS borrows (" 
                  "borrow_id INTEGER PRIMARY KEY AUTOINCREMENT, " 
                  "book_id INTEGER NOT NULL, " 
                  "reader_id INTEGER NOT NULL, " 
                  "borrow_date TEXT NOT NULL, " 
                  "due_date TEXT NOT NULL, " 
                  "return_date TEXT, " 
                  "FOREIGN KEY (book_id) REFERENCES books(book_id), " 
                  "FOREIGN KEY (reader_id) REFERENCES readers(reader_id)" 
                  ")";
    
    if (!query.exec(sql)) {
        qDebug() << "Failed to create borrows table:" << query.lastError().text();
        return false;
    }
    return true;
}

bool DatabaseManager::addBorrow(const QString& bookId, const QString& readerId, const QDate& borrowDate, const QDate& dueDate)
{
    // Check if book exists
    QSqlQuery bookCheckQuery;
    bookCheckQuery.prepare("SELECT * FROM books WHERE book_id = :bookId");
    bookCheckQuery.bindValue(":bookId", bookId.toInt());
    
    if (!(bookCheckQuery.exec() && bookCheckQuery.next())) {
        qDebug() << "Failed to add borrow: Book with ID" << bookId << "does not exist";
        return false;
    }
    
    // Check if reader exists
    QSqlQuery readerCheckQuery;
    readerCheckQuery.prepare("SELECT * FROM readers WHERE reader_id = :readerId");
    readerCheckQuery.bindValue(":readerId", readerId.toInt());
    
    if (!(readerCheckQuery.exec() && readerCheckQuery.next())) {
        qDebug() << "Failed to add borrow: Reader with ID" << readerId << "does not exist";
        return false;
    }
    
    // Check if due date is after borrow date
    if (dueDate <= borrowDate) {
        qDebug() << "Failed to add borrow: Due date must be after borrow date";
        return false;
    }
    
    // Check if book has available stock
    int stock = bookCheckQuery.value("stock").toInt();
    if (stock <= 0) {
        qDebug() << "Failed to add borrow: Book with ID" << bookId << "is out of stock";
        return false;
    }
    
    QSqlQuery query;
    query.prepare("INSERT INTO borrows (book_id, reader_id, borrow_date, due_date) VALUES (:bookId, :readerId, :borrowDate, :dueDate)");
    query.bindValue(":bookId", bookId.toInt());
    query.bindValue(":readerId", readerId.toInt());
    query.bindValue(":borrowDate", borrowDate.toString("yyyy-MM-dd"));
    query.bindValue(":dueDate", dueDate.toString("yyyy-MM-dd"));
    
    if (!query.exec()) {
        qDebug() << "Failed to add borrow:" << query.lastError().text();
        return false;
    }
    
    // Decrease book stock
    query.prepare("UPDATE books SET stock = stock - 1 WHERE book_id = :bookId");
    query.bindValue(":bookId", bookId.toInt());
    
    if (!query.exec()) {
        qDebug() << "Failed to update book stock:" << query.lastError().text();
        // Rollback borrow transaction
        query.prepare("DELETE FROM borrows WHERE borrow_id = last_insert_rowid()");
        query.exec();
        return false;
    }
    
    return true;
}

bool DatabaseManager::returnBook(int borrowId, const QDate& returnDate)
{
    // Check if borrow record exists and is not already returned
    QSqlQuery checkQuery;
    checkQuery.prepare("SELECT * FROM borrows WHERE borrow_id = :borrowId AND return_date IS NULL");
    checkQuery.bindValue(":borrowId", borrowId);
    
    if (!(checkQuery.exec() && checkQuery.next())) {
        qDebug() << "Failed to return book: Borrow record with ID" << borrowId << "does not exist or is already returned";
        return false;
    }
    
    int bookId = checkQuery.value("book_id").toInt();
    
    QSqlQuery query;
    query.prepare("UPDATE borrows SET return_date = :returnDate WHERE borrow_id = :borrowId");
    query.bindValue(":returnDate", returnDate.toString("yyyy-MM-dd"));
    query.bindValue(":borrowId", borrowId);
    
    if (!query.exec()) {
        qDebug() << "Failed to return book:" << query.lastError().text();
        return false;
    }
    
    if (query.numRowsAffected() == 0) {
        qDebug() << "No borrow record was updated with ID" << borrowId;
        return false;
    }
    
    // Increase book stock
    query.prepare("UPDATE books SET stock = stock + 1 WHERE book_id = :bookId");
    query.bindValue(":bookId", bookId);
    
    if (!query.exec()) {
        qDebug() << "Failed to update book stock:" << query.lastError().text();
        // Rollback return transaction
        query.prepare("UPDATE borrows SET return_date = NULL WHERE borrow_id = :borrowId");
        query.bindValue(":borrowId", borrowId);
        query.exec();
        return false;
    }
    
    return true;
}

QSqlQueryModel* DatabaseManager::getBorrows()
{
    QSqlQueryModel* model = new QSqlQueryModel();
    model->setQuery("SELECT * FROM borrows");
    
    // Set header labels
    model->setHeaderData(0, Qt::Horizontal, QObject::tr("Borrow ID"));
    model->setHeaderData(1, Qt::Horizontal, QObject::tr("Book ID"));
    model->setHeaderData(2, Qt::Horizontal, QObject::tr("Reader ID"));
    model->setHeaderData(3, Qt::Horizontal, QObject::tr("Borrow Date"));
    model->setHeaderData(4, Qt::Horizontal, QObject::tr("Due Date"));
    model->setHeaderData(5, Qt::Horizontal, QObject::tr("Return Date"));
    
    return model;
}

QSqlQueryModel* DatabaseManager::getActiveBorrows()
{
    QSqlQueryModel* model = new QSqlQueryModel();
    model->setQuery("SELECT * FROM borrows WHERE return_date IS NULL");
    
    // Set header labels
    model->setHeaderData(0, Qt::Horizontal, QObject::tr("Borrow ID"));
    model->setHeaderData(1, Qt::Horizontal, QObject::tr("Book ID"));
    model->setHeaderData(2, Qt::Horizontal, QObject::tr("Reader ID"));
    model->setHeaderData(3, Qt::Horizontal, QObject::tr("Borrow Date"));
    model->setHeaderData(4, Qt::Horizontal, QObject::tr("Due Date"));
    model->setHeaderData(5, Qt::Horizontal, QObject::tr("Return Date"));
    
    return model;
}

bool DatabaseManager::exportToCSV(const QString& tableName, const QString& filePath)
{
    QSqlQueryModel* model = nullptr;
    
    if (tableName == "books") {
        model = getBooks();
    } else if (tableName == "readers") {
        model = getReaders();
    } else if (tableName == "borrows") {
        model = getBorrows();
    } else {
        return false;
    }
    
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return false;
    }
    
    QTextStream out(&file);
    
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
    return true;
}

bool DatabaseManager::importFromCSV(const QString& tableName, const QString& filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return false;
    }
    
    QTextStream in(&file);
    QString header = in.readLine(); // Skip header line
    
    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList fields = line.split(",");
        
        if (tableName == "books" && fields.size() >= 4) {
            QString bookId = fields[0];
            QString bookName = fields[1];
            QString author = fields[2];
            QString category = fields[3];
            int stock = fields[4].toInt();
            
            // Check if book exists, update if it does
            QSqlQuery query;
            query.prepare("SELECT * FROM books WHERE book_id = :bookId");
            query.bindValue(":bookId", bookId.toInt());
            
            if (query.exec() && query.next()) {
                updateBook(bookId, bookName, author, category, stock);
            } else {
                // Insert new book with the given ID
                addBook(bookId, bookName, author, category, stock);
            }
        } else if (tableName == "readers" && fields.size() >= 3) {
            QString readerId = fields[0];
            QString name = fields[1];
            QString phone = fields[2];
            
            QSqlQuery query;
            query.prepare("SELECT * FROM readers WHERE reader_id = :readerId");
            query.bindValue(":readerId", readerId.toInt());
            
            if (query.exec() && query.next()) {
                updateReader(readerId, name, phone);
            } else {
                // Insert new reader with the given ID
                addReader(readerId, name, phone);
            }
        }
    }
    
    file.close();
    return true;
}

QSqlDatabase DatabaseManager::getDatabase() const
{
    return m_database;
}