#pragma once

#include <QMainWindow>
#include <QComboBox>
#include <QListWidget>
#include <QLineEdit>
#include <QCheckBox>
#include <QEvent>
#include <QTimer>

class   MultiSelectComboBox : public QComboBox
{
    Q_OBJECT

public:
    MultiSelectComboBox(QWidget* aParent = nullptr) :
        QComboBox(aParent),
        mListWidget(new QListWidget(this)),
        mLineEdit(new QLineEdit(this))
    {
        mLineEdit->setReadOnly(true);
        mLineEdit->installEventFilter(this);

        QStringList items = { "-", "Apple", "Banana", "Orane", "Peach" };
        addItems(items);
        setCurrentIndex(0); // "-"
        mLineEdit->setPlaceholderText("-");

        //setStyleSheet("QComboBox {background: white;}"
        //              "{ border:none; }"
        //              "{ padding: 2px 2px 2px 2px; }"   // 上右下左
        //              );
        setModel(mListWidget->model());
        setView(mListWidget);
        setLineEdit(mLineEdit);

        // チェックボックスとその文字以外の部分がクリックされた場合に実行される
        connect(this, static_cast<void (QComboBox::*)(int)>(&QComboBox::activated), this, [=](int aIndex)   // void itemClicked(int aIndex);
        {
            // 何かが選択されたときの処理
            // 例："-"以外のチェックをすべて外す、または、1つ以上チェックがあれば"-"の項目チェックを外す
            if (aIndex == 0) { // "-"
                ResetSelection();
            }
            else {  // "-"以外
                // チェックボックスとその文字以外の部分がクリックされた場合は自動的にチェックが変化しないので、ここで処理する。
                QWidget* widget = mListWidget->itemWidget(mListWidget->item(aIndex));
                QCheckBox *checkBox = static_cast<QCheckBox *>(widget);
                bool checked = !checkBox->isChecked();
                checkBox->setChecked(checked);
                checkBox->setCheckState(checked ? Qt::Checked : Qt::Unchecked);
            }
        });
    }

    void addItem(const QString& aText, const QVariant& aUserData = QVariant())
    {
        Q_UNUSED(aUserData);
        QListWidgetItem* listWidgetItem = new QListWidgetItem(mListWidget);
        QCheckBox* checkBox = new QCheckBox(this);
        checkBox->setText(aText);
        mListWidget->addItem(listWidgetItem);
        mListWidget->setItemWidget(listWidgetItem, checkBox);

        // チェックボックスかその文字の部分がクリックされた場合に実行される
        connect(checkBox, &QCheckBox::stateChanged, this, [=](int aState)   // void stateChanged(int aState);
        {
            QWidget* widget;
            QCheckBox *checkBox;
            if ((aState == Qt::Checked) && (aText == "-")) {
                for (int i = 1; i < mListWidget->count(); ++i) {
                    widget = mListWidget->itemWidget(mListWidget->item(i));
                    checkBox = static_cast<QCheckBox*>(widget);
                    checkBox->blockSignals(true);
                    checkBox->setChecked(false);
                    checkBox->setCheckState(Qt::Unchecked);
                    checkBox->blockSignals(false);
                }
            }
            else {
                bool checked = true;
                // "-"以外に1つ以上チェックがあれば"-"の項目チェックを外す
                for (int i = 1; i < mListWidget->count(); ++i) {
                    widget = mListWidget->itemWidget(mListWidget->item(i));
                    checkBox = static_cast<QCheckBox*>(widget);
                    if (checkBox->checkState() == Qt::Checked) {
                        checked = false;
                        break;
                    }
                }
                widget = mListWidget->itemWidget(mListWidget->item(0));
                checkBox = static_cast<QCheckBox*>(widget);
                checkBox->blockSignals(true);
                checkBox->setChecked(checked);
                checkBox->setCheckState(checked ? Qt::Checked : Qt::Unchecked);
                checkBox->blockSignals(false);
            }

            QString selectedData("");
            for (int i = 1; i < mListWidget->count(); ++i) {
                widget = mListWidget->itemWidget(mListWidget->item(i));
                checkBox = static_cast<QCheckBox *>(widget);
                if (checkBox->isChecked()) {
                    selectedData.append(checkBox->text()).append(",");
                }
            }
            if (selectedData.endsWith(",")) {   // 行末の","を除去
                selectedData.remove(selectedData.length() - 1, 1);
            }
            if (selectedData.isEmpty()) {
                selectedData.append("-");
            }
            mLineEdit->setText(selectedData);
            mLineEdit->setToolTip(selectedData);
            emit selectionChanged();
        });
    }
    void addItems(const QStringList& aTexts)
    {
        for(const auto& string : aTexts) {
            addItem(string);
        }
    }

    QList<int> currentIndex() {
        QList<int> indexList;
        for (int i = 0; i < mListWidget->count(); ++i) {
            QWidget* widget = mListWidget->itemWidget(mListWidget->item(i));
            QCheckBox* checkBox = static_cast<QCheckBox*>(widget);
            if (checkBox->isChecked()) {
                indexList.append(i);
            }
        }
        return indexList;
    }
    void setCurrentIndex(QList<int> indexList) {
        for (int i = 0; i < mListWidget->count(); ++i) {
            QWidget* widget = mListWidget->itemWidget(mListWidget->item(i));
            QCheckBox* checkBox = static_cast<QCheckBox*>(widget);
            bool checked = false;
            if(indexList.contains(i)) {
                checked = true;
            }
            checkBox->setChecked(checked);
            checkBox->setCheckState(checked ? Qt::Checked : Qt::Unchecked);
        }
    }
    void setCurrentIndex(int index) {
        QList<int> indexList;
        indexList.append(index);
        setCurrentIndex(indexList);
    }

    QStringList currentText()
    {
        QStringList emptyStringList;
        if(!mLineEdit->text().isEmpty()) {
            emptyStringList = mLineEdit->text().split(',');
        }
        return emptyStringList;
    }
    void setCurrentText(const QString& aText) { Q_UNUSED(aText); }
    void setCurrentText(const QStringList& aText)
    {
        for (int i = 1; i < mListWidget->count(); ++i) {
            QWidget* widget = mListWidget->itemWidget(mListWidget->item(i));
            QCheckBox* checkBox = static_cast<QCheckBox*>(widget);
            QString checkBoxString = checkBox->text();
            if(aText.contains(checkBoxString)) {
                checkBox->setChecked(true);
                checkBox->setCheckState(Qt::Checked);
            }
        }
    }

signals:
    void selectionChanged();
    void closedPopup();


protected:
    void wheelEvent(QWheelEvent* aWheelEvent) override { Q_UNUSED(aWheelEvent); } // Do not handle the wheel event
    bool eventFilter(QObject* aObject, QEvent* aEvent) override
    {
        if(aObject == mLineEdit && aEvent->type() == QEvent::MouseButtonRelease) {
            showPopup();
            return false;
        }
        return false;
    }
    void keyPressEvent(QKeyEvent* aEvent) override { Q_UNUSED(aEvent); } // Do not handle key event


private:
    QListWidget* mListWidget;
    QLineEdit* mLineEdit;

    void ResetSelection()
    {
        // いったんすべてのチェックを外す
        for (int i = 0; i < mListWidget->count(); ++i) {
            QWidget* widget = mListWidget->itemWidget(mListWidget->item(i));
            QCheckBox* checkBox = static_cast<QCheckBox*>(widget);
            checkBox->blockSignals(true);
            checkBox->setChecked(false);
            checkBox->setCheckState(Qt::Unchecked);
            checkBox->blockSignals(false);
        }
        // "-"のチェックを付ける
        QWidget *widget = mListWidget->itemWidget(mListWidget->item(0));
        QCheckBox *checkBox = static_cast<QCheckBox *>(widget);
        checkBox->setChecked(true);
        checkBox->setCheckState(Qt::Checked);
        // ここで stateChanged が呼ばれる
    }

    void hidePopup() override
    {
        int width = this->width();
        int height = mListWidget->height();
        int x = QCursor::pos().x() - mapToGlobal(geometry().topLeft()).x() + geometry().x();
        int y = QCursor::pos().y() - mapToGlobal(geometry().topLeft()).y() + geometry().y();
        if (x >= 0 && x <= width && y >= this->height() && y <= height + this->height()) {
            // Item was clicked, do not hide popup
        }
        else {
            bool enabled = QComboBox::isEnabled();
            if (!enabled) {
                // enabledがtrueからfalseになる時、無駄にイベント発生するので除外する
                return;
            }
            // コンボボックスが閉じる時の通知
            emit closedPopup();

            // コンボボックスを閉じる
            QComboBox::hidePopup();

            QTimer::singleShot(0, this, [=]() {
                // ADD コンボボックスが閉じた後の処理
            });
        }
    }
};
