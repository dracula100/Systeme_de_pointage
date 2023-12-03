#include "admin.h"
#include "ui_admin.h"


admin::admin(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::admin)
{
    ui->setupUi(this);

}


admin::~admin()
{
    delete ui;
}


