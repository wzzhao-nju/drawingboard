#ifndef INPUT_INSTRUCT_H
#define INPUT_INSTRUCT_H

#include <QDialog>
#include <QString>

namespace Ui {
class input_instruct;
}

class input_instruct : public QDialog
{
    Q_OBJECT

public:
    explicit input_instruct(QWidget *parent = nullptr);
    ~input_instruct();
signals:
    void sendInstruct(QStringList);
private slots:
    void on_pushButton_clicked();

private:
    Ui::input_instruct *ui;
};

#endif // INPUT_INSTRUCT_H
