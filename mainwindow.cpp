#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMessageBox>
#include <QSqlQuery>
#include <QFile>
#include <QDebug>
#include <QCloseEvent>
#include <QHeaderView>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , selectedBookId(-1)
    , selectedReaderId(-1)
    , selectedBorrowId(-1)
{
    ui->setupUi(this);
    
    // Load style sheet
    QFile styleFile("style.qss");
    if (styleFile.open(QIODevice::ReadOnly)) {
        QString styleSheet = QLatin1String(styleFile.readAll());
        qApp->setStyleSheet(styleSheet);
        styleFile.close();
    } else {
        qDebug() << "Warning: style.qss file not found. Continuing without custom styles.";
    }
    
    // Initialize managers
    bookManager = new BookManager(this);
    readerManager = new ReaderManager(this);
    borrowManager = new BorrowManager(this);
    
    // Setup UI
    setupUI();
    
    // Set window title
    setWindowTitle("Library Management System");
    
    // Set window properties
    setMinimumSize(800, 600);
    resize(1000, 800);
    
    // Set tab widget properties
    tabWidget->setTabShape(QTabWidget::Rounded);
    tabWidget->setDocumentMode(true);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupUI()
{
    // Create tab widget
    tabWidget = new QTabWidget(this);
    setCentralWidget(tabWidget);
    
    // Setup tabs
    setupBookTab();
    setupReaderTab();
    setupBorrowTab();
    
    // Add tabs to tab widget
    tabWidget->addTab(bookTab, tr("图书管理"));
    tabWidget->addTab(readerTab, tr("读者管理"));
    tabWidget->addTab(borrowTab, tr("借还管理"));
}

void MainWindow::setupBookTab()
{
    bookTab = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(bookTab);
    mainLayout->setContentsMargins(10, 10, 10, 10);
    mainLayout->setSpacing(10);
    
    // Create form group
    QGroupBox *formGroupBox = new QGroupBox(tr("图书信息"), bookTab);
    QGridLayout *formLayout = new QGridLayout(formGroupBox);
    formLayout->setContentsMargins(10, 10, 10, 10);
    formLayout->setSpacing(10);
    formLayout->setColumnStretch(1, 1);
    
    // Book ID
    QLabel *bookIdLabel = new QLabel(tr("图书 ID:"), formGroupBox);
    bookIdLabel->setMinimumWidth(80);
    bookIdLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    bookIdEdit = new QLineEdit(formGroupBox);
    formLayout->addWidget(bookIdLabel, 0, 0);
    formLayout->addWidget(bookIdEdit, 0, 1);
    
    // Book name
    QLabel *nameLabel = new QLabel(tr("图书名称:"), formGroupBox);
    nameLabel->setMinimumWidth(80);
    nameLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    bookNameEdit = new QLineEdit(formGroupBox);
    formLayout->addWidget(nameLabel, 1, 0);
    formLayout->addWidget(bookNameEdit, 1, 1);
    
    // Author
    QLabel *authorLabel = new QLabel(tr("作者:"), formGroupBox);
    authorLabel->setMinimumWidth(80);
    authorLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    authorEdit = new QLineEdit(formGroupBox);
    formLayout->addWidget(authorLabel, 2, 0);
    formLayout->addWidget(authorEdit, 2, 1);
    
    // Category
    QLabel *categoryLabel = new QLabel(tr("分类:"), formGroupBox);
    categoryLabel->setMinimumWidth(80);
    categoryLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    categoryEdit = new QLineEdit(formGroupBox);
    formLayout->addWidget(categoryLabel, 3, 0);
    formLayout->addWidget(categoryEdit, 3, 1);
    
    // Stock
    QLabel *stockLabel = new QLabel(tr("库存:"), formGroupBox);
    stockLabel->setMinimumWidth(80);
    stockLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    stockSpinBox = new QSpinBox(formGroupBox);
    stockSpinBox->setRange(0, 1000);
    stockSpinBox->setFixedWidth(100);
    formLayout->addWidget(stockLabel, 4, 0);
    formLayout->addWidget(stockSpinBox, 4, 1, Qt::AlignLeft);
    
    // Buttons
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->setSpacing(8);
    addBookBtn = new QPushButton(tr("添加图书"), formGroupBox);
    updateBookBtn = new QPushButton(tr("更新图书"), formGroupBox);
    deleteBookBtn = new QPushButton(tr("删除图书"), formGroupBox);
    exportBooksBtn = new QPushButton(tr("导出图书"), formGroupBox);
    importBooksBtn = new QPushButton(tr("导入图书"), formGroupBox);
    
    buttonLayout->addWidget(addBookBtn);
    buttonLayout->addWidget(updateBookBtn);
    buttonLayout->addWidget(deleteBookBtn);
    buttonLayout->addStretch();
    buttonLayout->addWidget(exportBooksBtn);
    buttonLayout->addWidget(importBooksBtn);
    
    formLayout->addLayout(buttonLayout, 5, 0, 1, 2);
    mainLayout->addWidget(formGroupBox);
    
    // Table view
    bookTable = new QTableView(bookTab);
    bookTable->setAlternatingRowColors(true);
    bookTable->horizontalHeader()->setStretchLastSection(true);
    refreshBookTable();
    mainLayout->addWidget(bookTable);
    
    // Connect signals
    connect(addBookBtn, &QPushButton::clicked, this, &MainWindow::on_addBook_clicked);
    connect(updateBookBtn, &QPushButton::clicked, this, &MainWindow::on_updateBook_clicked);
    connect(deleteBookBtn, &QPushButton::clicked, this, &MainWindow::on_deleteBook_clicked);
    connect(exportBooksBtn, &QPushButton::clicked, this, &MainWindow::on_exportBooks_clicked);
    connect(importBooksBtn, &QPushButton::clicked, this, &MainWindow::on_importBooks_clicked);
    connect(bookTable, &QTableView::clicked, this, &MainWindow::on_bookTable_clicked);
}

void MainWindow::setupReaderTab()
{
    readerTab = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(readerTab);
    mainLayout->setContentsMargins(10, 10, 10, 10);
    mainLayout->setSpacing(10);
    
    // Create form group
    QGroupBox *formGroupBox = new QGroupBox(tr("读者信息"), readerTab);
    QGridLayout *formLayout = new QGridLayout(formGroupBox);
    formLayout->setContentsMargins(10, 10, 10, 10);
    formLayout->setSpacing(10);
    formLayout->setColumnStretch(1, 1);
    
    // Reader ID
    QLabel *readerIdLabel = new QLabel(tr("读者 ID:"), formGroupBox);
    readerIdLabel->setMinimumWidth(80);
    readerIdLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    readerIdEdit = new QLineEdit(formGroupBox);
    formLayout->addWidget(readerIdLabel, 0, 0);
    formLayout->addWidget(readerIdEdit, 0, 1);
    
    // Name
    QLabel *nameLabel = new QLabel(tr("姓名:"), formGroupBox);
    nameLabel->setMinimumWidth(80);
    nameLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    readerNameEdit = new QLineEdit(formGroupBox);
    formLayout->addWidget(nameLabel, 1, 0);
    formLayout->addWidget(readerNameEdit, 1, 1);
    
    // Phone
    QLabel *phoneLabel = new QLabel(tr("电话:"), formGroupBox);
    phoneLabel->setMinimumWidth(80);
    phoneLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    phoneEdit = new QLineEdit(formGroupBox);
    formLayout->addWidget(phoneLabel, 2, 0);
    formLayout->addWidget(phoneEdit, 2, 1);
    
    // Buttons
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->setSpacing(8);
    addReaderBtn = new QPushButton(tr("添加读者"), formGroupBox);
    updateReaderBtn = new QPushButton(tr("更新读者"), formGroupBox);
    deleteReaderBtn = new QPushButton(tr("删除读者"), formGroupBox);
    exportReadersBtn = new QPushButton(tr("导出读者"), formGroupBox);
    importReadersBtn = new QPushButton(tr("导入读者"), formGroupBox);
    
    buttonLayout->addWidget(addReaderBtn);
    buttonLayout->addWidget(updateReaderBtn);
    buttonLayout->addWidget(deleteReaderBtn);
    buttonLayout->addStretch();
    buttonLayout->addWidget(exportReadersBtn);
    buttonLayout->addWidget(importReadersBtn);
    
    formLayout->addLayout(buttonLayout, 3, 0, 1, 2);
    mainLayout->addWidget(formGroupBox);
    
    // Table view
    readerTable = new QTableView(readerTab);
    readerTable->setAlternatingRowColors(true);
    readerTable->horizontalHeader()->setStretchLastSection(true);
    refreshReaderTable();
    mainLayout->addWidget(readerTable);
    
    // Connect signals
    connect(addReaderBtn, &QPushButton::clicked, this, &MainWindow::on_addReader_clicked);
    connect(updateReaderBtn, &QPushButton::clicked, this, &MainWindow::on_updateReader_clicked);
    connect(deleteReaderBtn, &QPushButton::clicked, this, &MainWindow::on_deleteReader_clicked);
    connect(exportReadersBtn, &QPushButton::clicked, this, &MainWindow::on_exportReaders_clicked);
    connect(importReadersBtn, &QPushButton::clicked, this, &MainWindow::on_importReaders_clicked);
    connect(readerTable, &QTableView::clicked, this, &MainWindow::on_readerTable_clicked);
}

void MainWindow::setupBorrowTab()
{
    borrowTab = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(borrowTab);
    mainLayout->setContentsMargins(10, 10, 10, 10);
    mainLayout->setSpacing(10);
    
    // Create form group
    QGroupBox *formGroupBox = new QGroupBox(tr("借还管理"), borrowTab);
    QGridLayout *formLayout = new QGridLayout(formGroupBox);
    formLayout->setContentsMargins(10, 10, 10, 10);
    formLayout->setSpacing(10);
    formLayout->setColumnStretch(1, 1);
    
    // Book selection
    QLabel *bookLabel = new QLabel(tr("图书:"), formGroupBox);
    bookLabel->setMinimumWidth(80);
    bookLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    bookComboBox = new QComboBox(formGroupBox);
    bookComboBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    refreshBookComboBox();
    formLayout->addWidget(bookLabel, 0, 0);
    formLayout->addWidget(bookComboBox, 0, 1);
    
    // Reader selection
    QLabel *readerLabel = new QLabel(tr("读者:"), formGroupBox);
    readerLabel->setMinimumWidth(80);
    readerLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    readerComboBox = new QComboBox(formGroupBox);
    readerComboBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    refreshReaderComboBox();
    formLayout->addWidget(readerLabel, 1, 0);
    formLayout->addWidget(readerComboBox, 1, 1);
    
    // Borrow date
    QLabel *borrowDateLabel = new QLabel(tr("借阅日期:"), formGroupBox);
    borrowDateLabel->setMinimumWidth(80);
    borrowDateLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    borrowDateEdit = new QDateEdit(QDate::currentDate(), formGroupBox);
    borrowDateEdit->setCalendarPopup(true);
    borrowDateEdit->setFixedWidth(150);
    formLayout->addWidget(borrowDateLabel, 2, 0);
    formLayout->addWidget(borrowDateEdit, 2, 1, Qt::AlignLeft);
    
    // Due date
    QLabel *dueDateLabel = new QLabel(tr("到期日期:"), formGroupBox);
    dueDateLabel->setMinimumWidth(80);
    dueDateLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    dueDateEdit = new QDateEdit(QDate::currentDate().addDays(30), formGroupBox);
    dueDateEdit->setCalendarPopup(true);
    dueDateEdit->setFixedWidth(150);
    formLayout->addWidget(dueDateLabel, 3, 0);
    formLayout->addWidget(dueDateEdit, 3, 1, Qt::AlignLeft);
    
    // Return date
    QLabel *returnDateLabel = new QLabel(tr("归还日期:"), formGroupBox);
    returnDateLabel->setMinimumWidth(80);
    returnDateLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    returnDateEdit = new QDateEdit(QDate::currentDate(), formGroupBox);
    returnDateEdit->setCalendarPopup(true);
    returnDateEdit->setFixedWidth(150);
    formLayout->addWidget(returnDateLabel, 4, 0);
    formLayout->addWidget(returnDateEdit, 4, 1, Qt::AlignLeft);
    
    // Buttons
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->setSpacing(8);
    borrowBookBtn = new QPushButton(tr("借阅图书"), formGroupBox);
    returnBookBtn = new QPushButton(tr("归还图书"), formGroupBox);
    exportBorrowsBtn = new QPushButton(tr("导出借还记录"), formGroupBox);
    
    buttonLayout->addWidget(borrowBookBtn);
    buttonLayout->addWidget(returnBookBtn);
    buttonLayout->addStretch();
    buttonLayout->addWidget(exportBorrowsBtn);
    
    formLayout->addLayout(buttonLayout, 5, 0, 1, 2);
    mainLayout->addWidget(formGroupBox);
    
    // Table view
    borrowTable = new QTableView(borrowTab);
    borrowTable->setAlternatingRowColors(true);
    borrowTable->horizontalHeader()->setStretchLastSection(true);
    refreshBorrowTable();
    mainLayout->addWidget(borrowTable);
    
    // Connect signals
    connect(borrowBookBtn, &QPushButton::clicked, this, &MainWindow::on_borrowBook_clicked);
    connect(returnBookBtn, &QPushButton::clicked, this, &MainWindow::on_returnBook_clicked);
    connect(exportBorrowsBtn, &QPushButton::clicked, this, &MainWindow::on_exportBorrows_clicked);
    connect(borrowTable, &QTableView::clicked, this, &MainWindow::on_borrowTable_clicked);
}

void MainWindow::on_addBook_clicked()
{
    QString bookId = bookIdEdit->text();
    QString bookName = bookNameEdit->text();
    QString author = authorEdit->text();
    QString category = categoryEdit->text();
    int stock = stockSpinBox->value();
    
    if (bookId.isEmpty() || bookName.isEmpty() || author.isEmpty() || category.isEmpty()) {
        QMessageBox::warning(this, tr("警告"), tr("请填写所有字段！"));
        return;
    }
    
    if (bookManager->addBook(bookId, bookName, author, category, stock)) {
        QMessageBox::information(this, tr("成功"), tr("图书添加成功！"));
        refreshBookTable();
        
        // Clear form
        bookIdEdit->clear();
        bookNameEdit->clear();
        authorEdit->clear();
        categoryEdit->clear();
        stockSpinBox->setValue(0);
    } else {
        QMessageBox::critical(this, tr("错误"), tr("图书添加失败！"));
    }
}

void MainWindow::on_updateBook_clicked()
{
    if (selectedBookId == -1) {
        QMessageBox::warning(this, tr("警告"), tr("请选择要更新的图书！"));
        return;
    }
    
    QString bookId = bookIdEdit->text();
    QString bookName = bookNameEdit->text();
    QString author = authorEdit->text();
    QString category = categoryEdit->text();
    int stock = stockSpinBox->value();
    
    if (bookId.isEmpty() || bookName.isEmpty() || author.isEmpty() || category.isEmpty()) {
        QMessageBox::warning(this, tr("警告"), tr("请填写所有字段！"));
        return;
    }
    
    if (bookManager->updateBook(bookId, bookName, author, category, stock)) {
        QMessageBox::information(this, tr("成功"), tr("图书更新成功！"));
        refreshBookTable();
        selectedBookId = -1;
        
        // Clear form
        bookIdEdit->clear();
        bookNameEdit->clear();
        authorEdit->clear();
        categoryEdit->clear();
        stockSpinBox->setValue(0);
    } else {
        QMessageBox::critical(this, tr("错误"), tr("图书更新失败！"));
    }
}

void MainWindow::on_deleteBook_clicked()
{
    if (selectedBookId == -1) {
        QMessageBox::warning(this, tr("警告"), tr("请选择要删除的图书！"));
        return;
    }
    
    if (QMessageBox::question(this, tr("确认"), tr("您确定要删除这本图书吗？"),
                              QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
        
        if (bookManager->deleteBook(bookIdEdit->text())) {
            QMessageBox::information(this, tr("成功"), tr("图书删除成功！"));
            refreshBookTable();
            refreshBookComboBox();
            selectedBookId = -1;
            
            // Clear form
            bookIdEdit->clear();
            bookNameEdit->clear();
            authorEdit->clear();
            categoryEdit->clear();
            stockSpinBox->setValue(0);
        } else {
            QMessageBox::critical(this, tr("错误"), tr("图书删除失败！"));
        }
    }
}

void MainWindow::on_exportBooks_clicked()
{
    QString filePath = QFileDialog::getSaveFileName(this, tr("导出图书"), QDir::homePath(), tr("CSV 文件 (*.csv)"));
    
    if (!filePath.isEmpty()) {
        if (bookManager->exportToCSV(filePath)) {
            // 成功信息由exportToCSV内部显示
        } else {
            QMessageBox::critical(this, tr("错误"), tr("图书导出失败！"));
        }
    }
}

void MainWindow::on_importBooks_clicked()
{
    QString filePath = QFileDialog::getOpenFileName(this, tr("导入图书"), QDir::homePath(), tr("CSV 文件 (*.csv)"));
    
    if (!filePath.isEmpty()) {
        if (bookManager->importFromCSV(filePath)) {
            // 成功信息由importFromCSV内部显示
            refreshBookTable();
            refreshBookComboBox();
        } else {
            QMessageBox::critical(this, tr("错误"), tr("图书导入失败！"));
        }
    }
}

void MainWindow::on_bookTable_clicked(const QModelIndex &index)
{
    if (index.isValid()) {
        selectedBookId = index.sibling(index.row(), 0).data().toInt();
        QString bookId = index.sibling(index.row(), 0).data().toString();
        QString bookName = index.sibling(index.row(), 1).data().toString();
        QString author = index.sibling(index.row(), 2).data().toString();
        QString category = index.sibling(index.row(), 3).data().toString();
        int stock = index.sibling(index.row(), 4).data().toInt();
        
        // Fill form
        bookIdEdit->setText(bookId);
        bookNameEdit->setText(bookName);
        authorEdit->setText(author);
        categoryEdit->setText(category);
        stockSpinBox->setValue(stock);
    }
}

void MainWindow::on_addReader_clicked()
{
    QString readerId = readerIdEdit->text();
    QString name = readerNameEdit->text();
    QString phone = phoneEdit->text();
    
    if (readerId.isEmpty() || name.isEmpty() || phone.isEmpty()) {
        QMessageBox::warning(this, tr("Warning"), tr("Please fill in all fields!"));
        return;
    }
    
    if (readerManager->addReader(readerId, name, phone)) {
        QMessageBox::information(this, tr("Success"), tr("Reader added successfully!"));
        refreshReaderTable();
        refreshReaderComboBox();
        
        // Clear form
        readerIdEdit->clear();
        readerNameEdit->clear();
        phoneEdit->clear();
    } else {
        QMessageBox::critical(this, tr("Error"), tr("Failed to add reader!"));
    }
}

void MainWindow::on_updateReader_clicked()
{
    if (selectedReaderId == -1) {
        QMessageBox::warning(this, tr("警告"), tr("请选择要更新的读者！"));
        return;
    }
    
    QString name = readerNameEdit->text();
    QString phone = phoneEdit->text();
    
    if (name.isEmpty() || phone.isEmpty()) {
        QMessageBox::warning(this, tr("警告"), tr("请填写所有字段！"));
        return;
    }
    
    if (readerManager->updateReader(QString::number(selectedReaderId), name, phone)) {
        QMessageBox::information(this, tr("成功"), tr("读者更新成功！"));
        refreshReaderTable();
        refreshReaderComboBox();
        selectedReaderId = -1;
        
        // Clear form
        readerIdEdit->clear();
        readerNameEdit->clear();
        phoneEdit->clear();
    } else {
        QMessageBox::critical(this, tr("错误"), tr("读者更新失败！"));
    }
}

void MainWindow::on_deleteReader_clicked()
{
    if (selectedReaderId == -1) {
        QMessageBox::warning(this, tr("Warning"), tr("Please select a reader to delete!"));
        return;
    }
    
    if (QMessageBox::question(this, tr("Confirmation"), tr("Are you sure you want to delete this reader?"),
                              QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
        
        if (readerManager->deleteReader(QString::number(selectedReaderId))) {
            QMessageBox::information(this, tr("Success"), tr("Reader deleted successfully!"));
            refreshReaderTable();
            refreshReaderComboBox();
            selectedReaderId = -1;
            
            // Clear form
            readerIdEdit->clear();
            readerNameEdit->clear();
            phoneEdit->clear();
        } else {
            QMessageBox::critical(this, tr("Error"), tr("Failed to delete reader!"));
        }
    }
}

void MainWindow::on_exportReaders_clicked()
{
    QString filePath = QFileDialog::getSaveFileName(this, tr("导出读者"), QDir::homePath(), tr("CSV 文件 (*.csv)"));
    
    if (!filePath.isEmpty()) {
        if (readerManager->exportToCSV(filePath)) {
            // 成功信息由exportToCSV内部显示
        } else {
            QMessageBox::critical(this, tr("错误"), tr("读者导出失败！"));
        }
    }
}

void MainWindow::on_importReaders_clicked()
{
    QString filePath = QFileDialog::getOpenFileName(this, tr("导入读者"), QDir::homePath(), tr("CSV 文件 (*.csv)"));
    
    if (!filePath.isEmpty()) {
        if (readerManager->importFromCSV(filePath)) {
            // 成功信息由importFromCSV内部显示
            refreshReaderTable();
            refreshReaderComboBox();
        } else {
            QMessageBox::critical(this, tr("错误"), tr("读者导入失败！"));
        }
    }
}

void MainWindow::on_readerTable_clicked(const QModelIndex &index)
{
    if (index.isValid()) {
        selectedReaderId = index.sibling(index.row(), 0).data().toInt();
        QString readerId = index.sibling(index.row(), 0).data().toString();
        QString name = index.sibling(index.row(), 1).data().toString();
        QString phone = index.sibling(index.row(), 2).data().toString();
        
        // Fill form
        readerIdEdit->setText(readerId);
        readerNameEdit->setText(name);
        phoneEdit->setText(phone);
    }
}

void MainWindow::on_borrowBook_clicked()
{
    int bookIndex = bookComboBox->currentIndex();
    int readerIndex = readerComboBox->currentIndex();
    
    if (bookIndex == -1 || readerIndex == -1) {
        QMessageBox::warning(this, tr("Warning"), tr("Please select a book and a reader!"));
        return;
    }
    
    QString bookId = QString::number(bookComboBox->itemData(bookIndex).toInt());
    QString readerId = QString::number(readerComboBox->itemData(readerIndex).toInt());
    QDate borrowDate = borrowDateEdit->date();
    QDate dueDate = dueDateEdit->date();
    
    if (dueDate <= borrowDate) {
        QMessageBox::warning(this, tr("Warning"), tr("Due date must be after borrow date!"));
        return;
    }
    
    if (borrowManager->borrowBook(bookId, readerId, borrowDate, dueDate)) {
        QMessageBox::information(this, tr("Success"), tr("Book borrowed successfully!"));
        refreshBorrowTable();
        refreshBookTable(); // Update book stock
        refreshBookComboBox(); // Update book combobox
    } else {
        QMessageBox::critical(this, tr("Error"), tr("Failed to borrow book!"));
    }
}

void MainWindow::on_returnBook_clicked()
{
    if (selectedBorrowId == -1) {
        QMessageBox::warning(this, tr("Warning"), tr("Please select a borrow record to return!"));
        return;
    }
    
    QDateTime returnDate = returnDateEdit->dateTime();
    
    if (borrowManager->returnBook(selectedBorrowId, returnDate.date())) {
        QMessageBox::information(this, tr("Success"), tr("Book returned successfully!"));
        refreshBorrowTable();
        selectedBorrowId = -1;
    } else {
        QMessageBox::critical(this, tr("Error"), tr("Failed to return book!"));
    }
}

void MainWindow::on_exportBorrows_clicked()
{
    QString filePath = QFileDialog::getSaveFileName(this, tr("导出借还记录"), QDir::homePath(), tr("CSV 文件 (*.csv)"));
    
    if (!filePath.isEmpty()) {
        if (borrowManager->exportToCSV(filePath)) {
            // 成功信息由exportToCSV内部显示
        } else {
            QMessageBox::critical(this, tr("错误"), tr("借还记录导出失败！"));
        }
    }
}
    // QString filePath = QFileDialog::getSaveFileName(this, tr("Export Borrows"), QDir::homePath(), tr("CSV Files (*.csv)"));
    
    // if (!filePath.isEmpty()) {
    //     if (borrowManager->exportBorrows(filePath)) {
    //         QMessageBox::information(this, tr("Success"), tr("Borrows exported successfully!"));
    //     } else {
    //         QMessageBox::critical(this, tr("Error"), tr("Failed to export borrows!"));
    //     }
    // }


void MainWindow::on_borrowTable_clicked(const QModelIndex &index)
{
    if (index.isValid()) {
        selectedBorrowId = index.sibling(index.row(), 0).data().toInt();
    }
}

void MainWindow::refreshBookTable()
{
    QSqlQueryModel *model = bookManager->getBooks();
    bookTable->setModel(model);
    bookTable->resizeColumnsToContents();
}

void MainWindow::refreshReaderTable()
{
    QSqlQueryModel *model = readerManager->getReaders();
    readerTable->setModel(model);
    readerTable->resizeColumnsToContents();
}

void MainWindow::refreshBorrowTable()
{
    QSqlQueryModel *model = borrowManager->getAllBorrows();
    borrowTable->setModel(model);
    borrowTable->resizeColumnsToContents();
}

void MainWindow::refreshBookComboBox()
{
    bookComboBox->clear();
    
    QSqlQueryModel *model = bookManager->getBooks();
    for (int i = 0; i < model->rowCount(); ++i) {
        int bookId = model->index(i, 0).data().toInt();
        QString bookName = model->index(i, 1).data().toString();
        bookComboBox->addItem(bookName, bookId);
    }
    
    delete model;
}

void MainWindow::refreshReaderComboBox()
{
    QApplication::setOverrideCursor(Qt::WaitCursor);
    readerComboBox->clear();
    
    QSqlQueryModel *model = readerManager->getReaders();
    for (int i = 0; i < model->rowCount(); ++i) {
        int readerId = model->index(i, 0).data().toInt();
        QString readerName = model->index(i, 1).data().toString();
        readerComboBox->addItem(readerName, readerId);
    }
    
    delete model;
    QApplication::restoreOverrideCursor();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, tr("确认退出"), tr("您确定要退出图书管理系统吗？"),
                                  QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
    
    if (reply == QMessageBox::Yes) {
        event->accept();
    } else {
        event->ignore();
    }
}
