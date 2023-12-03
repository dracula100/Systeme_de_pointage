#ifndef ADMIN_H
#define ADMIN_H

#include <QMainWindow>
#include <QTableWidget>

namespace Ui {
class admin;
}

class admin : public QMainWindow
{
    Q_OBJECT

public:
    explicit admin(QWidget *parent = nullptr);
    ~admin();


private:
    Ui::admin *ui;
};

#endif // ADMIN_H
