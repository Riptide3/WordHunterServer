#ifndef DATABASE_H
#define DATABASE_H

#include <QSql>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>

#include <QDebug>
#include <QMessageBox>
#include <QString>
#include <QVector>

struct Gamer
{
    QString nickname;
    QString username;
    int level;
    int exp;
    int passedStage;
};

struct Examer
{
    QString nickname;
    QString username;
    int level;
    int exp;
    int questionNum;
};

enum STATE {OFFLINE, ONLINE, WRONGPASSWD, NOUSER};

class Database
{
public:
    Database();
    ~Database();
    void init();
    STATE gamerSignin(QString username, QString password);
    bool gamerSignup(QString nickname, QString username, QString password);
    STATE examerSignin(QString username, QString password);
    bool examerSignup(QString nickname, QString username, QString password);
    void updateInfo(Gamer gamer);
    void updateInfo(Examer examer);
    Gamer getGamerInfo(QString username);
    Examer getExamerInfo(QString username);
    bool addWord(QString word);
    QString getWord(int difficultDegree);
    QVector<Gamer> getGamerRanklist();
    QVector<Examer> getExamerRanklist();
    void gamerSignout(QString username);
    void examerSignout(QString username);
//    STATE getGamerState(QString username);
//    STATE getExamerState(QString username);
private:
    QSqlQuery *query;
};

#endif // DATABASE_H
