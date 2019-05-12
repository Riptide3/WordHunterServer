#include "server.h"

Server::Server(QWidget *parent)
    : QMainWindow(parent)
{
    this->setWindowTitle(tr("服务器"));
    clientComboBox = new QComboBox;
    clientComboBox->setFixedWidth(300);
    startButton = new QPushButton;
    startButton->setText(tr("开启服务器"));
    startButton->setEnabled(true);
    endButton = new QPushButton;
    endButton->setText(tr("关闭服务器"));
    endButton->setEnabled(false);

    centralWidget = new QWidget(this);
    centralWidget->setFixedSize(400, 200);
    this->setCentralWidget(centralWidget);

    serverLayout = new QGridLayout;
    serverLayout->addWidget(clientComboBox, 0, 0, 1, 10, Qt::AlignCenter);
    serverLayout->addWidget(startButton, 1, 0, 1, 10, Qt::AlignCenter);
    serverLayout->addWidget(endButton, 2, 0, 1, 10, Qt::AlignCenter);

    centralWidget->setLayout(serverLayout);

    connect(startButton, SIGNAL(clicked()), this, SLOT(on_startButton_clicked()));
    connect(endButton, SIGNAL(clicked()), this, SLOT(on_endButton_clicked()));

    initServer();
}

Server::~Server()
{

}

void Server::initServer()
{
    this->server = new QTcpServer(this);
    this->infoReceiver = new QTcpSocket(this);

    qDebug() << "初始化服务器";
    connect(server, SIGNAL(newConnection()), this, SLOT(acceptConnection()));
    connect(infoReceiver, SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(displayError(QAbstractSocket::SocketError)));
}

void Server::acceptConnection()
{
    infoReceiver = server->nextPendingConnection();
    client.append(infoReceiver);
    clientComboBox->addItem(tr("%1:%2").arg(infoReceiver->peerAddress().toString().split("::ffff:")[1])\
                                       .arg(infoReceiver->peerPort()));
    connect(infoReceiver, SIGNAL(readyRead()), this, SLOT(receiveInfo()));
    connect(infoReceiver, SIGNAL(disconnected()), this, SLOT(disconnection()));
}

void Server::receiveInfo()
{
    for(int i = 0; i < client.length(); i++)
    {
        QByteArray receivedInfo;
        QJsonParseError infoParse;
        QJsonDocument parseDocument;
        QJsonObject receivedInfoObject;
        receivedInfo = client[i]->readAll();
        parseDocument = QJsonDocument::fromJson(receivedInfo, &infoParse);
        receivedInfoObject = parseDocument.object();
        if(receivedInfoObject.isEmpty())
        {
            continue;
        }
        FUNCTION func = static_cast<FUNCTION>(receivedInfoObject.take("function").toInt());
        QString nickname, username, password, word;
        int difficultDegree;
        Gamer gamer;
        Examer examer;
        qDebug() << "已收到" << func;
        switch(func)
        {
            case GAMER_SIGNIN:
                username = receivedInfoObject.take("username").toString();
                password = receivedInfoObject.take("password").toString();
                gamerSignin(i, username, password);
                break;
            case EXAMER_SIGNIN:
                username = receivedInfoObject.take("username").toString();
                password = receivedInfoObject.take("password").toString();
                examerSignin(i, username, password);
                break;
            case GAMER_SIGNUP:
                nickname = receivedInfoObject.take("nickname").toString();
                username = receivedInfoObject.take("username").toString();
                password = receivedInfoObject.take("password").toString();
                gamerSignup(i, nickname, username, password);
                break;
            case EXAMER_SIGNUP:
                nickname = receivedInfoObject.take("nickname").toString();
                username = receivedInfoObject.take("username").toString();
                password = receivedInfoObject.take("password").toString();
                examerSignup(i, nickname, username, password);
                break;
            case GAMER_SIGNOUT:
                username = receivedInfoObject.take("string").toString();
                qDebug() << username << "下线";
                gamerSignout(i, username);
                break;
            case EXAMER_SIGNOUT:
                username = receivedInfoObject.take("string").toString();
                examerSignout(i, username);
                break;
            case GET_GAMERINFO:
                username = receivedInfoObject.take("string").toString();
                getGamerInfo(i, username);
                break;
            case GET_EXAMERINFO:
                username = receivedInfoObject.take("string").toString();
                getExamerInfo(i, username);
                break;
            case ADDWORD:
                word = receivedInfoObject.take("string").toString();
                addWord(i, word);
                break;
            case GETWORD:
                difficultDegree = receivedInfoObject.take("difficulty").toInt();
                getWord(i, difficultDegree);
                break;
            case GET_GAMERRANKLIST:
                qDebug() << "收到获取玩家列表的请求";
                getGamerRanklist(i);
                break;
            case GET_EXAMERRANKLIST:
                getExamerRanklist(i);
                break;
            case UPDATE_GAMERINFO:
                gamer.nickname = receivedInfoObject.take("nickname").toString();
                gamer.username = receivedInfoObject.take("username").toString();
                gamer.level = receivedInfoObject.take("level").toInt();
                gamer.exp = receivedInfoObject.take("exp").toInt();
                gamer.passedStage = receivedInfoObject.take("passedStage").toInt();
                updateGamerInfo(i, gamer);
                break;
            case UPDATE_EXAMERINFO:
                qDebug() << "更新出题人信息";
                examer.nickname = receivedInfoObject.take("nickname").toString();
                examer.username = receivedInfoObject.take("username").toString();
                examer.level = receivedInfoObject.take("level").toInt();
                examer.exp = receivedInfoObject.take("exp").toInt();
                examer.questionNum = receivedInfoObject.take("questionNum").toInt();
                updateExamerInfo(i, examer);
                break;
        }
    }
}

void Server::gamerSignup(int clientNum, QString nickname, QString username, QString password)
{
    Database db;
    qDebug() << nickname << "即将存入";
    bool success = db.gamerSignup(nickname, username, password);
    QJsonObject infoObject;
    infoObject.insert("function", GAMER_SIGNUP);
    infoObject.insert("success", success);

    sendInfo(infoObject, clientNum);
}

void Server::gamerSignin(int clientNum, QString username, QString password)
{
    Database db;
    STATE state = db.gamerSignin(username, password);
    QJsonObject infoObject;
    infoObject.insert("function", GAMER_SIGNIN);
    infoObject.insert("state", state);

    sendInfo(infoObject, clientNum);
}

void Server::examerSignup(int clientNum, QString nickname, QString username, QString password)
{
    Database db;
    bool success = db.examerSignup(nickname, username, password);
    QJsonObject infoObject;
    infoObject.insert("function", EXAMER_SIGNUP);
    infoObject.insert("success", success);

    sendInfo(infoObject, clientNum);
}

void Server::examerSignin(int clientNum, QString username, QString password)
{
    Database db;
    STATE state = db.examerSignin(username, password);
    QJsonObject infoObject;
    infoObject.insert("function", EXAMER_SIGNIN);
    infoObject.insert("state", state);

    sendInfo(infoObject, clientNum);
}

void Server::getGamerInfo(int clientNum, QString username)
{
    Database db;
    Gamer gamer = db.getGamerInfo(username);
    QJsonObject infoObject;
    infoObject.insert("function", GET_GAMERINFO);
    infoObject.insert("nickname", gamer.nickname);
    infoObject.insert("username", gamer.username);
    infoObject.insert("level", gamer.level);
    infoObject.insert("exp", gamer.exp);
    infoObject.insert("passedStage", gamer.passedStage);

    sendInfo(infoObject, clientNum);
}

void Server::getExamerInfo(int clientNum, QString username)
{
    Database db;
    Examer examer = db.getExamerInfo(username);
    QJsonObject infoObject;
    infoObject.insert("function", GET_EXAMERINFO);
    infoObject.insert("nickname", examer.nickname);
    infoObject.insert("username", examer.username);
    infoObject.insert("level", examer.level);
    infoObject.insert("exp", examer.exp);
    infoObject.insert("questionNum", examer.questionNum);

    sendInfo(infoObject, clientNum);
}

void Server::addWord(int clientNum, QString word)
{
    Database db;
    bool success = db.addWord(word);
    QJsonObject infoObject;
    infoObject.insert("function", ADDWORD);
    infoObject.insert("success", success);

    sendInfo(infoObject, clientNum);
}

void Server::getWord(int clientNum, int difficultDegree)
{
    Database db;
    QString word = db.getWord(difficultDegree);
    QJsonObject infoObject;
    infoObject.insert("function", GETWORD);
    infoObject.insert("word", word);
    qDebug() << "服务器单词" << word;
    sendInfo(infoObject, clientNum);
}

void Server::gamerSignout(int clientNum, QString username)
{
    Database db;
    db.gamerSignout(username);
    QJsonObject infoObject;
    infoObject.insert("function", GAMER_SIGNOUT);

    sendInfo(infoObject, clientNum);
}

void Server::examerSignout(int clientNum, QString username)
{
    Database db;
    db.examerSignout(username);
    QJsonObject infoObject;
    infoObject.insert("function", EXAMER_SIGNOUT);

    sendInfo(infoObject, clientNum);
}

void Server::updateGamerInfo(int clientNum, Gamer gamer)
{
    Database db;
    db.updateInfo(gamer);

    QJsonObject infoObject;
    infoObject.insert("function", UPDATE_GAMERINFO);

    sendInfo(infoObject, clientNum);
}

void Server::updateExamerInfo(int clientNum, Examer examer)
{
    Database db;
    db.updateInfo(examer);

    QJsonObject infoObject;
    infoObject.insert("function", UPDATE_EXAMERINFO);

    sendInfo(infoObject, clientNum);
}

void Server::getGamerRanklist(int clientNum)
{
    Database db;
    QVector<Gamer> gamer = db.getGamerRanklist();

    QJsonObject infoObject;
    QJsonArray infoArray;

    for (auto it = gamer.begin(); it != gamer.end(); it++)
    {
        qDebug() << "正在获取玩家列表";
        infoObject.insert("function", GET_GAMERRANKLIST);
        infoObject.insert("nickname", it->nickname);
        infoObject.insert("username", it->username);
        infoObject.insert("level", it->level);
        infoObject.insert("exp", it->exp);
        infoObject.insert("passedStage", it->passedStage);

        infoArray.append(infoObject);

        infoObject.remove("function");
        infoObject.remove("nickname");
        infoObject.remove("username");
        infoObject.remove("level");
        infoObject.remove("exp");
        infoObject.remove("passedStage");
    }

    QJsonDocument infoDocument;
    infoDocument.setArray(infoArray);

    QByteArray infoByteArray;
    infoByteArray = infoDocument.toJson(QJsonDocument::Compact);

    client[clientNum]->write(infoByteArray);
}

void Server::getExamerRanklist(int clientNum)
{
    Database db;
    QVector<Examer> examer = db.getExamerRanklist();

    QJsonObject infoObject;
    QJsonArray infoArray;

    for (auto it = examer.begin(); it != examer.end(); it++)
    {
        infoObject.insert("function", GET_EXAMERRANKLIST);
        infoObject.insert("nickname", it->nickname);
        infoObject.insert("username", it->username);
        infoObject.insert("level", it->level);
        infoObject.insert("exp", it->exp);
        infoObject.insert("questionNum", it->questionNum);

        infoArray.append(infoObject);

        infoObject.remove("function");
        infoObject.remove("nickname");
        infoObject.remove("username");
        infoObject.remove("level");
        infoObject.remove("exp");
        infoObject.remove("questionNum");
    }

    QJsonDocument infoDocument;
    infoDocument.setArray(infoArray);

    QByteArray infoByteArray;
    infoByteArray = infoDocument.toJson(QJsonDocument::Compact);

    client[clientNum]->write(infoByteArray);
}

void Server::sendInfo(QJsonObject infoObject, int clientNum)
{
    QJsonDocument infoDocument;
    infoDocument.setObject(infoObject);

    QByteArray infoByteArray;
    infoByteArray = infoDocument.toJson(QJsonDocument::Compact);

    client[clientNum]->write(infoByteArray);
}

void Server::on_startButton_clicked()
{
   QMessageBox::information(this, tr("提示信息"), tr("服务器启动成功！"), QMessageBox::Ok);
   if(!server->listen(QHostAddress::Any, 8000))
   {
       qDebug() << server->errorString();
       server->close();
   }
   startButton->setEnabled(false);
   endButton->setEnabled(true);
}

void Server::on_endButton_clicked()
{
    for(int i = 0; i < client.length(); i++)
    {
        client[i]->disconnectFromHost();
        client.removeAt(i);
    }
    server->close();
    endButton->setEnabled(false);
    startButton->setEnabled(true);
}

void Server::displayError(QAbstractSocket::SocketError)
{
    qDebug() << infoReceiver->errorString();
    infoReceiver->close();
}

void Server::disconnection()
{
    for(int i = 0; i < client.length(); i++)
    {
        if(client[i]->state() == QAbstractSocket::UnconnectedState)
        {
            clientComboBox->removeItem(clientComboBox->findText(tr("%1:%2")\
                                       .arg(client[i]->peerAddress().toString().split("::ffff:")[1])\
                                       .arg(client[i]->peerPort())));
            client[i]->destroyed();
            client.removeAt(i);
        }
    }
}
