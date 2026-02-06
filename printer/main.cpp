#include <QApplication>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QComboBox>
#include <QFileDialog>
#include <QPrinter>
#include <QPainter>
#include <QRandomGenerator>
#include <QPrintDialog>
#include "QRCode/QrCodeGeneratorDemo.h"
#include <QTextStream>
#include <QGraphicsScene>

void QRCode(QString text2) {


    QString maxqrstr;
    for (int i=0 ;i < 2000 ; i++){ //4296 / 8 per ascii  537
        maxqrstr.append("i");
    }

 //   qDebug () << maxqrstr.toUtf8().size();
 //       qDebug () << maxqrstr.toLatin1().size();
         //   qDebug () << maxqrstr.toWCharArray().size();

std::wstring text ( text2.toStdWString() );
    //char *text2 = text.c_str();
    const wchar_t* wstr = text.c_str() ;
    char mbstr[4000];
    std::wcstombs(mbstr, wstr, 4000);

    const QrCode::Ecc errCorLvl = QrCode::Ecc::LOW;  // Error correction level
    const QrCode qr = QrCode::encodeText( mbstr , errCorLvl);

 //   ofstream write;
   // std::string   filename = "tmp.svg";
//    write.open(filename.c_str(), ios::out | ios::binary);
 //   write << qr.toSvgString(4);


    QString filename = QApplication::applicationDirPath() +"/tmp.svg";
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
    //    qWarning() << "Failed to open file for writing";
        return;
    }
    QTextStream out(&file);
    out << QString::fromStdString(qr.toSvgString(4));
    file.close();

   // QImage *img_object = new QImage();
   // img_object->load("./tmp.svg");
   // QPixmap image = QPixmap::fromImage(*img_object);
 //   QPixmap scaled_img = image.scaled(this->width(), this->height(), Qt::KeepAspectRatio);
  //  QPixmap scaled_img = image.scaled(ui->graphicsView->width(), ui->graphicsView->height(), Qt::KeepAspectRatio);
   // QGraphicsScene *scene= new QGraphicsScene();
   // scene->addItem(new QGraphicsSvgItem("./tmp.svg"));
    //scene->addPixmap(scaled_img);
   // scene->setSceneRect(scaled_img.rect());
  //  ui->graphicsView->setScene(scene);
  //  ui->graphicsView->show();

}


QString randomSerial()
{
    const QString chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    QString s = "FAKE";
    for (int i = 0; i < 10; ++i)
        s += chars.at(QRandomGenerator::global()->bounded(chars.size()));
    return s;
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QWidget window;
    window.setWindowTitle("Sheet Image Printer (Demo)");
    window.setFixedSize(420, 240);

    QVBoxLayout *layout = new QVBoxLayout(&window);

    QPushButton *loadFrontBtn = new QPushButton("Load Front Image");
    QPushButton *loadBackBtn  = new QPushButton("Load Back Image");

    QLabel *frontLabel = new QLabel("Front: not loaded");
    QLabel *backLabel  = new QLabel("Back: not loaded");

    QComboBox *gridBox = new QComboBox;
    gridBox->addItem("2 x 3 (6 per sheet)", QVariant::fromValue(QSize(2,3)));
    gridBox->addItem("3 x 3 (9 per sheet)", QVariant::fromValue(QSize(3,3)));

    QPushButton *printBtn = new QPushButton("Print Sheet");

    layout->addWidget(loadFrontBtn);
    layout->addWidget(frontLabel);
    layout->addWidget(loadBackBtn);
    layout->addWidget(backLabel);
    layout->addWidget(new QLabel("Layout:"));
    layout->addWidget(gridBox);
    layout->addSpacing(10);
    layout->addWidget(printBtn);

    QImage frontImg, backImg;

    QObject::connect(loadFrontBtn, &QPushButton::clicked, [&](){
        QString path = QFileDialog::getOpenFileName(&window, "Load Front Image",QApplication::applicationDirPath());
        if (!path.isEmpty()) {
            frontImg.load(path);
            frontLabel->setText("Front: loaded");
        }
    });

    QObject::connect(loadBackBtn, &QPushButton::clicked, [&](){
        QString path = QFileDialog::getOpenFileName(&window, "Load Back Image",QApplication::applicationDirPath());
        if (!path.isEmpty()) {
            backImg.load(path);
            backLabel->setText("Back: loaded");
        }
    });

    QObject::connect(printBtn, &QPushButton::clicked, [&](){
        if (frontImg.isNull() || backImg.isNull())
            return;

        QPrinter printer(QPrinter::HighResolution);
        printer.setPageSize(QPageSize(QPageSize::Letter));
        printer.setOrientation(QPrinter::Landscape);
        printer.setOutputFormat(QPrinter::PdfFormat);
        printer.setOutputFileName(QApplication::applicationDirPath() + "/output.pdf");


        QPainter painter(&printer);


        QSize grid = gridBox->currentData().toSize();
        int cols = grid.width();
        int rows = grid.height();

        QRect page = printer.pageRect();
        int cellW = page.width() / cols;
        int cellH = page.height() / rows;




        // ---------- FRONT ----------
        for (int y = 0; y < rows; ++y) {
            for (int x = 0; x < cols; ++x) {
                QRect cell(x * cellW, y * cellH, cellW, cellH);

                QSize imgSize = frontImg.size();
                imgSize.scale(cell.size(), Qt::KeepAspectRatio);

                QRect imgRect(
                    cell.center() - QPoint(imgSize.width()/2, imgSize.height()/2),
                    imgSize
                );

                QRect imgRect2(
                    cell.center() - QPoint(imgSize.width()/3, imgSize.height()/2),
                    imgSize
                );

                QString serial = randomSerial();
                QRCode(serial);
                QImage overlayImg;
                overlayImg.load(QApplication::applicationDirPath() + "/tmp.svg");

                painter.drawImage(cell, frontImg.scaled(cell.size(), Qt::KeepAspectRatio));


                painter.save();
                painter.setOpacity(0.35);
                painter.drawImage(imgRect, overlayImg);
                painter.restore();

              //  painter.setPen(Qt::Black);
                painter.drawText(imgRect2.adjusted(10,10,-10,-10),
                                 Qt::AlignTop,
                                 randomSerial());
            }
        }

        printer.newPage();

        // ---------- BACK ----------
        for (int y = 0; y < rows; ++y) {
            for (int x = 0; x < cols; ++x) {
                QRect cell(x * cellW, y * cellH, cellW, cellH);

                painter.drawImage(cell, backImg.scaled(cell.size(), Qt::KeepAspectRatio));

                painter.setPen(Qt::red);
                painter.drawText(cell.adjusted(10,10,-10,-10),
                                 Qt::AlignBottom | Qt::AlignRight,
                                 randomSerial());
            }
        }

        painter.end();
    });

    window.show();
    return app.exec();
}

