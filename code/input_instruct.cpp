#include "input_instruct.h"
#include "ui_input_instruct.h"

input_instruct::input_instruct(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::input_instruct)
{
    ui->setupUi(this);
}

input_instruct::~input_instruct()
{
    delete ui;
}

void input_instruct::on_pushButton_clicked()
{
    QString text=ui->textEdit->toPlainText();
    QStringList ins = text.split('\n');
    this->close();
    emit sendInstruct(ins);
}
