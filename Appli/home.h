#ifndef HOME_H
#define HOME_H

#include <QMainWindow>
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>
#include <QWidget>
#include <QParallelAnimationGroup>
#include <QSequentialAnimationGroup>
#include <QMessageBox>
#include <QtSerialPort>
#include <QtSql>
#include <QRandomGenerator>
namespace Ui {
class home;
}

class home : public QMainWindow
{
    Q_OBJECT

public:
    explicit home(QWidget *parent = nullptr);
    ~home();
    void anim_ens();
    void anim_adm();

private slots:

    void on_admin_login_clicked();

    void on_login_enseignant_clicked();

    void on_enseignant_sign_btn_clicked();

    void on_btn_ajout_enseignant_in_db_clicked();

    void on_btn_suppr_enseignant_in_db_clicked();

    void on_btn_modif_name_enseignant_in_db_new_clicked();

    void on_btn_modif_email_enseignant_in_db_new_clicked();

    void on_btn_modif_mdp_enseignant_in_db_new_clicked();

    void on_btn_scanner_badge_prof_clicked();

    void on_btn_stopper_badge_prof_clicked();

    void onSerialDataReceived();

    void on_input_matricule_recherche_ens_textChanged(const QString &arg1);

    void on_btn_redir_scann_bade_clicked();

    void on_btn_recherche_enseignant_clicked();

    void changement();

    void on_input_date_recherche_ens_userDateChanged(const QDate &date);

    void affichage_a_jour();

    void annuler();

    void on_enseignant_cancel_btn_1_clicked();

    void on_enseignant_cancel_btn_clicked();

    void on_admin_cancel_clicked();

    void on_pushButton_18_clicked();

    void on_pushButton_21_clicked();

    void on_pushButton_22_clicked();

    void on_pushButton_31_clicked();

    void on_pushButton_27_clicked();

    void on_rechercher_enseignant_admin_clicked();

    void on_barre_recherche_ens_textChanged(const QString &arg1);

    void on_pushButton_19_clicked();

    void on_pushButton_20_clicked();

    void affichage_scanned(QVector <QString> v);

    void on_btn_menu_principale_eseignant_clicked();

    void clearTableView();

    void on_pushButton_23_clicked();

    void on_pushButton_28_clicked();

    void on_ajout_eleve_in_db_clicked();

    void on_btn_scan_writing_clicked();

    void on_btn_stop_writing_clicked();

    void onSerialWritingRecieved();

    void on_btn_supr_eleve_in_db_clicked();

    void affichage_eleve();

    void on_btn_modif_eleve_nom_in_db_new_clicked();

    void on_btn_modif_eleve_prenom_in_db_new_clicked();

    void on_rechercher_eleve_admin_clicked();

    void on_barre_recherche_eleve_textChanged(const QString &arg1);

    void on_btn_menu_principale_eseignant_2_clicked();

    void on_btn_quitter_clicked();

    void on_btn_quitter_2_clicked();

    void on_btn_menu_principale_eseignant_3_clicked();

    void on_btn_quitter_3_clicked();

    void changes();


    void on_input_matricule_recherche_ens_2_textChanged(const QString &arg1);

    void on_input_matiere_recherche_ens_2_textChanged(const QString &arg1);

    void on_input_date_recherche_ens_2_userDateChanged(const QDate &date);

    void on_pushButton_32_clicked();

    void on_find_to_recover_clicked();

    void on_btn_scan_writing_2_clicked();

    void onRecoverData();

    void on_btn_stop_writing_2_clicked();

private:
    Ui::home *ui;
    QString ilayizy;
    QSerialPort *serialPort;
    bool scanning;
    bool writing;
    QMessageBox msg;
    bool recovery;
};

#endif // HOME_H
