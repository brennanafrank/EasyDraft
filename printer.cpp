//#include <QPrintDialog>
//#include <QtPdf>
//#include <QPdfDocument>
//#include <QtPrintSupport>

#include "printer.hpp"

void printPdf(const QString& filePath) {
    QPdfDocument pdfDocument = QPdfDocument();

    pdfDocument.load(filePath);

    QPrinter printer = QPrinter(QPrinter::HighResolution);
    printer.setPageSize(QPageSize(QPageSize::A4));
    printer.setOutputFormat(QPrinter::NativeFormat);

    QPainter painter(&printer);

    for (int i = 0; i < pdfDocument.pageCount(); i++) {
        QImage image = pdfDocument.render(i, pdfDocument.pagePointSize(i).toSize());
        painter.drawImage(printer.pageRect(QPrinter::Point), image);
        printer.newPage();
    }

    painter.end();

    // Print dialog
    QPrintDialog printDialog(&printer);
    if (printDialog.exec() == QDialog::Accepted) {
        // Printing accepted, nothing more to do
    }
}
