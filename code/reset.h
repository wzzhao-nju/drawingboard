#ifndef RESET_H
#define RESET_H

#include <QDialog>
#include <QString>

namespace Ui {
class reset;
}

class reset : public QDialog
{
    Q_OBJECT

public:
    explicit reset(QWidget *parent = nullptr);
    ~reset();
signals:
    void sendInfo(int, int);
private slots:
    void on_buttonBox_rejected();

    void on_buttonBox_accepted();

private:
    Ui::reset *ui;
};

#endif // RESET_H
