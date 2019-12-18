#include "reset.h"
#include "ui_reset.h"

reset::reset(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::reset)
{
    ui->setupUi(this);
}

reset::~reset()
{
    delete ui;
}

void reset::on_buttonBox_rejected()
{
    this->close();
}

void reset::on_buttonBox_accepted()
{
    int w = ui->width->text().toInt();
    int h = ui->height->text().toInt();
    emit sendInfo(w, h);
}
