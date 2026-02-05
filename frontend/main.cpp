#include <QApplication>
#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QProcess>
#include <QFont>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <qdebug.h>
#include <QLineEdit>

#include "dogecoin/libdogecoin.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QWidget window;
    window.setWindowTitle("libdogecoin Key Generator");
    window.setFixedSize(520, 240);

    QVBoxLayout *layout = new QVBoxLayout(&window);

    QLabel *title = new QLabel("libdogecoin Keypair Generator");
    QFont titleFont;
    titleFont.setPointSize(14);
    titleFont.setBold(true);
    title->setFont(titleFont);
    title->setAlignment(Qt::AlignCenter);

    QLineEdit *publicLabel = new QLineEdit("Public Key:\n—");
    QLabel *publicLabel2 = new QLabel("Public Key:\n—");
    //publicLabel->setWordWrap(true);

    QLineEdit *privateLabel = new QLineEdit("Private Key:\n—");
    QLabel *privateLabel2 = new QLabel("Private Key:\n—");
    //privateLabel->setWordWrap(true);

    QPushButton *generateBtn = new QPushButton("Generate Keypair");

    layout->addWidget(title);
    layout->addSpacing(10);
    layout->addWidget(publicLabel);
    layout->addWidget(privateLabel);
    layout->addSpacing(10);
    layout->addWidget(generateBtn);

    QObject::connect(generateBtn, &QPushButton::clicked, [&]() {
        QProcess process;
        process.start(QApplication::applicationDirPath() + "/test", QStringList() << "generatePrivPubKeypair");

        if (!process.waitForFinished()) {
            publicLabel->setText("Public Key:\nERROR");
            privateLabel->setText("Private Key:\nERROR");
            return;
        }


        QString output = process.readAllStandardOutput();

        QJsonParseError err;
        QJsonDocument doc = QJsonDocument::fromJson(output.toUtf8(), &err);

        if (err.error != QJsonParseError::NoError || !doc.isObject()) {
            qWarning() << "Invalid JSON:" << err.errorString();
            return;
        }

        QJsonObject obj = doc.object();

        QString privateKey = obj.value("private").toString().trimmed();
        QString publicKey  = obj.value("public").toString().trimmed();



      //  for (const QString &line : output.split('\n')) {
     //       if (line.startsWith("Public Key:"))
      //          publicKey = line.section(':', 1).trimmed();
      //      else if (line.startsWith("Private Key:"))
      //          privateKey = line.section(':', 1).trimmed();
      //  }

//         dogecoin_ecc_start();



       // publicLabel->setText("Public Key:\n" + publicKey);
       // privateLabel->setText("Private Key:\n" + privateKey);

        publicLabel->setText( publicKey);
        privateLabel->setText( privateKey);
    });

    window.show();
    return app.exec();
}
