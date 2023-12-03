#include "splashscreen.h"
#include "ui_splashscreen.h"



SplashScreen::SplashScreen(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::SplashScreen)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    QPixmap pix("/home/huzer/appli/images/logo.png");
    ui->logo_img->setPixmap(pix);
    ui->logo_img->setScaledContents(true);
    ui->logo_img->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);

    ui->bg->setMaximumHeight(0);
    this->animation_logo();
    this->anim_descri();
    this->progress_bar();
    QFileInfo checkfile("/home/huzer/appli/BDD/db.sqlite");
    if(checkfile.isFile()){
        ui->status->setText("Status: database ok!");
    }else{
        ui->status->setText("Status: database error!");
        QMessageBox::critical(this, "Erreur DATABASE", "La base de données n'est pas là, ou ne peut pas etre ouverte!");
        return;
    }
}




void SplashScreen::animation_logo(){
    QGraphicsOpacityEffect *opacity_effect = new QGraphicsOpacityEffect (ui->logo_img);
    ui->logo_img->setGraphicsEffect(opacity_effect);

    QPropertyAnimation *logo_opacity_animation = new QPropertyAnimation();
    logo_opacity_animation->setTargetObject(opacity_effect);
    logo_opacity_animation->setPropertyName("opacity");
    logo_opacity_animation->setDuration(2500);
    logo_opacity_animation->setStartValue(0);
    logo_opacity_animation->setEndValue(1);
    logo_opacity_animation->setEasingCurve(QEasingCurve::InOutCubic);
    logo_opacity_animation->start();
}

void SplashScreen::anim_descri(){
    QGraphicsOpacityEffect *opacity_effect = new QGraphicsOpacityEffect (ui->bg);
    ui->bg->setGraphicsEffect(opacity_effect);

    QPropertyAnimation *geometry_animation = new QPropertyAnimation();
    geometry_animation->setTargetObject(ui->bg);
    geometry_animation->setPropertyName("maximumHeight");
    geometry_animation->setDuration(1500);
    geometry_animation->setStartValue(0);
    geometry_animation->setEndValue(228);
    geometry_animation->setEasingCurve(QEasingCurve::InOutCubic);

    QPropertyAnimation *opacity_animation = new QPropertyAnimation();
    opacity_animation->setTargetObject(opacity_effect);
    opacity_animation->setPropertyName("opacity");
    opacity_animation->setDuration(500);
    opacity_animation->setStartValue(0);
    opacity_animation->setEndValue(1);
    opacity_animation->setEasingCurve(QEasingCurve::InOutCubic);

    QParallelAnimationGroup *anim_descri = new QParallelAnimationGroup(ui->bg);
    anim_descri->addAnimation(geometry_animation);
    anim_descri->addAnimation(opacity_animation);
    anim_descri->start();
}

void SplashScreen::progress_bar() {
  QPropertyAnimation *progress_bar_animation = new QPropertyAnimation(ui->progressBar, "value");
  progress_bar_animation->setDuration(5000);
  progress_bar_animation->setStartValue(0);
  progress_bar_animation->setEndValue(100);
  progress_bar_animation->setEasingCurve(QEasingCurve::InOutCubic);
   progress_bar_animation->start();
   ui->status->setText("Status: database ok..!");
}

SplashScreen::~SplashScreen()
{
    delete ui;
}


void SplashScreen::on_progressBar_valueChanged(int value)
{
    if(value == 100){
        this->hide();
        h.show();
    }
}

