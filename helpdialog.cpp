#include "helpdialog.h"

HelpDialog::HelpDialog(QWidget *parent) : QDialog(parent) {
    setWindowTitle("Keyboard Shortcuts");
    QVBoxLayout *layout = new QVBoxLayout(this);

    QTextEdit *textEdit = new QTextEdit(this);
    textEdit->setReadOnly(true);
    textEdit->setHtml(
        "<style>"
        "ul { list-style-type: none; padding: 0; margin: 0; }"
        "li { margin: 5px; }"
        "h3 { color: #333366; }"
        "</style>"
        "<h3>Keyboard Shortcuts</h3>"
        "<ul>"
        "<li><b>Esc:</b> Back</li>"
        "<li><b>Ctrl + ]:</b> Next placeholder page</li>"
        "<li><b>Ctrl + [:</b> Previous placeholder page</li>"
        "<li><b>Ctrl + Shift + M:</b> Open context menu</li>"
        "<li><b>Ctrl + D:</b> Delete item</li>"
        "<li><b>Ctrl + N:</b> Create folder</li>"
        "<li><b>Ctrl + E:</b> Export</li>"
        "<li><b>Ctrl + S:</b> Save current draft</li>"
        "<li><b>Ctrl + T:</b> Add tag</li>"
        "<li><b>Ctrl + Shift + T:</b> Remove tag</li>"
        "<li><b>Ctrl + A:</b> Auto-save on/off</li>"
        "<li><b>Ctrl + Up:</b> Focus Recent</li>"
        "<li><b>Ctrl + Down:</b> Focus Templates</li>"
        "<li><b>Ctrl + J:</b> Fill from JSON</li>"
        "</ul>"
        );
    layout->addWidget(textEdit);

    QPushButton *closeButton = new QPushButton("Close", this);
    closeButton->setStyleSheet("background-color: #333366; color: white; font-weight: bold; padding: 5px;");
    connect(closeButton, &QPushButton::clicked, this, &HelpDialog::accept);
    layout->addWidget(closeButton);

    setLayout(layout);
    setFixedSize(400, 400);
}
