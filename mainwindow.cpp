#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include "MultiSelectComboBox.h"

#include <QPalette>
#include <QColor>
#include <QTableWidget>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // 背景色
    QPalette pal = palette();
    QColor color = QColor(255, 255, 192, 255);
    pal.setColor(QPalette::Window, color);  // QPallete::Window -> background
    this->setAutoFillBackground(true);
    this->setPalette(pal);
    this->show();

    // アイテム追加
    ui->multiSelectComboBox->addItem("Kiwi");
    ui->multiSelectComboBox->addItem("Grape");
    ui->multiSelectComboBox->addItem("Mango");
    ui->multiSelectComboBox->setFixedWidth(100);

    QTableWidget *t = ui->tableWidget;
    int column = 1;
    // Qt Designerで作った表の2列目各セルに、コンボボックスを設定する
    for (int row = 0; row < t->rowCount(); row++) {
        QComboBox *comboBox = new QComboBox();
        comboBox->addItem("-");
        comboBox->addItem("Kiwi");
        comboBox->addItem("Grape");
        comboBox->addItem("Mango");
        t->setCellWidget(row, column, comboBox);
        // コンボボックスが閉じる時の処理
        connect(comboBox, &QComboBox::currentIndexChanged, this, [=](int index) {
            QString text = comboBox->currentText();
            qDebug() << Q_FUNC_INFO << "row:" << row << "column:" << column << index << text;
        });
    }
    // Qt Designerで作った表の3列目各セルに、複数選択対応のコンボボックスを設定する
    column = 2; // 3列目に設定する
    for (int row = 0; row < t->rowCount(); row++) {
        MultiSelectComboBox *comboBox = new MultiSelectComboBox();
        t->setCellWidget(row, column, comboBox);
        // コンボボックスが閉じる時の処理
        connect(comboBox, &MultiSelectComboBox::closedPopup, this, [=]() {
            // 現在チェックされているインデックスのリストを取得
            QList<int> indexList = comboBox->currentIndex();
            QStringList textList = comboBox->currentText();
            qDebug() << Q_FUNC_INFO << "row:" << row << "column:" << column << indexList << textList;
            // ADD 幅の調整とか
        });
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}
