#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTabWidget>
#include <QTableView>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QComboBox>
#include <QSpinBox>
#include <QDateEdit>
#include <QGroupBox>
#include <QLabel>
#include <QFileDialog>
#include <QCloseEvent>

#include "bookmanager.h"
#include "readermanager.h"
#include "borrowmanager.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    // Book management slots
    void on_addBook_clicked();
    void on_updateBook_clicked();
    void on_deleteBook_clicked();
    void on_exportBooks_clicked();
    void on_importBooks_clicked();
    void on_bookTable_clicked(const QModelIndex &index);
    
    // Reader management slots
    void on_addReader_clicked();
    void on_updateReader_clicked();
    void on_deleteReader_clicked();
    void on_exportReaders_clicked();
    void on_importReaders_clicked();
    void on_readerTable_clicked(const QModelIndex &index);
    
    // Borrow management slots
    void on_borrowBook_clicked();
    void on_returnBook_clicked();
    void on_exportBorrows_clicked();
    void on_borrowTable_clicked(const QModelIndex &index);

private:
    Ui::MainWindow *ui;
    
    // Tab widget
    QTabWidget *tabWidget;
    
    // Book management components
    QWidget *bookTab;
    QTableView *bookTable;
    QPushButton *addBookBtn;
    QPushButton *updateBookBtn;
    QPushButton *deleteBookBtn;
    QPushButton *exportBooksBtn;
    QPushButton *importBooksBtn;
    QLineEdit *bookIdEdit;
    QLineEdit *bookNameEdit;
    QLineEdit *authorEdit;
    QLineEdit *categoryEdit;
    QSpinBox *stockSpinBox;
    int selectedBookId;
    
    // Reader management components
    QWidget *readerTab;
    QTableView *readerTable;
    QPushButton *addReaderBtn;
    QPushButton *updateReaderBtn;
    QPushButton *deleteReaderBtn;
    QPushButton *exportReadersBtn;
    QPushButton *importReadersBtn;
    QLineEdit *readerIdEdit;
    QLineEdit *readerNameEdit;
    QLineEdit *phoneEdit;
    int selectedReaderId;
    
    // Borrow management components
    QWidget *borrowTab;
    QTableView *borrowTable;
    QPushButton *borrowBookBtn;
    QPushButton *returnBookBtn;
    QPushButton *exportBorrowsBtn;
    QComboBox *bookComboBox;
    QComboBox *readerComboBox;
    QDateEdit *borrowDateEdit;
    QDateEdit *dueDateEdit;
    QDateEdit *returnDateEdit;
    int selectedBorrowId;
    
    // Managers
    BookManager *bookManager;
    ReaderManager *readerManager;
    BorrowManager *borrowManager;
    
    // Helper methods
    void setupUI();
    void setupBookTab();
    void setupReaderTab();
    void setupBorrowTab();
    void refreshBookTable();
    void refreshReaderTable();
    void refreshBorrowTable();
    void refreshBookComboBox();
    void refreshReaderComboBox();
};
#endif // MAINWINDOW_H
