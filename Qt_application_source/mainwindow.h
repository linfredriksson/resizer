#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "QFileDialog"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void generateImages();
    void handleInputBrowseButton();
    void handleOutputBrowseButton();
    void handlePrefixText();
    void handleSuffixText();
    void loggSizeSetting();
    void loggInterpolationSetting();
    void logg(QString text);
    QFileInfoList getInputFileList(QString filePath);
    void listImageFiles(QString filePath);

private:
    Ui::MainWindow *ui;
    QString inputDirectory;
    QString outputDirectory;
    QString prefix;
    QString suffix;
};

#endif // MAINWINDOW_H
