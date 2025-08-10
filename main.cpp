#include <QApplication>
#include <QWebEngineView>
#include <QPrinter>
#include <QFile>
#include <QTextStream>
#include <QCommandLineParser>
#include <QDebug>
#include <QRegularExpression>

class MarkdownConverter : public QObject {
    Q_OBJECT
public:
    explicit MarkdownConverter(QObject *parent = nullptr) : QObject(parent) {}

    bool convertToHtml(const QString &inputFile, const QString &outputFile) {
        QString markdown = readFile(inputFile);
        if (markdown.isEmpty()) {
            qWarning() << "Failed to read input file:" << inputFile;
            return false;
        }

        QString html = generateHtml(markdown);
        return writeFile(outputFile, html);
    }

    bool convertToPdf(const QString &inputFile, const QString &outputFile) {
        QString markdown = readFile(inputFile);
        if (markdown.isEmpty()) {
            qWarning() << "Failed to read input file:" << inputFile;
            return false;
        }

        QWebEngineView *view = new QWebEngineView;
        view->setHtml(generateHtml(markdown));

        QPrinter printer(QPrinter::HighResolution);
        printer.setOutputFormat(QPrinter::PdfFormat);
        printer.setOutputFileName(outputFile);
        printer.setPageMargins(QMarginsF(15, 15, 15, 15));

        QEventLoop loop;
        bool conversionSuccess = false;

        QObject::connect(view, &QWebEngineView::loadFinished, [&](bool ok) {
            if (!ok) {
                qWarning() << "Failed to load HTML content";
                loop.quit();
                return;
            }

            view->page()->printToPdf(printer, [&](const QByteArray &pdfData) {
                conversionSuccess = !pdfData.isEmpty();
                if (!conversionSuccess) {
                    qWarning() << "Failed to generate PDF";
                }
                loop.quit();
            });
        });

        loop.exec();
        view->deleteLater();
        return conversionSuccess;
    }

private:
    QString readFile(const QString &filename) {
        QFile file(filename);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            return QString();
        }
        QTextStream in(&file);
        return in.readAll();
    }

    bool writeFile(const QString &filename, const QString &content) {
        QFile file(filename);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            return false;
        }
        QTextStream out(&file);
        out << content;
        return true;
    }

    QString generateHtml(const QString &markdown) {
        // 基本Markdown转换
        QString html = markdown;

        // 标题
        html.replace(QRegularExpression("^# (.*)$", QRegularExpression::MultilineOption), "<h1>\\1</h1>");
        html.replace(QRegularExpression("^## (.*)$", QRegularExpression::MultilineOption), "<h2>\\1</h2>");
        html.replace(QRegularExpression("^### (.*)$", QRegularExpression::MultilineOption), "<h3>\\1</h3>");

        // 粗体和斜体
        html.replace(QRegularExpression("\\*\\*(.*?)\\*\\*"), "<strong>\\1</strong>");
        html.replace(QRegularExpression("\\*(.*?)\\*"), "<em>\\1</em>");

        // 代码
        html.replace(QRegularExpression("`(.*?)`"), "<code>\\1</code>");

        // 代码块
        html.replace(QRegularExpression("```(.*?)```", QRegularExpression::DotMatchesEverythingOption), 
                    "<pre><code>\\1</code></pre>");

        // 链接和图片
        html.replace(QRegularExpression("\\[(.*?)\\]\\((.*?)\\)"), "<a href=\"\\2\">\\1</a>");
        html.replace(QRegularExpression("!\\[(.*?)\\]\\((.*?)\\)"), "<img src=\"\\2\" alt=\"\\1\">");

        // 列表
        html.replace(QRegularExpression("^\\* (.*)$", QRegularExpression::MultilineOption), "<li>\\1</li>");
        html.replace(QRegularExpression("^\\d+\\. (.*)$", QRegularExpression::MultilineOption), "<li>\\1</li>");
        html.replace(QRegularExpression("(<li>.*</li>)", QRegularExpression::DotMatchesEverythingOption), 
                    "<ul>\\1</ul>");

        // 段落
        html.replace(QRegularExpression("\n\n"), "</p><p>");
        html.replace(QRegularExpression("\n"), "<br>");

        // 完整的HTML文档
        return QStringLiteral(
            "<!DOCTYPE html>\n"
            "<html>\n"
            "<head>\n"
            "  <meta charset=\"UTF-8\">\n"
            "  <title>Markdown Conversion</title>\n"
            "  <style>\n"
            "    body { font-family: Arial, sans-serif; line-height: 1.6; max-width: 800px; margin: 0 auto; padding: 20px; }\n"
            "    h1 { color: #2c3e50; border-bottom: 1px solid #eee; padding-bottom: 10px; }\n"
            "    h2 { color: #34495e; }\n"
            "    h3 { color: #7f8c8d; }\n"
            "    code { background: #f8f8f8; padding: 2px 5px; border-radius: 3px; font-family: monospace; }\n"
            "    pre { background: #f8f8f8; padding: 10px; border-radius: 5px; overflow-x: auto; }\n"
            "    pre code { background: none; padding: 0; }\n"
            "    blockquote { border-left: 4px solid #ddd; padding-left: 15px; color: #777; }\n"
            "    ul, ol { padding-left: 20px; }\n"
            "    img { max-width: 100%; height: auto; }\n"
            "  </style>\n"
            "</head>\n"
            "<body>\n"
            "<p>%1</p>\n"
            "</body>\n"
            "</html>").arg(html);
    }
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    QApplication::setApplicationName("MarkdownConverter");
    QApplication::setApplicationVersion("1.0");

    QCommandLineParser parser;
    parser.setApplicationDescription("Markdown to HTML/PDF Converter using Qt 6.4.2");
    parser.addHelpOption();
    parser.addVersionOption();

    parser.addPositionalArgument("input", "Input Markdown file");
    parser.addPositionalArgument("output", "Output file (HTML or PDF)");
    parser.addPositionalArgument("format", "Output format (html or pdf)");

    parser.process(app);

    const QStringList args = parser.positionalArguments();
    if (args.size() != 3) {
        parser.showHelp(1);
    }

    QString inputFile = args.at(0);
    QString outputFile = args.at(1);
    QString format = args.at(2).toLower();

    MarkdownConverter converter;

    bool success = false;
    if (format == "html") {
        success = converter.convertToHtml(inputFile, outputFile);
    } else if (format == "pdf") {
        success = converter.convertToPdf(inputFile, outputFile);
    } else {
        qWarning() << "Invalid format. Use 'html' or 'pdf'";
        return 1;
    }

    return success ? 0 : 1;
}

#include "main.moc"