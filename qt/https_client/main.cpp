
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

    QNetworkRequest getReq(QUrl("https://127.0.0.1:8443/get"));
    QSslConfiguration config = QSslConfiguration::defaultConfiguration();
    config.setProtocol(QSsl::TlsV1_2);
//    config.setPeerVerifyMode(QSslSocket::VerifyNone);

    config.setPeerVerifyMode(QSslSocket::QueryPeer);

    QFile file(":/ssl/ca.pem");
    file.open(QIODevice::ReadOnly);
    const QByteArray bytes = file.readAll();
    const QSslCertificate certificate(bytes);
    config.setCaCertificates({certificate});
    
    QFile file1(":/ssl/client.pem");
    file1.open(QIODevice::ReadOnly);
    const QByteArray bytes1 = file1.readAll();
    const QSslCertificate certificate1(bytes1);
    config.setLocalCertificate(certificate1);

    QFile file2(":/ssl/client.key");
    file2.open(QIODevice::ReadOnly);
    const QByteArray bytes2 = file2.readAll();
    const QSslKey privateKey(bytes2,QSsl::Rsa);
    config.setPrivateKey(privateKey);

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


    QCoreApplication::connect(getReply, &QNetworkReply::encrypted, [getReply](){
	QList<QSslCertificate> peerCerts = getReply->sslConfiguration().peerCertificateChain();
	for(int i=0; i < peerCerts.length(); i++) {
	    QByteArray d = peerCerts[i].digest(QCryptographicHash::Sha1);
	    for(int i=0; i < d.length(); i++) {
		printf("%02x:",d.at(i) & 0xff);
	    }
	    printf("\n");
	}
    });


    return app.exec();
}
