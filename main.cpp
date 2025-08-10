#include <QApplication>
#include <QWebEngineView>
#include <QPrinter>
#include <QFile>
#include <QTextStream>
#include <QTimer>
#include <QDebug>

class MarkdownConverter : public QObject {
    Q_OBJECT
public:
    explicit MarkdownConverter(QObject *parent = nullptr) : QObject(parent) {}

    void convertToHtml(const QString &markdown, const QString &outputFile) {
        // 简单的Markdown到HTML转换（实际项目中可以使用更完整的解析器）
        QString html = "<!DOCTYPE html><html><head><meta charset='UTF-8'>"
                      "<style>body { font-family: Arial; max-width: 800px; margin: 0 auto; }"
                      "pre { background: #f4f4f4; padding: 10px; border-radius: 5px; }"
                      "code { background: #f4f4f4; padding: 2px 4px; border-radius: 3px; }"
                      "blockquote { border-left: 4px solid #ddd; padding-left: 15px; color: #777; }"
                      "</style></head><body>" + 
                      markdownToHtml(markdown) + 
                      "</body></html>";

        QFile file(outputFile);
        if (file.open(QIODevice::WriteOnly)) {
            QTextStream stream(&file);
            stream << html;
            file.close();
            qDebug() << "HTML saved to" << outputFile;
        } else {
            qWarning() << "Failed to save HTML file";
        }
    }

    void convertToPdf(const QString &markdown, const QString &outputFile) {
        QString html = markdownToHtml(markdown);
        
        QWebEngineView *view = new QWebEngineView;
        view->setHtml(html);
        
        QPrinter printer(QPrinter::HighResolution);
        printer.setOutputFormat(QPrinter::PdfFormat);
        printer.setOutputFileName(outputFile);
        printer.setPageMargins(QMarginsF(15, 15, 15, 15));
        
        // 等待页面加载完成
        QObject::connect(view, &QWebEngineView::loadFinished, [view, &printer, this](bool ok) {
            if (!ok) {
                qWarning() << "Failed to load HTML content";
                QApplication::exit(1);
                return;
            }
            
            // 打印到PDF
            view->page()->printToPdf(printer, [this](const QByteArray &) {
                qDebug() << "PDF conversion completed";
                QApplication::quit();
            });
        });
    }

private:
    QString markdownToHtml(const QString &markdown) {
        // 简单的Markdown转换（实际项目中应该使用更完整的解析器）
        QString html = markdown;
        
        // 替换基本的Markdown语法
        html.replace("### ", "<h3>").replace("\n###", "</h3>\n");
        html.replace("## ", "<h2>").replace("\n##", "</h2>\n");
        html.replace("# ", "<h1>").replace("\n#", "</h1>\n");
        
        html.replace("**", "<strong>").replace("**", "</strong>");
        html.replace("*", "<em>").replace("*", "</em>");
        html.replace("`", "<code>").replace("`", "</code>");
        
        html.replace("\n- ", "\n<ul><li>").replace("\n-", "</li></ul>\n");
        html.replace("\n* ", "\n<ul><li>").replace("\n*", "</li></ul>\n");
        html.replace("\n1. ", "\n<ol><li>").replace("\n1.", "</li></ol>\n");
        
        html.replace("\n\n", "<p>");
        html.replace("\n", "<br>");
        
        // 处理代码块
        html.replace("```", "<pre><code>").replace("```", "</code></pre>");
        
        return html;
    }
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    if (argc < 4) {
        qDebug() << "Usage:" << argv[0] << "<input.md> <output.html/pdf> <format:html|pdf>";
        return 1;
    }
    
    QString inputFile = argv[1];
    QString outputFile = argv[2];
    QString format = argv[3];
    
    QFile file(inputFile);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Failed to open input file";
        return 1;
    }
    
    QTextStream in(&file);
    QString markdown = in.readAll();
    file.close();
    
    MarkdownConverter converter;
    
    if (format.toLower() == "html") {
        converter.convertToHtml(markdown, outputFile);
        return 0;
    } else if (format.toLower() == "pdf") {
        converter.convertToPdf(markdown, outputFile);
        return app.exec();
    } else {
        qWarning() << "Invalid format. Use 'html' or 'pdf'";
        return 1;
    }
}

#include "main.moc"