#ifndef CONFIGURE
#define CONFIGURE

#include <QFile>
#include <QIODevice>
#include <QMap>
#include <QJsonDocument>
#include <QDebug>

bool writeConfigure()
{
    QMap<QString, QVariant> IsShowHelp;

    QVariant variant("true");
    IsShowHelp.insert("IsShowHelp", variant);

    QVariant result(IsShowHelp);

    QJsonDocument JsonDocument = QJsonDocument::fromVariant(result);
    QFile file("../Unicursal/configure");
    if (file.open(QIODevice::WriteOnly)){
        if (!JsonDocument.isNull()){
            QTextStream in(&file);
            in << JsonDocument.toJson();

            return true;
        }
        return false;
    }else{
        return false;
    }
}

bool modifyConfigure()
{
    QMap<QString, QVariant> IsShowHelp;
    QVariant variant("false");
    IsShowHelp.insert("IsShowHelp", variant);

    QVariant result(IsShowHelp);
    QJsonDocument JsonDocument = QJsonDocument::fromVariant(result);

    QFile file("../Unicursal/configure");
    if (file.open(QIODevice::WriteOnly)){
        QTextStream in(&file);

        // Clear file
        in << "";
        if (!JsonDocument.isNull()){
            in << JsonDocument.toJson();
        }
        return true;
    }else{
        return false;
    }
}

bool readConfigure()
{
    QFile file("../Unicursal/configure");
    if (file.open(QIODevice::ReadOnly)){
        QTextStream in(&file);

        QString str = in.readAll();
        QJsonParseError error;
        QJsonDocument JsonDocument = QJsonDocument::fromJson(str.toUtf8(), &error);
        if (error.error == QJsonParseError::NoError){
            if (JsonDocument.isObject()){
                QVariantMap result = JsonDocument.toVariant().toMap();
                if (result["IsShowHelp"].toString() == "true"){
                    qDebug() << "true";
                }else{
                    qDebug() << "false";
                }
            }
        }else{
            return false;
        }
        return true;
    }else{
        return false;
    }
}

#endif // CONFIGURE

