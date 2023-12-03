#include "home.h"
#include "ui_home.h"
#include "admin.h"
#include <QDateTime>
#include <iostream>

#define lln qint64
qint64 start_timestamp;
qint64 stop_timestamp;
lln fin_timestamp;
lln deb;
QString matiere;
QVector <QString> scanned;
QString recovered_badge;

home::home(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::home),scanning(false),writing(false),recovery(false)
{
    ui->setupUi(this);
    ui->ens->setMaximumHeight(0);
    anim_ens();
    QPixmap pix("/home/huzer/appli/images/POINTE.png");
    ui->logos->setPixmap(pix);
    ui->logos->setScaledContents(true);
    ui->logos->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    ui->LOGO_im->setPixmap(pix);
    ui->LOGO_im->setScaledContents(true);
    ui->LOGO_im->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    anim_adm();
    ui->stackedWidget->setCurrentIndex(0);
    serialPort = new QSerialPort(this);
    serialPort->setPortName("/dev/ttyACM0"); // Remplacez par le port série de votre Arduino
    serialPort->setBaudRate(QSerialPort::Baud9600);
    serialPort->open(QIODevice::ReadWrite);
    connect(serialPort, &QSerialPort::readyRead, this, &home::onSerialDataReceived);
    connect(serialPort, &QSerialPort::readyRead, this, &home::onSerialWritingRecieved);
    connect(serialPort, &QSerialPort::readyRead, this, &home::onRecoverData);
    if(!serialPort->isOpen()){
        QMessageBox::critical(this, "Erreur d'arduino", "Veuillez connecter l'arduino et redemarrer l'appli.");
        this->close();
    }
}




home::~home()
{
    delete ui;

}

//-------------------ANIMATION-----------------------//
void home::anim_ens(){
    QGraphicsOpacityEffect *opacity_effect = new QGraphicsOpacityEffect (ui->ens);
    ui->ens->setGraphicsEffect(opacity_effect);

    QPropertyAnimation *geometry_animation = new QPropertyAnimation();
    geometry_animation->setTargetObject(ui->ens);
    geometry_animation->setPropertyName("maximumHeight");
    geometry_animation->setDuration(11500);
    geometry_animation->setStartValue(0);
    geometry_animation->setEndValue(800);
    geometry_animation->setEasingCurve(QEasingCurve::InOutCubic);

    QPropertyAnimation *opacity_animation = new QPropertyAnimation();
    opacity_animation->setTargetObject(opacity_effect);
    opacity_animation->setPropertyName("opacity");
    opacity_animation->setDuration(12000);
    opacity_animation->setStartValue(0);
    opacity_animation->setEndValue(1);
    opacity_animation->setEasingCurve(QEasingCurve::InOutCubic);

    QParallelAnimationGroup *anim_descri = new QParallelAnimationGroup(ui->ens);
    anim_descri->addAnimation(geometry_animation);
    anim_descri->addAnimation(opacity_animation);
    anim_descri->start();
}

void home::anim_adm(){
    QGraphicsOpacityEffect *opacity_effect = new QGraphicsOpacityEffect (ui->adm);
    ui->adm->setGraphicsEffect(opacity_effect);

    QPropertyAnimation *geometry_animation = new QPropertyAnimation();
    geometry_animation->setTargetObject(ui->adm);
    geometry_animation->setPropertyName("maximumHeight");
    geometry_animation->setDuration(11500);
    geometry_animation->setStartValue(0);
    geometry_animation->setEndValue(800);
    geometry_animation->setEasingCurve(QEasingCurve::InOutCubic);

    QPropertyAnimation *opacity_animation = new QPropertyAnimation();
    opacity_animation->setTargetObject(opacity_effect);
    opacity_animation->setPropertyName("opacity");
    opacity_animation->setDuration(12000);
    opacity_animation->setStartValue(0);
    opacity_animation->setEndValue(1);
    opacity_animation->setEasingCurve(QEasingCurve::InOutCubic);

    QParallelAnimationGroup *anim_descri = new QParallelAnimationGroup(ui->adm);
    anim_descri->addAnimation(geometry_animation);
    anim_descri->addAnimation(opacity_animation);
    anim_descri->start();
}
//-------------------ANIMATION-----------------------//

//-------------------PAGE 1 (login) STACKED WIDGET-------------------//

            //---Login de Profs---//
void home::on_login_enseignant_clicked()
{
    QString enseignant_name = ui->enseignant_usrname_log->text();
    QString enseignant_matiere = ui->enseignant_email_log->text();
    matiere = enseignant_matiere;
    QString enseignant_password = ui->enseignant_mdp_log->text();

    QSqlQuery query;
    query.prepare("SELECT * FROM ENSEIGNANT WHERE Nom_d_utilisateur = :nom AND Matiere = :mat AND Mot_de_passe = :motDePasse");
    query.bindValue(":nom", enseignant_name);
    query.bindValue(":mat", enseignant_matiere);
    query.bindValue(":motDePasse", enseignant_password);

    if (query.exec()) {
        bool loginSucceeded = false;
        while (query.next()) {
            QString enseignant_name_db = query.value(0).toString();
            QString enseignant_matiere_db = query.value(1).toString();
            QString enseignant_password_db = query.value(2).toString();

            if (enseignant_matiere == enseignant_matiere_db && enseignant_password == enseignant_password_db && enseignant_name == enseignant_name_db) {
                ui->stackedWidget->setCurrentIndex(1);
                annuler();
                loginSucceeded = true;
                break; // Sortir de la boucle car l'utilisateur est trouvé
            }
        }

        if (!loginSucceeded) {
            qDebug() << "Aucun utilisateur correspondant trouvé.";
            QMessageBox::critical(this, "Erreur de saisie", "Veuillez vérifier vos informations de connexion.");
        }
    } else {
        qDebug() << "Erreur d'exécution de la requête : " << query.lastError().text();
        QMessageBox::critical(this, "Erreur de base de données", "Une erreur s'est produite lors de la connexion à la base de données.");
    }
}
            //---Sign in de Profs---//
void home::on_enseignant_sign_btn_clicked()
{
        QString enseigant_name = ui->nom_enseignant_sign->text();
        QString enseignant_matiere = ui->email_inscript->text();
        matiere=enseignant_matiere;
        QString enseignant_password = ui->mdp_enseignant_sign->text();

        QRegExp nameRegex("^([A-ZÉÀÇ][a-zéèàç]*)+(\\s[A-ZÉÀÇ][a-zéèàç]*)*$"); // Commence par une majuscule, suivie de minuscules
        if (!nameRegex.exactMatch(enseigant_name) || enseigant_name.length() > 50) {
            QMessageBox::critical(this, "Erreur de saisie", "Le nom de l'enseignant est invalide.");
            return;
        }
        if (enseigant_name =="") {
            QMessageBox::critical(this, "Erreur de saisie", "Le nom de l'enseignant est invalide.");
            return;
        }

        QRegExp matiereRegex("^[A-ZÉÀÇa-zéèçà ]*$"); // Autorise les lettres et les espaces
        if (!matiereRegex.exactMatch(enseignant_matiere) || enseignant_matiere.length() > 20){
            QMessageBox::critical(this, "Erreur de saisie", "La matière de l'enseignant est invalide.");
            return;
        }
        if (enseignant_matiere==""){
            QMessageBox::critical(this, "Erreur de saisie", "La matière de l'enseignant est invalide.");
            return;
        }
        if(enseignant_password==""){
            QMessageBox::critical(this, "Erreur de saisie", "Le mot de passe de l'enseignant est invalide.");
            return;
        }

        QSqlQuery query;
        query.prepare("INSERT INTO ENSEIGNANT (Nom_d_utilisateur, Matiere, Mot_de_passe) VALUES (:nom, :nomDutilisateur, :motDePasse)");
        query.bindValue(":nom", enseigant_name);
        query.bindValue(":nomDutilisateur", enseignant_matiere);
        query.bindValue(":motDePasse",enseignant_password);

        if(query.exec()){
            ui->stackedWidget->setCurrentIndex(1);
        }
        annuler();
}

            //---Login d'administrateur---//
void home::on_admin_login_clicked()
{
        QString admin_name = ui->nom_admin->text();
        QString admin_passd = ui->mdp_admin->text();
        QDate currentDate= QDate::currentDate();
        ui->input_date_recherche_ens_2->setDate(currentDate);
        QSqlQuery query;
        query.prepare("SELECT * FROM ADMIN WHERE nom= :nom AND psswrd = :motDePasse");
        query.bindValue(":nom",admin_name);
        query.bindValue(":motDePasse",admin_passd);

        if (query.exec()) {
            bool loginSucceeded = false;
            while (query.next()) {
                QString admin_name_db = query.value(0).toString();
                QString admin_passd_db = query.value(1).toString();

                if (admin_name == admin_name_db && admin_passd == admin_passd_db) {
                    ui->stackedWidget->setCurrentIndex(3);
                    annuler();
                    affichage_a_jour();
                    affichage_eleve();
                    loginSucceeded = true;
                    break;
                }
            }
            if (!loginSucceeded) {
                QMessageBox::critical(this, "Erreur de connexion", "La connexion a échoué. Veuillez vérifier votre nom d'utilisateur et votre mot de passe.");
            }
        }
}

//-------------------PAGE 2 (INTERFACE CRUD~ADMINISTRATEUR) STACKED WIDGET-------------------//

       //------------CRUD côté Enseignant------------//

     //---Barre de recherche d'enseignant dans BDD---//
void home::on_rechercher_enseignant_admin_clicked()
{
    QSqlQueryModel * model = new QSqlQueryModel();
    QSqlQuery query;
    query.prepare("SELECT * FROM ENSEIGNANT WHERE Nom_d_utilisateur LIKE :nom OR Matiere LIKE :email");
    query.bindValue(":nom","%"+ui->barre_recherche_ens->text()+"%");
    query.bindValue(":email","%"+ui->barre_recherche_ens->text()+"%");
    query.exec();
    model->setQuery(query);
    ui->tableView_enseignant->setModel(model);
    ui->tableView_enseignant->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableView_enseignant->show();
}


void home::on_barre_recherche_ens_textChanged(const QString &arg1)
{
    if(arg1 == ""){
        affichage_a_jour();
    }
}

            //---Ajout d'enseignant dans BDD---//
void home::on_btn_ajout_enseignant_in_db_clicked()
{
    QString enseigant_name = ui->ajout_name_enseignant_in_db->text();
    QString enseignant_matiere = ui->ajout_email_enseignant_in_db->text();
    QString enseignant_password = ui->ajout_password_enseignant_in_db->text();

    QRegExp nameRegex("^([A-ZÉÀÇ][a-zéèàç]*)+(\\s[A-ZÉÀÇ][a-zéèàç]*)*$"); // Commence par une majuscule, suivie de minuscules
    if (!nameRegex.exactMatch(enseigant_name) || enseigant_name.length() > 50) {
        QMessageBox::critical(this, "Erreur de saisie", "Le nom de l'enseignant est invalide.");
        return;
    }
    if (enseigant_name=="") {
        QMessageBox::critical(this, "Erreur de saisie", "Le nom de l'enseignant est invalide.");
        return;
    }

    QRegExp matiereRegex("^[A-ZÉÀÇa-zéèàç]*$"); // Autorise les lettres et les espaces
    if (!matiereRegex.exactMatch(enseignant_matiere) || enseignant_matiere.length() > 20) {
        QMessageBox::critical(this, "Erreur de saisie", "La matière de l'enseignant est invalide.");
        return;
    }
    if (enseignant_matiere=="") {
        QMessageBox::critical(this, "Erreur de saisie", "La matière de l'enseignant est invalide.");
        return;
    }

    if(enseignant_password==""){
        QMessageBox::critical(this, "Erreur de saisie", "Le mot de passe de l'enseignant est invalide.");
        return;
    }

    QSqlQuery query;
    query.prepare("INSERT INTO ENSEIGNANT (Nom_d_utilisateur, Matiere, Mot_de_passe) VALUES (:nom, :nomDutilisateur, :motDePasse)");
    query.bindValue(":nom", enseigant_name);
    query.bindValue(":nomDutilisateur", enseignant_matiere);
    query.bindValue(":motDePasse",enseignant_password);
    query.exec();
    QMessageBox::information(this, "Succès", "Données entrées dans la base de donnée !");
    affichage_a_jour();
}

            //---Suppression d'enseignant dans la BDD---//
void home::on_btn_suppr_enseignant_in_db_clicked()
{
        QString enseigant_name = ui->enseignant_name_suppr_in_db->text();
        QString enseignant_email = ui->enseignant_email_suppr_in_db->text();
        QSqlQuery query;
        query.prepare("DELETE FROM ENSEIGNANT WHERE Nom_d_utilisateur = :nom AND Matiere =:nomDutilisateur");
        query.bindValue(":nom", enseigant_name);
        query.bindValue(":nomDutilisateur", enseignant_email);
        query.exec();
        affichage_a_jour();
}

            //---Modification d'identité d'enseignant dans la BDD---//
                            //---Modifier Nom du prof---//
void home::on_btn_modif_name_enseignant_in_db_new_clicked()
{
        QString enseigant_name = ui->enseignant_name_modif_in_db_stock->text();
        QString enseignant_email = ui->enseignant_email_modif_in_db_stock->text();
        QString enseignant_password = ui->enseignant_password_modif_in_db_stock->text();
        QString enseigant_name_new = ui->enseignant_name_modif_in_db_new->text();

        QRegExp nameRegex("^([A-ZÉÀÇ][a-zéèàç]*)+(\\s[A-ZÉÀÇ][a-zéèàç]*)*$"); // Commence par une majuscule, suivie de minuscules
        if (!nameRegex.exactMatch(enseigant_name_new) || enseigant_name_new.length() > 50)  {
            QMessageBox::critical(this, "Erreur de saisie", "Le nom de l'enseignant est invalide.");
            return;
        }
        if (enseigant_name_new=="") {
            QMessageBox::critical(this, "Erreur de saisie", "Le nom de l'enseignant est invalide.");
            return;
        }

        QSqlQuery query;
        query.prepare("UPDATE ENSEIGNANT SET Nom_d_utilisateur = :nomNew WHERE Nom_d_utilisateur = :nom AND Matiere = :nomDutilisateur AND Mot_de_passe = :motDePasse");
        query.bindValue(":nom", enseigant_name);
        query.bindValue(":nomNew", enseigant_name_new);
        query.bindValue(":motDePasse", enseignant_password);
        query.bindValue(":nomDutilisateur", enseignant_email);
        query.exec();
        affichage_a_jour();
}
                            //---Modifier Matière du Prof---//
void home::on_btn_modif_email_enseignant_in_db_new_clicked()
{
        QString enseigant_name = ui->enseignant_name_modif_in_db_stock->text();
        QString enseignant_email = ui->enseignant_email_modif_in_db_stock->text();
        QString enseignant_password = ui->enseignant_password_modif_in_db_stock->text();
        QString enseigant_email_new = ui->enseignant_email_modif_in_db_new->text();

        QRegExp matiereRegex("^[A-ZÉÀÇa-zéçà ]*$"); // Autorise les lettres et les espaces
        if (!matiereRegex.exactMatch(enseigant_email_new) || enseigant_email_new.length() > 20){
            QMessageBox::critical(this, "Erreur de saisie", "La matière de l'enseignant est invalide.");
            return;
        }
        if (enseigant_email_new=="") {
            QMessageBox::critical(this, "Erreur de saisie", "La matière de l'enseignant est invalide.");
            return;
        }

        QSqlQuery query;
        query.prepare("UPDATE ENSEIGNANT SET Matiere = :nomDutilisateurNew WHERE Nom_d_utilisateur = :nom AND Matiere = :nomDutilisateur AND Mot_de_passe = :motDePasse");
        query.bindValue(":nom", enseigant_name);
        query.bindValue(":nomDutilisateurNew", enseigant_email_new);
        query.bindValue(":motDePasse", enseignant_password);
        query.bindValue(":nomDutilisateur", enseignant_email);
        query.exec();
        affichage_a_jour();
}
                            //---Modifier Mot de passe du Prof---//
void home::on_btn_modif_mdp_enseignant_in_db_new_clicked()
{
        QString enseigant_name = ui->enseignant_name_modif_in_db_stock->text();
        QString enseignant_email = ui->enseignant_email_modif_in_db_stock->text();
        QString enseignant_password = ui->enseignant_password_modif_in_db_stock->text();
        QString enseigant_mdp_new = ui->enseignant_mdp_modif_in_db_new->text();

        if(enseigant_mdp_new==""){
            QMessageBox::critical(this, "Erreur de saisie", "Le mot de passe de l'enseignant est invalide.");
            return;
        }

        QSqlQuery query;
        query.prepare("UPDATE ENSEIGNANT SET Mot_de_passe = :motDePasseNew WHERE Nom_d_utilisateur = :nom AND Matiere = :nomDutilisateur AND Mot_de_passe = :motDePasse");
        query.bindValue(":nom", enseigant_name);
        query.bindValue(":motDePasseNew", enseigant_mdp_new);
        query.bindValue(":motDePasse", enseignant_password);
        query.bindValue(":nomDutilisateur", enseignant_email);
        query.exec();
        affichage_a_jour();
}

void home::affichage_a_jour() {
    QSqlQueryModel *model = new QSqlQueryModel();
    QSqlQuery query;
    query.prepare("SELECT * FROM ENSEIGNANT");

    if (query.exec()) {
        model->setQuery(query); // Charger les données dans le modèle
        ui->tableView_enseignant->setModel(model); // Définir le modèle pour la QTableView
        ui->tableView_enseignant->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch); // Redimensionner les colonnes
        ui->tableView_enseignant->show();
    }

}


                //------------CRUD côté Elève------------//

             //---Barre de recherche d'élève dans le BDD---//
void home::on_rechercher_eleve_admin_clicked()
{
    QSqlQueryModel * model = new QSqlQueryModel();
    QSqlQuery query;
    query.prepare("SELECT * FROM ELEVE WHERE Nom LIKE :nom OR Matricule LIKE :mat OR Prenoms LIKE :prenom");
    query.bindValue(":nom","%"+ui->barre_recherche_eleve->text()+"%");
    query.bindValue(":mat","%"+ui->barre_recherche_eleve->text()+"%");
    query.bindValue(":prenom","%"+ui->barre_recherche_eleve->text()+"%");
    query.exec();
    model->setQuery(query);
    ui->tableView_eleves->setModel(model);
    ui->tableView_eleves->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableView_eleves->show();
}
void home::on_barre_recherche_eleve_textChanged(const QString &arg1)
{
    if(arg1 == ""){
        affichage_eleve();
    }
}

                     //---Affichage des élèves---//
void home::affichage_eleve(){
    QSqlQueryModel *model = new QSqlQueryModel();
    QSqlQuery query;
    query.prepare("SELECT * FROM ELEVE");

    if (query.exec()) {
        model->setQuery(query); // Charger les données dans le modèle
        ui->tableView_eleves->setModel(model); // Définir le modèle pour la QTableView
        ui->tableView_eleves->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch); // Redimensionner les colonnes
        ui->tableView_eleves->show();
    }
}
                      //---Ajout d'élève dans BDD---//
void home::on_ajout_eleve_in_db_clicked()
{
    QString eleve_name = ui->ajout_nom_eleve->text();
    QString eleve_prenom = ui->ajout_prenom_eleve->text();
    QString eleve_matricule = ui->ajout_matricule_eleve->text();
    QString eleve_badge = ui->badge_ajout->text();

    QRegExp nameRegex("^[A-Z ]+$");  // Commence par une majuscule, suivie de minuscules
    if (!nameRegex.exactMatch(eleve_name) || eleve_name.length() > 50) {
        QMessageBox::critical(this, "Erreur de saisie", "Le nom de l'élève est invalide.");
        return;
    }
    if (eleve_name=="") {
        QMessageBox::critical(this, "Erreur de saisie", "Le nom de l'élève est invalide.");
        return;
    }

    QRegExp prenomRegex("^([A-ZÉÀÇ][a-zéèçà]*)+(\\s[A-ZÉÀÇ][a-zéèçà]*)*$"); // Commence par une majuscule, suivie de minuscules
    if (!prenomRegex.exactMatch(eleve_prenom) || eleve_prenom.length() > 50) {
        QMessageBox::critical(this, "Erreur de saisie", "Le(s) prénom(s) de l'élève est invalide.");
        return;
    }
    if (eleve_prenom=="") {
        QMessageBox::critical(this, "Erreur de saisie", "Le(s) prénom(s) de l'élève est invalide.");
        return;
    }

    QRegExp matRegex("^\\d+$|^\\d+-HF$");  // Commence par une majuscule, suivie de minuscules
    if (!matRegex.exactMatch(eleve_matricule)) {
        QMessageBox::critical(this, "Erreur de saisie", "Le numero matricule de l'élève est invalide.");
        return;
    }
    if (eleve_matricule=="") {
        QMessageBox::critical(this, "Erreur de saisie", "Le numero matricule de l'élève est invalide.");
        return;
    }
    if(eleve_badge == ""){
        QMessageBox::critical(this, "Erreur de saisie", "Veuiller scanner le badge de l'élève");
        return;
    }

    QSqlQuery query_test_badge;
    query_test_badge.prepare("SELECT CID_badge FROM ELEVE WHERE CID_badge= :uid");
    query_test_badge.bindValue(":uid", eleve_badge);
    query_test_badge.exec();
    if(query_test_badge.next()){
        QMessageBox::critical(this, "Erreur de badge", "Ce badge s'est déjà enregistré!");
        return;
    }

    if(writing){
        QMessageBox::critical(this, "Erreur", "Veuillez stopper le bouton scan d'abord!");
        return;
    }

    QSqlQuery query;
    query.prepare("INSERT INTO ELEVE (Matricule, Nom, Prenoms,CID_badge ) VALUES (:mat, :nom, :prenom, :uid)");
    query.bindValue(":nom", eleve_name);
    query.bindValue(":mat", eleve_matricule);
    query.bindValue(":prenom",eleve_prenom);
    query.bindValue(":uid", eleve_badge);
    if(query.exec()){
        QMessageBox::information(this, "Succès", "Données entrées dans la base de donnée !");
        affichage_eleve();
    }else{
        QMessageBox::critical(this, "Erreur de numero matricule", "Ce numéro éxiste déjà dans la base de donnée!");
    }

}
void home::on_btn_scan_writing_clicked()
{
    if (!writing)
    {
        writing = true;
        // Envoyer une commande à l'Arduino pour démarrer la lecture RFID ici
        serialPort->write("W");
    }
}


void home::on_btn_stop_writing_clicked()
{
    if (writing)
    {
        writing = false;

    }
}

//---Slots de scan de badge---//
void home::onSerialWritingRecieved()
{
    if (writing)
    {
        static QString receivedData = "";
        while (serialPort->canReadLine()) {
            QString line = serialPort->readLine().trimmed().toUpper();
            if (line.isEmpty()) {
                continue; // Ignorer les lignes vides
            }
            receivedData += line; // Ajouter la ligne aux données reçues
        }
        // Vérifier si les données reçues indiquent la présence d'une carte RFID
        if (receivedData.contains("DATA")) { // Remplacez "CARTE_DETECTED" par le texte réel de détection de carte
            // Générez le nombre hexadécimal
            QString nombreHexadecimal;


                for (int i = 0; i < 32; ++i) {
                    int chiffreAleatoire = QRandomGenerator::global()->bounded(16); // Génère un chiffre entre 0 et F
                    QString chiffreHexadecimal = QString::number(chiffreAleatoire, 16).toUpper(); // Convertit en hexadécimal et en majuscules
                    nombreHexadecimal += chiffreHexadecimal;

                    // Ajoute un espace après chaque paire de chiffres hexadécimaux (sauf après le dernier)
                    if (i % 2 == 1 && i != 31) {
                        nombreHexadecimal += " ";
                    }
                }

                serialPort->write(nombreHexadecimal.toUtf8());
            qDebug()<<"Envoyé avec succes";
            for(int i = 0; i<2;i++){
                nombreHexadecimal[i]='0';
            }


            ui->badge_ajout->setText(nombreHexadecimal);
           }

        receivedData = ""; // Réinitialiser les données reçues pour la prochaine lecture
        serialPort->write("N");

    }
}

                      //---Suppression d'élève dans BDD---//
void home::on_btn_supr_eleve_in_db_clicked()
{
    QString eleve_name = ui->supr_nom_eleve->text();
    QString eleve_prenom = ui->supr_prenom_eleve->text();
    QString eleve_matricule = ui->supr_matricule_eleve->text();
    QSqlQuery query;
    query.prepare("DELETE FROM ELEVE WHERE Nom = :nom AND Prenoms =:prenom AND Matricule = :mat ");
    query.bindValue(":nom", eleve_name);
    query.bindValue(":prenom", eleve_prenom);
    query.bindValue(":mat", eleve_matricule);
    query.exec();
    affichage_eleve();
}

                      //---Modification d'identité d'élève dans BDD---//
                            //---Modifier le Nom de l'élève---//
void home::on_btn_modif_eleve_nom_in_db_new_clicked()
{
    QString eleve_nom = ui->eleve_nom_modif_in_db_stock->text();
    QString eleve_prenom = ui->eleve_prenom_modif_in_db_stock->text();
    QString eleve_matricule = ui->eleve_matricule_modif_in_db_stock->text();
    QString eleve_nom_new = ui->eleve_nom_modif_in_db_new->text();

    QRegExp nameRegex("^[A-Z ]+$");  // Commence par une majuscule, suivie de minuscules
    if (!nameRegex.exactMatch(eleve_nom_new) || eleve_nom_new.length() > 50) {
        QMessageBox::critical(this, "Erreur de saisie", "Le nom de l'élève est invalide.");
        return;
    }
    if (eleve_nom_new=="") {
        QMessageBox::critical(this, "Erreur de saisie", "Le nom de l'élève est invalide.");
        return;
    }

    QSqlQuery query_verif;
    query_verif.prepare("SELECT * FROM ELEVE WHERE Nom = :nom AND Prenoms = :prenom AND Matricule = :mat");
    query_verif.bindValue(":nom", eleve_nom);
    query_verif.bindValue(":prenom", eleve_prenom);
    query_verif.bindValue(":mat", eleve_matricule);
    if(query_verif.exec() && !query_verif.next()){
        QMessageBox::critical(this, "Erreur de modification", "L'élève ne figure pas dans la base donnée!");
        return;
    }

    QSqlQuery query;
    query.prepare("UPDATE ELEVE SET Nom = :nomNew WHERE Nom = :nom AND Prenoms = :prenom AND Matricule = :mat");
    query.bindValue(":nom", eleve_nom);
    query.bindValue(":nomNew", eleve_nom_new);
    query.bindValue(":prenom", eleve_prenom);
    query.bindValue(":mat", eleve_matricule);
    query.exec();
    affichage_eleve();
}

                            //---Modifier les Prénoms de l'élève---//
void home::on_btn_modif_eleve_prenom_in_db_new_clicked()
{
    QString eleve_nom = ui->eleve_nom_modif_in_db_stock->text();
    QString eleve_prenom = ui->eleve_prenom_modif_in_db_stock->text();
    QString eleve_matricule = ui->eleve_matricule_modif_in_db_stock->text();
    QString eleve_prenom_new = ui->eleve_prenom_modif_in_db_new->text();

    QRegExp prenomRegex("^([A-ZÉÀÇ][a-zéèçà]*)+(\\s[A-ZÉÀÇ][a-zéèçà]*)*$"); // Commence par une majuscule, suivie de minuscules
    if (!prenomRegex.exactMatch(eleve_prenom_new) || eleve_prenom_new.length() > 50) {
        QMessageBox::critical(this, "Erreur de saisie", "Le(s) prénom(s) de l'élève est invalide.");
        return;
    }
    if (eleve_prenom_new=="") {
        QMessageBox::critical(this, "Erreur de saisie", "Le(s) prénom(s) de l'élève est invalide.");
        return;
    }

    QSqlQuery query_verif;
    query_verif.prepare("SELECT * FROM ELEVE WHERE Nom = :nom AND Prenoms = :prenom AND Matricule = :mat");
    query_verif.bindValue(":nom", eleve_nom);
    query_verif.bindValue(":prenom", eleve_prenom);
    query_verif.bindValue(":mat", eleve_matricule);
    query_verif.exec();
    if(query_verif.exec() && !query_verif.next()){
        QMessageBox::critical(this, "Erreur de modification", "L'élève ne figure pas dans la base donnée!");
        return;
    }

    QSqlQuery query;
    query.prepare("UPDATE ELEVE SET Prenoms = :prenomNew WHERE Nom = :nom AND Prenoms = :prenom AND Matricule = :mat");
    query.bindValue(":nom", eleve_nom);
    query.bindValue(":prenomNew", eleve_prenom_new);
    query.bindValue(":prenom", eleve_prenom);
    query.bindValue(":mat", eleve_matricule);
    query.exec();
    affichage_eleve();
}

                //---Recherche de présence d'élèves---//
void home::changes(){
    QSqlQueryModel * model = new QSqlQueryModel();
    QString critere[3] = {"Date","Matiere","Matricule"};
    QString tab_line_edit[3] = {ui->input_date_recherche_ens_2->text(),ui->input_matiere_recherche_ens_2->text(),ui->input_matricule_recherche_ens_2->text()};
    int t_i[3] = {1,1,1};
    t_i[0] = (tab_line_edit[0].size())? 1:0;
    t_i[1] = (tab_line_edit[1].size())? 1:0;
    t_i[2] = (tab_line_edit[2].size())? 1:0;
    QString requete = "SELECT * FROM PRESENCE WHERE ";
    int c = 0;
    for(int i = 0 ; i < 3 ; i++){
        if(t_i[i]){
            requete += ((c)? " AND ":"") + critere[i] + " LIKE '%"+tab_line_edit[i]+"%'";
            c++;
        }
    }

    if(c){
        model->setQuery(requete);
        ui->tableViewRecherche_ens_2->setModel(model);
        ui->tableViewRecherche_ens_2->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        ui->tableViewRecherche_ens_2->show();
     }
}

//--------------------------------------//


void home::on_input_matricule_recherche_ens_2_textChanged(const QString &arg1)
{
    changes();
}


void home::on_input_matiere_recherche_ens_2_textChanged(const QString &arg1)
{
    changes();
}


void home::on_input_date_recherche_ens_2_userDateChanged(const QDate &date)
{
    changes();
}

//-------------------PAGE 3 (INTERFACE DE PROF) STACKED WIDGET-------------------//
                       //-------Page de SCAN de badge-------//

//------Bouton SCAN------//
void home::on_btn_scanner_badge_prof_clicked()
{
    if (!scanning)
    {
        scanning = true;
        // Envoyer une commande à l'Arduino pour démarrer la lecture RFID ici
        serialPort->write("S");
        QDateTime current_stamp = QDateTime::currentDateTime();
        start_timestamp = current_stamp.toMSecsSinceEpoch();
        QSqlQuery query;
        query.prepare("CREATE TABLE TIMESTAMP(uid varchar(50),debut integer,fin integer, variation integer,counter integer)");
        qDebug()<<query.lastError();
        query.exec();
        QDate currentDate= QDate::currentDate();
        QString date_presence= currentDate.toString("dd-MM-yyyy");
        QSqlQuery query_select;
        query_select.prepare("SELECT Matricule FROM ELEVE");
        query_select.exec();
        QSqlQuery query_presence;
        while(query_select.next()){
            query_presence.prepare("INSERT INTO PRESENCE(Matricule,Matiere,Date,Presence) VALUES (:matricule,:matiere,:time,'newabsent')");
            query_presence.bindValue(":matricule",query_select.value(0).toString());
            query_presence.bindValue(":matiere",matiere);
            query_presence.bindValue(":time",date_presence);
            query_presence.exec();
        }
    }
}

//------Bouton STOP------//
void home::on_btn_stopper_badge_prof_clicked()
{
    if (scanning)
    {
        scanning = false;
        serialPort->write("X");
        QDateTime current_stamp = QDateTime::currentDateTime();
        stop_timestamp = current_stamp.toMSecsSinceEpoch();
        float variation_scan = stop_timestamp - start_timestamp;

        //Algo de presence //

        QSqlQuery query_timestamp;
        query_timestamp.prepare("SELECT * FROM TIMESTAMP");
        query_timestamp.exec();
        while(query_timestamp.next()){
            float var = (query_timestamp.value(3).toInt()/variation_scan);
            if(var >=0.2){
                QSqlQuery query_select;
                query_select.prepare("SELECT Matricule FROM ELEVE WHERE CID_badge = :uid");
                query_select.bindValue(":uid", query_timestamp.value(0).toString());
                query_select.exec();
                while(query_select.next()){
                    QSqlQuery query_presence;
                    query_presence.prepare("UPDATE PRESENCE SET Presence = 'present' WHERE Matricule = :matricule AND Presence= 'newabsent'");
                    query_presence.bindValue(":matricule", query_select.value(0).toString());
                    query_presence.exec();
                }
            }
        }
        // Mettre tout les absent en absent
        QSqlQuery query_presence;
        query_presence.prepare("UPDATE PRESENCE SET Presence = 'absent' WHERE Presence= 'newabsent'");
        query_presence.exec();

        //Suppression de table temporaire timestamp
        QSqlQuery q;
        q.prepare("DROP TABLE TIMESTAMP");
        q.exec();

    }
}
//-----Slots de recetpion des données depuis ARDUINO------//
void home::onSerialDataReceived()
{
    if (scanning)
    {
        static QString receivedData = "";

        while (serialPort->canReadLine()) {
            QString line = serialPort->readLine().trimmed().toUpper();

            if (line.isEmpty()) {
                continue; // Ignorer les lignes vides
            }

            receivedData += line; // Ajouter la ligne aux données reçues
        }

        // Vérifier si les données reçues contiennent un badge complet (par exemple, "B1 8A 39 06")
        if (receivedData.contains(" ")) {
            // Divisez la chaîne en octets séparés par un espace
            QStringList bytes = receivedData.split(" ");

            // Formatez chaque octet pour avoir deux caractères
            for (int i = 0; i < bytes.size(); i++) {
                if (bytes[i].size() == 1) {
                    bytes[i] = "0" + bytes[i];
                }
            }

            // Reconstituez la chaîne formatée
            receivedData = bytes.join(" ");

                QSqlQuery query_1;
                query_1.prepare("SELECT uid FROM TIMESTAMP WHERE uid= :uid");
                query_1.bindValue(":uid", receivedData);

                if(query_1.exec() && query_1.next()){
                    QSqlQuery query_2;
                    query_2.prepare("UPDATE TIMESTAMP SET counter = counter + 1 WHERE uid = :uid");
                    query_2.bindValue(":uid",receivedData);
                    query_2.exec();
                    int c;
                    qDebug()<<"data recu: "<<receivedData;
                    QSqlQuery query_4;
                    query_4.prepare("SELECT counter FROM TIMESTAMP WHERE uid = :uid");
                    query_4.bindValue(":uid", receivedData);
                    query_4.exec();
                    if(query_4.next()){
                        c = query_4.value(0).toInt();
                        if(c == 2){
                            QSqlQuery query_5;
                            QDateTime current_stamp = QDateTime::currentDateTime();
                            fin_timestamp = current_stamp.toMSecsSinceEpoch();
                            query_5.prepare("UPDATE TIMESTAMP SET fin = :fin WHERE uid = :uid");
                            query_5.bindValue(":uid",receivedData);
                            query_5.bindValue(":fin",fin_timestamp);
                            query_5.exec();
                            QSqlQuery query_3;
                            query_3.prepare("SELECT debut FROM TIMESTAMP WHERE uid= :uid");
                            query_3.bindValue(":uid", receivedData);
                            if(query_3.exec() && query_3.next()){
                                deb = query_3.value(0).toULongLong();
                            }
                            qDebug()<<"variation: "<<fin_timestamp - deb;
                            query_2.prepare("UPDATE TIMESTAMP SET variation = :variation WHERE uid = :uid");
                            query_2.bindValue(":uid",receivedData);
                            query_2.bindValue(":variation",fin_timestamp - deb);
                            query_2.exec();
                        }
                    }

                }else{
                    QDateTime current_stamp = QDateTime::currentDateTime();
                    lln debut_timestamp;
                    debut_timestamp = current_stamp.toMSecsSinceEpoch();
                    QSqlQuery query_2;
                    query_2.prepare("INSERT INTO TIMESTAMP(uid,debut,counter) VALUES (:uid,:debut,1)");
                    query_2.bindValue(":uid",receivedData);
                    query_2.bindValue(":debut",debut_timestamp);
                    query_2.exec();

                    QString matric = 0;

                    QSqlQuery query_sel;
                    query_sel.prepare("SELECT Matricule FROM ELEVE WHERE CID_badge = :uid");
                    query_sel.bindValue(":uid", receivedData);
                    query_sel.exec();
                    while(query_sel.next()){
                        matric = query_sel.value(0).toString();
                    }

                    scanned.push_back(matric);

                    affichage_scanned(scanned);



                    qDebug()<<query_2.lastError();
                }

            qDebug()<<receivedData<<endl;
            receivedData = ""; // Réinitialiser les données reçues pour la prochaine lecture
        }
    }
}


//-----Fonction d'affichage des badges scannées-----//
void home::affichage_scanned(QVector<QString> v) {
    QSqlQueryModel *model = new QSqlQueryModel();
    QString queryText = "SELECT * FROM ELEVE WHERE Matricule IN (";

    for (int i = 0; i < v.size(); ++i) {
        queryText += v[i];

        if (i < v.size() - 1) {
            queryText += ",";
        }
    }

    queryText += ")";

    QSqlQuery query;
    query.prepare(queryText);

    if (query.exec()) {
        model->setQuery(query);
        ui->affichage_badge->setModel(model);
        ui->affichage_badge->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        ui->affichage_badge->show();
    }
}
void home::clearTableView() {
    QSqlQueryModel *model = new QSqlQueryModel();
    ui->affichage_badge->setModel(model);
    ui->affichage_badge->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}


                       //-------Page de recherche d'élève present-----//
void home::changement(){
    QSqlQueryModel * model = new QSqlQueryModel();
    QString critere[3] = {"Date","Matiere","Matricule"};
    QString tab_line_edit[3] = {ui->input_date_recherche_ens->text(),matiere,ui->input_matricule_recherche_ens->text()};
    int t_i[3] = {1,1,1};
    t_i[0] = (tab_line_edit[0].size())? 1:0;
    t_i[1] = (tab_line_edit[1].size())? 1:0;
    t_i[2] = (tab_line_edit[2].size())? 1:0;
    QString requete = "SELECT * FROM PRESENCE WHERE ";
    int c = 0;
    for(int i = 0 ; i < 3 ; i++){
        if(t_i[i]){
            requete += ((c)? " AND ":"") + critere[i] + " LIKE '%"+tab_line_edit[i]+"%'";
            c++;
        }
    }

    if(c){
        model->setQuery(requete);
        ui->tableViewRecherche_ens->setModel(model);
        ui->tableViewRecherche_ens->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        ui->tableViewRecherche_ens->show();
     }
}


void home::on_input_matricule_recherche_ens_textChanged(const QString &arg1)
{
    changement();
}

void home::on_input_date_recherche_ens_userDateChanged(const QDate &date)
{
    changement();
}

//-----Bouton de navigation de page-----//
void home::on_btn_redir_scann_bade_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
}
void home::on_btn_menu_principale_eseignant_clicked()
{
    matiere="";
    ui->stackedWidget->setCurrentIndex(0);
    clearTableView();
    annuler();
}
void home::on_btn_recherche_enseignant_clicked()
{
    ui->stackedWidget->setCurrentIndex(2);
    QDate currentDate= QDate::currentDate();
    ui->input_date_recherche_ens->setDate(currentDate);
}
void home::on_btn_menu_principale_eseignant_2_clicked()
{
    matiere="";
    ui->stackedWidget->setCurrentIndex(0);
    clearTableView();
    annuler();
}

//---Slots des boutons Annuler---//
void home::on_enseignant_cancel_btn_1_clicked()
{
    annuler();
}
void home::on_enseignant_cancel_btn_clicked()
{
    annuler();
}
void home::on_admin_cancel_clicked()
{
    annuler();
}
void home::on_pushButton_18_clicked()
{
    annuler();
}
void home::on_pushButton_21_clicked()
{
    annuler();
}
void home::on_pushButton_22_clicked()
{
    annuler();
}
void home::on_pushButton_31_clicked()
{
    annuler();
}
void home::on_pushButton_27_clicked()
{
    annuler();
}
void home::on_pushButton_19_clicked()
{
    annuler();
}
void home::on_pushButton_20_clicked()
{
    annuler();
}
void home::on_pushButton_23_clicked()
{
    annuler();
}
void home::on_pushButton_28_clicked()
{
    annuler();
}
void home::on_pushButton_32_clicked()
{
    annuler();
}

void home::annuler() {
    // Parcourir tous les widgets enfants de la fenêtre principale
    QList<QWidget*> widgets = this->findChildren<QWidget*>();
    ui->badge_ajout->setText("");
    for (QWidget* widget : widgets) {
        // Vérifier si le widget est un objet QLineEdit
        QLineEdit* lineEdit = qobject_cast<QLineEdit*>(widget);

        if (lineEdit) {
            // Effacer le contenu de l'objet QLineEdit
            lineEdit->clear();
        }
    }
}
void home::on_btn_quitter_clicked()
{
    this->close();
}


void home::on_btn_quitter_2_clicked()
{
    this->close();
}


void home::on_btn_menu_principale_eseignant_3_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}


void home::on_btn_quitter_3_clicked()
{
    this->close();
}








void home::on_find_to_recover_clicked()
{
    QSqlQuery query;
    query.prepare("SELECT * FROM ELEVE WHERE Matricule= :mat AND Nom = :nom AND Prenoms= :prenoms");
    query.bindValue(":mat",ui->recove_mat->text());
    query.bindValue(":nom",ui->recover_name->text());
    query.bindValue(":prenoms", ui->recover_prenom->text());


    if(query.exec() && query.next()){
        ui->recover_badge_text->setText(query.value(3).toString());
        recovered_badge = query.value(3).toString();
    }else{
        QMessageBox::critical(this, "Erreur de recherche", "Veulliez entrer les bonnes information de l'éléve svp!");
        return;
    }
}


void home::on_btn_scan_writing_2_clicked()
{
    if (!recovery)
    {
        if(ui->recover_badge_text->text()==""){
            QMessageBox::critical(this, "Erreur de gravage", "Veuiller rechercher l'élève d'abord!");
            return;
        }
        recovery = true;
        // Envoyer une commande à l'Arduino pour démarrer la lecture RFID ici
        serialPort->write("W");
    }
}

void home::onRecoverData(){
    if (recovery)
    {
        static QString receivedData = "";
        while (serialPort->canReadLine()) {
            QString line = serialPort->readLine().trimmed().toUpper();
            if (line.isEmpty()) {
                continue; // Ignorer les lignes vides
            }
            receivedData += line; // Ajouter la ligne aux données reçues
        }
        // Vérifier si les données reçues indiquent la présence d'une carte RFID
        if (receivedData.contains("DATA")) { // Remplacez "CARTE_DETECTED" par le texte réel de détection de carte
            // Générez le nombre hexadécimal
            QString nombreHexadecimal;


                nombreHexadecimal = ui->recover_badge_text->text();
                for(int i = 0; i<2;i++){
                    nombreHexadecimal[i]='0';
                }

                serialPort->write(nombreHexadecimal.toUtf8());
            qDebug()<<"Envoyé avec succes: "<<nombreHexadecimal;
            for(int i = 0; i<2;i++){
                nombreHexadecimal[i]='0';
            }
           }

        receivedData = ""; // Réinitialiser les données reçues pour la prochaine lecture
        serialPort->write("N");
    }
}





void home::on_btn_stop_writing_2_clicked()
{
    if (!recovery)
    {
        recovery = false;
        // Envoyer une commande à l'Arduino pour démarrer la lecture RFID ici
    }
}

