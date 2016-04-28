#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "resizer.h"

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->button_browse_input, SIGNAL (released()), this, SLOT (handleInputBrowseButton()));
    connect(ui->button_browse_output, SIGNAL (released()), this, SLOT (handleOutputBrowseButton()));
    connect(ui->prefixTextBox, SIGNAL(editingFinished()), this, SLOT(handlePrefixText()));
    connect(ui->suffixTextBox, SIGNAL(editingFinished()), this, SLOT (handleSuffixText()));
    connect(ui->radioButtonPixels, SIGNAL(clicked()), this, SLOT (loggSizeSetting()));
    connect(ui->radioButtonPercentage, SIGNAL(clicked()), this, SLOT (loggSizeSetting()));
    connect(ui->GenerateButton, SIGNAL(released()), this, SLOT (generateImages()));
    connect(ui->interpolationSelectionBox, SIGNAL(currentIndexChanged(int)), SLOT (loggInterpolationSetting()));
    inputDirectory = QCoreApplication::applicationDirPath();
    outputDirectory = inputDirectory;
    prefix = "";
    suffix = "";
    ui->input_box->setText(inputDirectory);
    ui->output_box->setText(outputDirectory);
    listImageFiles(inputDirectory);
    ui->progressBar->setRange(1, 100);
    ui->interpolationSelectionBox->addItem("Nearest Neighbour");
    ui->interpolationSelectionBox->addItem("Bicubic");
    ui->interpolationSelectionBox->addItem("Bilinear");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::generateImages()
{
    bool readyToGenerateImages = true;
    inputDirectory = ui->input_box->text();
    outputDirectory = ui->output_box->text();

    // make sure there is a input directory choosen
    if(inputDirectory.isNull() || inputDirectory.isEmpty() || !QDir(inputDirectory).exists())
    {
        readyToGenerateImages = false;
        logg("ERROR: invalid input directory...");
    }

    // make sure there is a output directory choosen
    if(outputDirectory.isNull() || outputDirectory.isEmpty() || !QDir(outputDirectory).exists())
    {
        readyToGenerateImages = false;
        logg("ERROR: invalid output directory...");
    }

    // make sure the original images will not be overwritten. Either by saving them in a different directory
    // or by adding a prefix/siffix to the file name
    if(outputDirectory == inputDirectory && (ui->prefixTextBox->text().isEmpty() || ui->suffixTextBox->text().isEmpty()))
    {
        readyToGenerateImages = false;
        logg("ERROR: change output directory or add a prefix/suffix to avoid overwriting original images...");
    }

    // if everything is ok, generate the scaled images
    if (readyToGenerateImages)
    {
        ui->progressBar->setValue(0);
        logg("Generating scaled images...");
        QFileInfoList fileList = getInputFileList(inputDirectory);
        Resizer::Image *original, *scaled;
        int interpolationIndex = ui->interpolationSelectionBox->currentIndex();

        for(int i = 0; i < fileList.count(); ++i)
        {
            original = Resizer::readImageFromFile(fileList.at(i).absoluteFilePath().toStdString().c_str());
            if(original != nullptr)
            {
                QString newFilename = prefix + fileList.at(i).fileName().section(".",0,0) + suffix + ".png";
                QString outFilePath = outputDirectory + "/" + newFilename;
                logg("Generating " + newFilename);
                if(ui->radioButtonPixels->isChecked())
                {
                    int width = ui->spinBoxWidthPixels->value();
                    int height = ui->spinBoxHeightPixels->value();
                    if(interpolationIndex == 0)
                        scaled = Resizer::nearestNeighbourInterpolation(original, width, height);
                    else if(interpolationIndex == 1)
                        scaled = Resizer::bicubicInterpolation(original, width, height);
                    else if(interpolationIndex == 2)
                        scaled = Resizer::bilinearInterpolation(original, width, height);
                }
                else
                {
                    float width = ui->spinBoxWidthPercentage->value() * 0.01f;
                    float height = ui->spinBoxHeightPercentage->value() * 0.01f;
                    if(interpolationIndex == 0)
                        scaled = Resizer::nearestNeighbourInterpolation(original, width, height);
                    else if(interpolationIndex == 1)
                        scaled = Resizer::bicubicInterpolation(original, width, height);
                    else if(interpolationIndex == 2)
                        scaled = Resizer::bilinearInterpolation(original, width, height);
                }

                if(scaled != nullptr)
                    Resizer::saveImageToFile(outFilePath.toStdString().c_str(), scaled);
                delete original;
                delete scaled;
            }
            ui->progressBar->setValue(100 * ((i + 1) / fileList.count()));
        }

        logg("DONE...");
    }
}

void MainWindow::loggSizeSetting()
{
    if(ui->radioButtonPixels->isChecked())
        logg("Resize images based on pixel settings...");
    else
        logg("Resize images based on percentage settings...");
}

void MainWindow::loggInterpolationSetting()
{
    logg("Interpolation method: " + ui->interpolationSelectionBox->currentText());
}

void MainWindow::handleInputBrowseButton()
{
    inputDirectory = QFileDialog::getExistingDirectory(this, tr("Open Directory"), inputDirectory, QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if(!inputDirectory.isNull())
    {
        listImageFiles(inputDirectory);
        ui->input_box->setText(inputDirectory);
        logg("New input directory: " + inputDirectory);
    }
}

void MainWindow::handleOutputBrowseButton()
{
    outputDirectory = QFileDialog::getExistingDirectory(this, tr("Open Directory"), outputDirectory, QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if(!outputDirectory.isNull())
    {
        ui->output_box->setText(outputDirectory);
        logg("New output directory: " + outputDirectory);
    }
}

void MainWindow::handlePrefixText()
{
    if(ui->prefixTextBox->text() != prefix)
    {
        prefix = ui->prefixTextBox->text();
        logg("Modified prefix: " + prefix);
    }
}

void MainWindow::handleSuffixText()
{
    if(ui->suffixTextBox->text() != suffix)
    {
        suffix = ui->suffixTextBox->text();
        logg("Modified prefix: " + suffix);
    }
}

void MainWindow::logg(QString text)
{
    if(text.isNull()) return;
    ui->loggBrowser->append(text);
}

QFileInfoList MainWindow::getInputFileList(QString filePath)
{
    if(filePath.isNull()) return QFileInfoList();
    QDir myDir(filePath);
    QStringList filters;
    filters << "*.png";
    return myDir.entryInfoList(filters, QDir::Files);
}

void MainWindow::listImageFiles(QString filePath)
{
    if(filePath.isNull()) return;
    QFileInfoList fileInfoList = getInputFileList(filePath);
    QString tmp = "Number of files: " + QString::number(fileInfoList.count()) + "\n";
    for(int i = 0; i < fileInfoList.count(); ++i)
        tmp += fileInfoList.at(i).fileName() + "\n";
    ui->textBrowser->setText(tmp);
}
