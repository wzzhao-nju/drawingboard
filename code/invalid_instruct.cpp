#include "invalid_instruct.h"
#include "ui_invalid_instruct.h"

invalid_instruct::invalid_instruct(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::invalid_instruct)
{
    ui->setupUi(this);
}

invalid_instruct::~invalid_instruct()
{
    delete ui;
}
