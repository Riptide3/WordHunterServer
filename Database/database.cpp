#include "database.h"

Database::Database()
{

}

Database::~Database()
{

}

void Database::init()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setHostName("127.0.0.1");
    db.setDatabaseName("WordHunter");
    db.setUserName("yzy");
    db.setPassword("521521");
    if(!db.open())
    {
        qDebug() << "数据库创建失败" ;
    }
    else
    {
        qDebug() << "数据库创建成功" ;
        query = new QSqlQuery;
        query->exec("create table gamer(username varchar(20) primary key, \
                    nickname varchar(20), password varchar(20), \
                    level int, experiencePoint int, \
                    passedStageNumber int, state int)");
        query->exec("create table examer(username varchar(20) primary key, \
                    nickname varchar(20), password varchar(20), \
                    level int, experiencePoint int, \
                    QuestionNumber int, state int)");
        query->exec("create table words(word varchar(30) primary key)");
    }
}

STATE Database::gamerSignin(QString username, QString password)
{
    STATE state = NOUSER;
    query = new QSqlQuery;
    QString gamer = QString("select * from gamer where \
                            username = '%1'").arg(username);
    query->exec(gamer);

    if(query->first())
    {
        if(query->value(2).toString() == password)
        {
            state = static_cast<STATE>(query->value(6).toInt());

            if(state == OFFLINE)
            {
                QString online = QString("update gamer set \
                                          state = %1 \
                                          where username = '%2'").arg(ONLINE).arg(username);
                query->exec(online);
            }
        }
        else
        {
            state = WRONGPASSWD;
        }
    }
    else
    {
        qDebug() << query->lastError();
    }

    return state;
}

bool Database::gamerSignup(QString nickname, QString username, QString password)
{
    query = new QSqlQuery;
    QString gamer = QString("select * from gamer where \
                            username = '%1' ").arg(username);
    if(query->exec(gamer))
    {
        if(query->first())
        {
            return false;
        }
        else
        {
            QString nGamer = QString("insert into gamer values(?, ?, ?, ?, ?, ?, ?)");
            query->prepare(nGamer);
            query->bindValue(0, username);
            query->bindValue(1, nickname);
            query->bindValue(2, password);
            query->bindValue(3, 0);
            query->bindValue(4, 0);
            query->bindValue(5, 0);
            query->bindValue(6, OFFLINE);
            query->exec();
            qDebug() << "注册成功";
            return true;
        }
    }
    else
    {
        qDebug() << query->lastError();
        return false;
    }
}

STATE Database::examerSignin(QString username, QString password)
{
    STATE state = NOUSER;
    query = new QSqlQuery;
    QString examer = QString("select * from examer where \
                              username = '%1'").arg(username);
    query->exec(examer);
    if(query->first())
    {
        if(query->value(2).toString() == password)
        {
            state = static_cast<STATE>(query->value(6).toInt());
            if(state == OFFLINE)
            {
                QString online = QString("update examer set \
                                          state = %1 \
                                          where username = '%2'").arg(ONLINE).arg(username);
                query->exec(online);
            }
        }
        else
        {
            state = WRONGPASSWD;
        }
    }

    return state;
}

bool Database::examerSignup(QString nickname, QString username, QString password)
{
    query = new QSqlQuery;
    QString examer = QString("select * from examer where \
                             username = '%1'").arg(username);
    if(query->exec(examer))
    {
        if(query->first())
        {
            return false;
        }
        else
        {
            QString nExamer = QString("insert into examer values(?, ?, ?, ?, ?, ?, ?)");
            query->prepare(nExamer);
            query->bindValue(0, username);
            query->bindValue(1, nickname);
            query->bindValue(2, password);
            query->bindValue(3, 0);
            query->bindValue(4, 0);
            query->bindValue(5, 0);
            query->bindValue(6, OFFLINE);
            query->exec();
            qDebug() << "注册成功";
            return true;
        }
    }
    else
    {
        qDebug() << query->lastError();
        return false;
    }
}

Gamer Database::getGamerInfo(QString username)
{
    query = new QSqlQuery;
    QString getInfo = QString("select * from gamer where \
                                username = '%1'").arg(username);
    query->exec(getInfo);
    Gamer gamer;
    qDebug() << "查找用户" << username;
    if(query->first())
    {
        qDebug() << "找到用户" << username;
        gamer.nickname = query->value(1).toString();
        gamer.level = query->value(3).toInt();
        gamer.exp = query->value(4).toInt();
        gamer.passedStage = query->value(5).toInt();
        gamer.username = username;
        qDebug() << "数据库中" << gamer.username;
        return gamer;
    }
    else
    {
        gamer.nickname = "";
        gamer.username = "";
        gamer.level = 0;
        gamer.exp = 0;
        gamer.passedStage = 0;
        qDebug() << query->lastError();
        return gamer;
    }
}

Examer Database::getExamerInfo(QString username)
{
    query = new QSqlQuery;
    QString getInfo = QString("select * from examer where \
                                username = '%1'").arg(username);
    query->exec(getInfo);
    Examer examer;
    if(query->first())
    {
        examer.nickname = query->value(1).toString();
        examer.level = query->value(3).toInt();
        examer.exp = query->value(4).toInt();
        examer.questionNum = query->value(5).toInt();
        examer.username = username;
        qDebug() << "数据库中" << examer.username;
        return examer;
    }
    else
    {
        examer.nickname = "";
        examer.username = "";
        examer.level = 0;
        examer.exp = 0;
        examer.questionNum = 0;
        qDebug() << query->lastError();
        return examer;
    }
}


void Database::updateInfo(Gamer gamer)
{
    query = new QSqlQuery;
    QString updateInfo = QString("update gamer \
                                    set nickname = '%1', \
                                    level = %2, \
                                    experiencePoint = %3, \
                                    passedStageNumber = %4 \
                                    where username = '%5'").arg(gamer.nickname) \
                                                           .arg(gamer.level) \
                                                           .arg(gamer.exp) \
                                                           .arg(gamer.passedStage) \
                                                           .arg(gamer.username);
    query->exec(updateInfo);
}

void Database::updateInfo(Examer examer)
{
    query = new QSqlQuery;
    QString updateInfo = QString("update examer \
                                    set nickname = '%1', \
                                    level = %2, \
                                    experiencePoint = %3, \
                                    QuestionNumber = %4 \
                                    where username = '%5'").arg(examer.nickname) \
                                                           .arg(examer.level) \
                                                           .arg(examer.exp) \
                                                           .arg(examer.questionNum) \
                                                           .arg(examer.username);
    query->exec(updateInfo);
}

bool Database::addWord(QString word)
{
    query = new QSqlQuery;
    QString isExisting = QString("select * from words where \
                           word = '%1'").arg(word);
    if(query->exec(isExisting))
    {
        if(query->first())
        {
            return false;
        }
        else
        {
            QString isNewWord = QString("insert into words values(?)");
            query->prepare(isNewWord);
            query->bindValue(0, word);
            query->exec();
            qDebug() << "添加单词" << word;
            return true;
        }
    }
    else
    {
        qDebug() << query->lastError();
        return false;
    }
}

QString Database::getWord(int difficultDegree)
{
    qDebug() << "单词难度" << difficultDegree;
    query = new QSqlQuery;
    QString getword = QString("select * from words where length(word) = %1 \
                                order by random() limit 1").arg(difficultDegree);
    query->exec(getword);

    QString word;

    if(query->first())
    {
        word = query->value(0).toString();
    }
    else
    {
        qDebug() << query->lastError();
    }
    return word;
}

QVector<Gamer> Database::getGamerRanklist()
{
    qDebug() << "数据库中获取玩家列表";
    query = new QSqlQuery;
    QString getRanklist = QString("select * from gamer");

    QVector<Gamer> gamerRanklist;

    query->exec(getRanklist);
    Gamer gamer;
    while(query->next())
    {
        gamer.nickname = query->value(1).toString();
        gamer.username = query->value(0).toString();
        gamer.level = query->value(3).toInt();
        gamer.exp = query->value(4).toInt();
        gamer.passedStage = query->value(5).toInt();

        gamerRanklist.append(gamer);
    }
    return gamerRanklist;
}

QVector<Examer> Database::getExamerRanklist()
{
    query = new QSqlQuery;
    QString getRanklist = QString("select * from examer");

    QVector<Examer> examerRanklist;

    query->exec(getRanklist);
    Examer examer;
    while(query->next())
    {
        examer.nickname = query->value(1).toString();
        examer.username = query->value(0).toString();
        examer.level = query->value(3).toInt();
        examer.exp = query->value(4).toInt();
        examer.questionNum = query->value(5).toInt();

        examerRanklist.append(examer);
    }
    return examerRanklist;
}

void Database::gamerSignout(QString username)
{
    query = new QSqlQuery;
    QString find = QString("select * from gamer where \
                            username = '%1'").arg(username);
    query->exec(find);

    if(query->first())
    {
        QString offline = QString("update gamer set \
                                  state = %1 \
                                  where username = '%2'").arg(OFFLINE).arg(username);
        query->exec(offline);
        qDebug() << username << "下线";
    }
    else
    {
        qDebug() << query->lastError();
    }
}

void Database::examerSignout(QString username)
{
    query = new QSqlQuery;
    QString find = QString("select * from examer where \
                            username = '%1'").arg(username);
    query->exec(find);

    if(query->first())
    {
        QString offline = QString("update examer set \
                                  state = %1 \
                                  where username = '%2'").arg(OFFLINE).arg(username);
        query->exec(offline);
    }
    else
    {
        qDebug() << query->lastError();
    }
}
