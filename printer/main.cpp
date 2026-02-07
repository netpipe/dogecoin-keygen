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
#include <QDesktopServices>
#include <QUrl>
#include <QProcess>
#include <QFont>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <qdebug.h>
#include <QLineEdit>

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

QString privateKey ;
QString publicKey;

void GenerateKeys(QString test){
    if (test == "Dogecoin"){
        QProcess process;
        process.start(QApplication::applicationDirPath() + "/test", QStringList() << "generatePrivPubKeypair");

        if (!process.waitForFinished()) {
         //   publicLabel->setText("Public Key:\nERROR");
          //  privateLabel->setText("Private Key:\nERROR");
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

         privateKey = obj.value("private").toString().trimmed();
         publicKey  = obj.value("public").toString().trimmed();

    }else{
        QProcess process;
        process.start(QApplication::applicationDirPath() + "/php", QStringList() << "index.php");

        if (!process.waitForFinished()) {
           // publicLabel->setText("Public Key:\nERROR");
           // privateLabel->setText("Private Key:\nERROR");
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

         privateKey = obj.value("private").toString().trimmed();
         publicKey  = obj.value("public").toString().trimmed();
    }

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

    QComboBox *comboCoin = new QComboBox;
    comboCoin->addItem("Dogecoin");
    comboCoin->addItem("Bitcoin");

    layout->addWidget(loadFrontBtn);
    layout->addWidget(frontLabel);
    layout->addWidget(loadBackBtn);
    layout->addWidget(backLabel);
    layout->addWidget(new QLabel("Layout:"));
    layout->addWidget(gridBox);
    layout->addSpacing(10);
        layout->addWidget(comboCoin);
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




        // Generate Serials specifically for this batch
        // We need (rows * cols) serials.
        QVector<QString> pageSerials;
        QVector<QString> pageSerials2;
        int totalBills = rows * cols;
        for(int i=0; i<totalBills; ++i) {
            GenerateKeys( comboCoin->currentText() );
             pageSerials.append(publicKey);
             pageSerials2.append(privateKey);
        }

        // ---------- FRONT ----------
        for (int y = 0; y < rows; ++y) {
            for (int x = 0; x < cols; ++x) {
                QRect cell(x * cellW, y * cellH, cellW, cellH);

                QSize imgSize = frontImg.size();
                imgSize.scale(cell.size(), Qt::KeepAspectRatio);

                // Derived from Python Mockup (300 DPI @ 130.75mm x 58.13mm)
                // Canvas: 1544x686
                // QR X: 0.8316 * width
                // QR Y: 0.3105 * height
                // QR Size: 0.3790 * height
                // Text Y: 0.1200 * height (Top aligned)
                // Text X: Centered horizontally

                // --- QR CODE PLACEMENT ---
                int qrH = cell.height() * 0.38; // relative height
                int qrW = qrH; // Square
                int qrX = cell.x() + (cell.width() * 0.82); // Right alignedish
                int qrY = cell.y() + (cell.height() * 0.31); // Vertically centered roughly

                QRect imgRect(qrX, qrY, qrW, qrH);

                // --- TEXT PLACEMENT ---
                // Centered at the top, between the "20,000" corner marks (roughly).
                int textH = cell.height() * 0.15;
                int textW = cell.width(); // Full width to allow center alignment
                int textX = cell.x(); // Start at left
                int textY = cell.y() + (cell.height() * 0.12); // Top margin

                QRect imgRect2(textX, textY, textW, textH);

                // GET SERIAL FOR THIS CELL
                int index = y * cols + x;
                QString serial = pageSerials.value(index, "ERROR");

                // Generate QR SVG for this serial (Note: Inefficient to re-gen if duplicate, but safer)
                QRCode(serial);
                QImage overlayImg;
                overlayImg.load(QApplication::applicationDirPath() + "/tmp.svg");

                painter.drawImage(cell, frontImg.scaled(cell.size(), Qt::KeepAspectRatio));


                painter.save();
                painter.setOpacity(1.0); // Full opacity for QR to be scanneable
                painter.drawImage(imgRect, overlayImg);
                painter.restore();

                // Draw Serial
                QFont font = painter.font();
                font.setPixelSize(cell.height() * 0.06); // Smaller font (was 0.08)
                font.setBold(true); // Bold as requested
                painter.setFont(font);

                // Color match magenta/purple from ticket? Or standard black?
                // Request said "cleaner". Black or Dark Gray is safest unless specified.
                // Original code had "DN1..." in Magenta. Let's stick to a clean readable color.
                painter.setPen(QColor(200, 0, 200)); // Magenta-ish to match "DN1..." style if needed, or just black.
                                                     // Actually original code was commenting out setPen(Qt::Black).
                                                     // Let's use a dark distinct color.

                painter.drawText(imgRect2,
                                 Qt::AlignCenter, // Center in its box
                                 serial);
            }
        }

        printer.newPage();

        // ---------- BACK ----------
        for (int y = 0; y < rows; ++y) {
            for (int x = 0; x < cols; ++x) {
                QRect cell(x * cellW, y * cellH, cellW, cellH);

                painter.drawImage(cell, backImg.scaled(cell.size(), Qt::KeepAspectRatio));

                // MIRROR LOGIC:
                // Front(x, y) corresponds to Back(cols-1-x, y) physically when printed double-sided.
                // We want the same serial number on the back of the physical bill.
                int sourceCol = cols - 1 - x;
                int index = y * cols + sourceCol;
                QString serial = pageSerials2.value(index, "ERROR");

                // --- MIRRORED QR PLACEMENT ---
                // Front QR was at ~82% width.
                // Back QR should be at ~18% width (mirrored).
                // frontQrX_rel = 0.82
                // backQrX_rel = 1.0 - 0.82 - qrWidth_rel(0.38 * h / w) ... roughly
                // Let's use exact coordinate mirroring relative to cell.
                // Front: int qrX = cell.x() + (cell.width() * 0.82);
                // Back:  int qrX = cell.x() + (cell.width() - (cell.width() * 0.82) - qrW);

                int qrH = cell.height() * 0.48;
                int qrW = qrH;

                // Keep same Y
                int qrY = cell.y() + (cell.height() * 0.31);

                // Mirror X
                // Distance from Right Edge in Front = Distance from Left Edge in Back
                int qrX_Back = cell.x() + (cell.width() - (cell.width() * 0.82) - qrW);
                // Or simply: cell.x() + cell.width() * (1.0 - 0.82) - qrW ?
                // Let's use the explicit logic:
                // Front Offset R = cell.width() * 0.18 (approx, since 0.82 + w shouldn't overflow)
                // Actually in Front logic: X = 0.82 * W.
                // So Margin Right = W - (0.82*W + qrW).
                // In Back, Margin Left = Margin Right of Front.
                // X_Back = cell.x() + (cell.width() - (qrX_Front_Relative + qrW)) // No wait.
                // If Front is at Right, Back is at Left.
                // Let's use simple mirroring:
                // X' = Width - X - ElementWidth
                int frontRelX = cell.width() * 0.52;
                int mirroredRelX = cell.width() - frontRelX - qrW;
                int qrX = cell.x() + mirroredRelX;

                QRect imgRect(qrX, qrY, qrW, qrH);

                // --- TEXT PLACEMENT ---
                // Text is Centered. Mirror of Center is Center.
                int textH = cell.height() * 0.15;
                int textW = cell.width();
                int textX = cell.x();
                int textY = cell.y() + (cell.height() * 0.12);

                QRect imgRect2(textX, textY, textW, textH);

                // Generate QR (Should be same as front, cache if needed but this is fast enough)
                QRCode(serial);
                QImage overlayImg;
                overlayImg.load(QApplication::applicationDirPath() + "/tmp.svg");

                painter.save();
                painter.setOpacity(1.0);
                painter.drawImage(imgRect, overlayImg);
                painter.restore();

                QFont font = painter.font();
                font.setPixelSize(cell.height() * 0.06);
                font.setBold(true);
                painter.setFont(font);

                // Same Color
                painter.setPen(QColor(200, 0, 200));

                painter.drawText(imgRect2,
                                 Qt::AlignCenter,
                                 serial);
            }
        }

        painter.end();
        QDesktopServices::openUrl(QUrl::fromLocalFile(QApplication::applicationDirPath()+"/output.pdf"));
    });

    window.show();
    return app.exec();
}

