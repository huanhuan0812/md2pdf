#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("Markdown Converter");
    
    // 初始化UI
    ui->splitter->setSizes({300, 300});
    ui->textEdit_markdown->setPlaceholderText("Write your Markdown here...");
    
    // 连接文本变化信号
    connect(ui->textEdit_markdown, &QTextEdit::textChanged, this, &MainWindow::updatePreview);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionOpen_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Open Markdown File", "", "Markdown Files (*.md *.markdown);;All Files (*)");
    if (fileName.isEmpty()) return;
    
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "Error", "Could not open file");
        return;
    }
    
    QTextStream in(&file);
    ui->textEdit_markdown->setPlainText(in.readAll());
    file.close();
    
    currentFilePath = fileName;
    setWindowTitle(QString("%1 - Markdown Converter").arg(QFileInfo(fileName).fileName()));
}

void MainWindow::on_actionSave_HTML_triggered()
{
    QString html = markdownToHtml(ui->textEdit_markdown->toPlainText());
    saveAsHtml(html);
}

void MainWindow::on_actionExport_PDF_triggered()
{
    QString html = markdownToHtml(ui->textEdit_markdown->toPlainText());
    exportToPdf(html);
}

void MainWindow::updatePreview()
{
    QString html = markdownToHtml(ui->textEdit_markdown->toPlainText());
    ui->textBrowser_preview->setHtml(html);
}

QString MainWindow::markdownToHtml(const QString &markdown)
{
    // 简单的Markdown到HTML转换
    QString html = markdown;
    
    // 标题
    html.replace(QRegularExpression("^#\\s+(.*)$", QRegularExpression::MultilineOption), "<h1>\\1</h1>");
    html.replace(QRegularExpression("^##\\s+(.*)$", QRegularExpression::MultilineOption), "<h2>\\1</h2>");
    html.replace(QRegularExpression("^###\\s+(.*)$", QRegularExpression::MultilineOption), "<h3>\\1</h3>");
    
    // 粗体和斜体
    html.replace(QRegularExpression("\\*\\*(.*?)\\*\\*"), "<strong>\\1</strong>");
    html.replace(QRegularExpression("\\*(.*?)\\*"), "<em>\\1</em>");
    
    // 链接
    html.replace(QRegularExpression("\\[(.*?)\\]\\((.*?)\\)"), "<a href=\"\\2\">\\1</a>");
    
    // 无序列表
    html.replace(QRegularExpression("^\\*\\s+(.*)$", QRegularExpression::MultilineOption), "<li>\\1</li>");
    html.replace(QRegularExpression("(<li>.*</li>)", QRegularExpression::DotMatchesEverythingOption), "<ul>\\1</ul>");
    
    // 代码块
    html.replace(QRegularExpression("```(.*?)```", QRegularExpression::DotMatchesEverythingOption), "<pre><code>\\1</code></pre>");
    
    // 行内代码
    html.replace(QRegularExpression("`(.*?)`"), "<code>\\1</code>");
    
    // 段落
    html.replace(QRegularExpression("\n\n+"), "</p><p>");
    html = "<p>" + html + "</p>";
    
    // 完整HTML文档
    return QString("<!DOCTYPE html><html><head><meta charset=\"UTF-8\"><style>"
                  "body { font-family: Arial, sans-serif; line-height: 1.6; margin: 20px; }"
                  "code { background: #f4f4f4; padding: 2px 5px; border-radius: 3px; }"
                  "pre { background: #f4f4f4; padding: 10px; border-radius: 5px; overflow-x: auto; }"
                  "</style></head><body>%1</body></html>").arg(html);
}

void MainWindow::saveAsHtml(const QString &html)
{
    QString fileName = QFileDialog::getSaveFileName(this, "Save HTML File", "", "HTML Files (*.html *.htm);;All Files (*)");
    if (fileName.isEmpty()) return;
    
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "Error", "Could not save file");
        return;
    }
    
    QTextStream out(&file);
    out << html;
    file.close();
}

void MainWindow::exportToPdf(const QString &html)
{
    QString fileName = QFileDialog::getSaveFileName(this, "Export PDF", "", "PDF Files (*.pdf);;All Files (*)");
    if (fileName.isEmpty()) return;
    
    QPrinter printer(QPrinter::HighResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(fileName);
    
    QTextDocument document;
    document.setHtml(html);
    document.print(&printer);
}