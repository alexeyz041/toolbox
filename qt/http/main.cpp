
#include <QCoreApplication>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include <QDebug>

#include <iostream>
#include <string>


int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    QNetworkAccessManager nam;
    QNetworkRequest getReq(QUrl("http://localhost:8080/get"));
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
