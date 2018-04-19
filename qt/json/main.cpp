
#include <QFile>
#include <QVariantMap>
#include <QByteArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QTextStream>
#include <QDebug>

#include <iostream>
#include <string>



bool readJsonFile(std::string file_path, QVariantMap& result)
{
    // step 1
    QFile file_obj(QString::fromStdString(file_path));
    if (!file_obj.open(QIODevice::ReadOnly)) {
	std::cout << "Failed to open " << file_path << std::endl;
	exit(1);
    }

    // step 2
    QTextStream file_text(&file_obj);
    QString json_string;
    json_string = file_text.readAll();
    file_obj.close();
    QByteArray json_bytes = json_string.toLocal8Bit();

    // step 3
    auto json_doc = QJsonDocument::fromJson(json_bytes);

    if (json_doc.isNull()) {
        std::cout << "Failed to create JSON doc." << std::endl;
        return false;
    }
    if (!json_doc.isObject()) {
        std::cout << "JSON is not an object." << std::endl;
        return false;
    }

    QJsonObject json_obj = json_doc.object();

    if (json_obj.isEmpty()) {
        std::cout << "JSON object is empty." << std::endl;
        return false;
    }

    // step 4
    result = json_obj.toVariantMap();
    return true;
}


// writes a QVariantMap to disk
bool writeJsonFile(QVariantMap point_map, std::string file_path)
{
    QJsonObject json_obj = QJsonObject::fromVariantMap(point_map);
    QJsonDocument json_doc(json_obj);
    QString json_string = json_doc.toJson();

    QFile save_file(QString::fromStdString(file_path));
    if (!save_file.open(QIODevice::WriteOnly)) {
        std::cout << "failed to open save file" << std::endl;
        return false;
    }
    save_file.write(json_string.toLocal8Bit());
    save_file.close();
    return true;
}


int main(int argc, char *argv[])
{
    QVariantMap m;

    m["hello"] = "world";
    writeJsonFile(m,"m.json");

    QVariantMap m1;
    readJsonFile("m.json",m1);

    for(QVariantMap::const_iterator iter = m1.begin(); iter != m1.end(); ++iter) {
      qDebug() << iter.key() << iter.value();
    }

    std::cout << m1["hello"].toString().toLatin1().data() << "\n";

    return 0;
}
