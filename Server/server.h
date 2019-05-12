#ifndef SERVER_H
#define SERVER_H

#include <QMainWindow>
#include <QtNetwork>
#include <QPushButton>
#include <QComboBox>
#include <QGridLayout>
#include <QWidget>
#include <QByteArray>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonParseError>
#include <QJsonValue>
#include <QJsonObject>
#include <QDebug>

#include "Database/database.h"

enum FUNCTION
{
    GAMER_SIGNIN,
    EXAMER_SIGNIN,
    GAMER_SIGNUP,
    EXAMER_SIGNUP,
    GAMER_SIGNOUT,
    EXAMER_SIGNOUT,
    GET_GAMERINFO,
    GET_EXAMERINFO,
    ADDWORD,
    GETWORD,
    GET_GAMERRANKLIST,
    GET_EXAMERRANKLIST,
    UPDATE_GAMERINFO,
    UPDATE_EXAMERINFO
};


class Server : public QMainWindow
{
    Q_OBJECT

public:
    explicit Server(QWidget *parent = nullptr);
    ~Server();

    void gamerSignin(int clientNum, QString username, QString password);
    void gamerSignup(int clientNum, QString nickname, QString username, QString password);
    void examerSignin(int clientNum, QString username, QString password);
    void examerSignup(int clientNum, QString nickname, QString username, QString password);
    void getGamerInfo(int clientNum, QString username);
    void getExamerInfo(int clientNum, QString username);
    void addWord(int clientNum, QString word);
    void getWord(int clientNum, int difficultDegree);
    void gamerSignout(int clientNum, QString username);
    void examerSignout(int clientNum, QString username);
    void updateGamerInfo(int clientNum, Gamer gamer);
    void updateExamerInfo(int clientNum, Examer examer);
    void getGamerRanklist(int clientNum);
    void getExamerRanklist(int clientNum);
protected:
    void initServer();
private:
    void sendInfo(QJsonObject infoObject, int clientNum);
private slots:
    void on_startButton_clicked();
    void on_endButton_clicked();
    void acceptConnection();
    void receiveInfo();
    void displayError(QAbstractSocket::SocketError);
    void disconnection();
private:
    QTcpServer *server;
    QList<QTcpSocket*> client;
    QTcpSocket *infoReceiver;

    QPushButton *startButton;
    QPushButton *endButton;
    QComboBox *clientComboBox;

    QGridLayout *serverLayout;
    QWidget *centralWidget;
};

#endif // SERVER_H
