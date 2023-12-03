#ifndef SPLASHSCREEN_H
#define SPLASHSCREEN_H

#include <QMainWindow>
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>
#include <QWidget>
#include <QParallelAnimationGroup>
#include <QSequentialAnimationGroup>
#include <QFileInfo>
#include "home.h"

QT_BEGIN_NAMESPACE
namespace Ui { class SplashScreen; }
QT_END_NAMESPACE

class SplashScreen : public QMainWindow
{
    Q_OBJECT

public:
    SplashScreen(QWidget *parent = nullptr);
    ~SplashScreen();
    void animation_logo();
    void anim_descri();
    void progress_bar();

private slots:
    void on_progressBar_valueChanged(int value);

private:
    Ui::SplashScreen *ui;
    home h;
};
#endif // SPLASHSCREEN_H
