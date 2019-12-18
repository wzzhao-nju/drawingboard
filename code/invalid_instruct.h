#ifndef INVALID_INSTRUCT_H
#define INVALID_INSTRUCT_H

#include <QDialog>

namespace Ui {
class invalid_instruct;
}

class invalid_instruct : public QDialog
{
    Q_OBJECT

public:
    explicit invalid_instruct(QWidget *parent = nullptr);
    ~invalid_instruct();

private:
    Ui::invalid_instruct *ui;
};

#endif // INVALID_INSTRUCT_H
