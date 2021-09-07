
#include <QCoreApplication>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include <QDebug>
#include <QSslConfiguration>
#include <QFile>
#include <QSslKey>
#include <QCryptographicHash>

#include <iostream>
#include <string>


int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    QNetworkAccessManager nam;

    QNetworkRequest getReq(QUrl("https://url"));

    QString username = "user";
    QString password = "pwd";
    QString concatenated = username + ":" + password;
    QByteArray data = concatenated.toLocal8Bit().toBase64();
    QString headerData = "Basic " + data;
    getReq.setRawHeader("Authorization", headerData.toLocal8Bit());

    QSslConfiguration config = QSslConfiguration::defaultConfiguration();
    config.setProtocol(QSsl::TlsV1_2);
    getReq.setSslConfiguration(config);

    QNetworkReply *getReply = nam.get(getReq);

    QCoreApplication::connect(getReply, &QNetworkReply::finished, [&]() {
	if (getReply->error()) {
            qDebug() << getReply->errorString();
            return;
        }
        QString answer = getReply->readAll();
        qDebug() << answer;

	app.quit();
    });


    return app.exec();
}
