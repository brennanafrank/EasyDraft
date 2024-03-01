#ifndef TEMPLATES_H
#define TEMPLATES_H

#include <QDialog>

namespace Ui {
class Templates;
}

class Templates : public QDialog
{
    Q_OBJECT

public:
    explicit Templates(QWidget *parent = nullptr);
    ~Templates();

private:
    Ui::Templates *ui;
};

#endif // TEMPLATES_H
