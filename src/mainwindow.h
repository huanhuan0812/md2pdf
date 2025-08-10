#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QMessageBox>
#include <QTextStream>
#include <QPrinter>
#include <QTextDocument>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_actionOpen_triggered();
    void on_actionSave_HTML_triggered();
    void on_actionExport_PDF_triggered();
    void on_textEdit_markdown_textChanged();
    void updatePreview();

private:
    Ui::MainWindow *ui;
    QString currentFilePath;
    QString markdownToHtml(const QString &markdown);
    void saveAsHtml(const QString &html);
    void exportToPdf(const QString &html);
};

#endif // MAINWINDOW_H