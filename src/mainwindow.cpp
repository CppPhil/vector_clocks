#include "mainwindow.hpp"
#include "../deps/genann/genann.h" // genann_actfun, ...
#include "closest_to_one.hpp"      // isp2::closestToOne
#include "constants.hpp" // isp2::relativePathFromAppFilePathToMnistDirectory, ...
#include "data.hpp"      // isp2::trainingSetLabelFileBuffer
#include "genann_relu.hpp"  // isp2::genannReLu
#include "genann_tanh.hpp"  // isp2::genannTanh
#include <QCoreApplication> // QCoreApplication::applicationDirPath
#include <QDir>             // QDir::separator
#include <cmath>            // std::exp, std::tanh
#include <iomanip>          // std::setprecision
#include <ios>              // std::fixed
#include <sstream>          // std::ostringstream
#include <utility>          // std::move

namespace isp2 {
// String constants for the different activation functions.
const QString sigmoidString       = "sigmoid";
const QString cachedSigmoidString = "cached sigmoid";
const QString thresholdString     = "threshold";
const QString linearString        = "linear";
const QString tanhString          = "tanh";
const QString reLuString          = "ReLu";

/*!
 * \brief Maps strings to their corresponding activation functions.
 * \param str The string input.
 * \return The corresponding activation funtion.
 * \warning Only put in valid strings!
 **/
static genann_actfun stringToFunction(const QString& str)
{
    if (str == sigmoidString) { return &genann_act_sigmoid; }
    else if (str == cachedSigmoidString) {
        return &genann_act_sigmoid_cached;
    }
    else if (str == thresholdString) {
        return &genann_act_threshold;
    }
    else if (str == linearString) {
        return &genann_act_linear;
    }
    else if (str == tanhString) {
        return &genannTanh;
    }
    else if (str == reLuString) {
        return &genannReLu;
    }

    Q_UNREACHABLE();
}

/*!
 * \def MNIST_DIR
 * \brief Object like macro for the path to the directory containing
 *        the MNIST files.
 **/
#define MNIST_DIR                                                       \
    (QString(                                                           \
         ::QCoreApplication::applicationDirPath() + ::QDir::separator() \
         + ::isp2::relativePathFromAppFilePathToMnistDirectory)         \
         .toStdString())

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , m_trainingSetLabelFile(/* Load the images */
                             MNIST_DIR + trainingSetLabelFileString,
                             trainingSetLabelFileBuffer(),
                             trainingSetLabelFileSize)
    , m_trainingSetImageFile(
          MNIST_DIR + trainingSetImageFileString,
          trainingSetImageFileBuffer(),
          trainingSetImageFileSize)
    , m_testSetLabelFile(
          MNIST_DIR + testSetLabelFileString,
          testSetLabelFileBuffer(),
          testSetLabelFileSize)
    , m_testSetImageFile(
          MNIST_DIR + testSetImageFileString,
          testSetImageFileBuffer(),
          testSetImageFileSize)
    , m_thread(nullptr)
    , m_testImageDataEntries()
{
    init();
}
#undef MNIST_DIR

void MainWindow::onNewProgress(int currentImage, int totalImages)
{
    // Update progress label
    m_ui.labelProgress->setText(
        QString("processed %1 of %2 images")
            .arg(QString::number(currentImage), QString::number(totalImages)));

    // Update progress bar
    const double percentage
        = (static_cast<double>(currentImage) / totalImages) * 100.0;
    m_ui.progressBar->setValue(static_cast<int>(percentage));

    // If we're done -> do the following.
    if (currentImage == totalImages) {
        m_ui.progressBar->setValue(100);

        // Calculate how many images were correctly classified by the neural
        // network.
        int correctCount = 0;
        for (const TestImageData& testImageData : m_testImageDataEntries) {
            if (testImageData.closestToOne() == testImageData.expectedLabel) {
                ++correctCount;
            }
        }

        // Update the statistics label.
        m_ui.labelClassificationStatistics->setText(
            QString("correctly classified %1 of %2 images (%3%)")
                .arg(
                    QString::number(correctCount),
                    QString::number(m_ui.spinBoxTestsToRun->value()),
                    QString::number(
                        (static_cast<double>(correctCount)
                         / m_ui.spinBoxTestsToRun->value())
                        * 100.0)));

        setGuiControlsEnabled(true);
    }
}

void MainWindow::onNewTestImageProcessed(
    QPixmap  bitmap,
    pl::byte expectedLabel,
    double   zero,
    double   one,
    double   two,
    double   three,
    double   four,
    double   five,
    double   six,
    double   seven,
    double   eight,
    double   nine)
{
    // Put the info into the vector.
    m_testImageDataEntries.emplace_back(
        std::move(bitmap),
        expectedLabel,
        zero,
        one,
        two,
        three,
        four,
        five,
        six,
        seven,
        eight,
        nine);

    const int imageNumber = static_cast<int>(m_testImageDataEntries.size());
    const int labelThatIsTheClosestToOne
        = m_testImageDataEntries.back().closestToOne();

    const int numberColumn                = 0;
    const int correctClassificationColumn = 1;

    m_ui.tableWidget->insertRow(imageNumber - 1);

    m_ui.tableWidget->setItem(
        imageNumber - 1,
        numberColumn,
        new QTableWidgetItem(QString::number(static_cast<int>(expectedLabel))));
    m_ui.tableWidget->setItem(
        imageNumber - 1,
        correctClassificationColumn,
        new QTableWidgetItem(
            labelThatIsTheClosestToOne == expectedLabel ? "success"
                                                        : "failure"));
}

void MainWindow::onStartButtonClicked()
{
    m_ui.tableWidget->clear();
    m_ui.tableWidget->setRowCount(0);
    m_ui.tableWidget->setHorizontalHeaderLabels({"Number", "Classification"});
    m_testImageDataEntries.clear();

    m_thread = std::make_unique<Thread>(
        static_cast<std::size_t>(m_ui.spinBoxTrainingCycles->value()),
        static_cast<std::size_t>(m_ui.spinBoxTestsToRun->value()),
        m_ui.spinBoxHiddenLayerCount->value(),
        m_ui.spinBoxHiddenLayerNeuronCount->value(),
        stringToFunction(m_ui.comboBoxFunction->currentText()),
        m_ui.doubleSpinBoxLearningRate->value(),
        m_trainingSetLabelFile,
        m_trainingSetImageFile,
        m_testSetLabelFile,
        m_testSetImageFile);
    Thread* thd = m_thread.get();
    connect(
        thd,
        &Thread::newProgress,
        this,
        &MainWindow::onNewProgress,
        Qt::QueuedConnection);
    connect(
        thd,
        &Thread::newTestImageProcessed,
        this,
        &MainWindow::onNewTestImageProcessed,
        Qt::QueuedConnection);

    setGuiControlsEnabled(false);
    thd->start(); // Actually start the thread for real
}

void MainWindow::onTableCellClicked(int row, int column)
{
    Q_UNUSED(column);

    // Lambda to convert a double to a string with decent precision
    // and without stupid scientific notation that no mortal
    // human being may read.
    const auto toS = [](double f) {
        std::ostringstream oss;
        const int          precision = 12;
        oss << std::fixed << std::setprecision(precision) << f;
        return QString::fromStdString(oss.str());
    };

    // If you clicked on a cell for which data actually exists.
    if (static_cast<std::size_t>(row) < m_testImageDataEntries.size()) {
        // Grab it.
        const TestImageData& d
            = m_testImageDataEntries[static_cast<std::size_t>(row)];

        // Update the GUI left hand side.
        m_ui.labelImage->setPixmap(d.bitmap);
        m_ui.labelExpectedLabel->setText(
            "expected: " + QString::number(d.expectedLabel));
        m_ui.labelZero->setText("zero: " + toS(d.zero));
        m_ui.labelOne->setText("one: " + toS(d.one));
        m_ui.labelTwo->setText("two: " + toS(d.two));
        m_ui.labelThree->setText("three: " + toS(d.three));
        m_ui.labelFour->setText("four: " + toS(d.four));
        m_ui.labelFive->setText("five: " + toS(d.five));
        m_ui.labelSix->setText("six: " + toS(d.six));
        m_ui.labelSeven->setText("seven: " + toS(d.seven));
        m_ui.labelEight->setText("eight: " + toS(d.eight));
        m_ui.labelNine->setText("nine: " + toS(d.nine));
    }
}

MainWindow::TestImageData::TestImageData(
    QPixmap&& pixmap,
    pl::byte  label,
    double    p_zero,
    double    p_one,
    double    p_two,
    double    p_three,
    double    p_four,
    double    p_five,
    double    p_six,
    double    p_seven,
    double    p_eight,
    double    p_nine)
    : bitmap(std::move(pixmap))
    , expectedLabel(label)
    , zero(p_zero)
    , one(p_one)
    , two(p_two)
    , three(p_three)
    , four(p_four)
    , five(p_five)
    , six(p_six)
    , seven(p_seven)
    , eight(p_eight)
    , nine(p_nine)
{
}

int MainWindow::TestImageData::closestToOne() const
{
    return ::isp2::closestToOne(
        {zero, one, two, three, four, five, six, seven, eight, nine});
}

void MainWindow::init()
{
    m_ui.setupUi(this);

    // Default values.
    const int     defaultTrainingCycles         = 4;
    const int     defaultTestsToRun             = 10000;
    const int     defaultHiddenLayerCount       = 1;
    const int     defaultHiddenLayerNeuronCount = 400;
    const QString defaultFunction               = cachedSigmoidString;
    const double  defaultLearningRate           = 0.1;
    const int     defaultProgressBarValue       = 0;

    // Set the defaults.
    m_ui.spinBoxTrainingCycles->setValue(defaultTrainingCycles);
    m_ui.spinBoxTestsToRun->setValue(defaultTestsToRun);
    m_ui.spinBoxHiddenLayerCount->setValue(defaultHiddenLayerCount);
    m_ui.spinBoxHiddenLayerNeuronCount->setValue(defaultHiddenLayerNeuronCount);
    m_ui.comboBoxFunction->clear();
    m_ui.comboBoxFunction->insertItems(
        m_ui.comboBoxFunction->count(),
        QStringList() << sigmoidString << cachedSigmoidString << thresholdString
                      << linearString << tanhString << reLuString);
    m_ui.doubleSpinBoxLearningRate->setValue(defaultLearningRate);
    m_ui.comboBoxFunction->setCurrentText(defaultFunction);
    m_ui.progressBar->setValue(defaultProgressBarValue);

    setGuiControlsEnabled(true);

    establishConnections();
}

void MainWindow::establishConnections()
{
    connect(
        m_ui.pushButtonStart,
        &QPushButton::clicked,
        this,
        &MainWindow::onStartButtonClicked);

    connect(
        m_ui.tableWidget,
        &QTableWidget::cellClicked,
        this,
        &MainWindow::onTableCellClicked);
}

void MainWindow::setGuiControlsEnabled(bool enabled)
{
    m_ui.pushButtonStart->setEnabled(enabled);
    m_ui.spinBoxTrainingCycles->setEnabled(enabled);
    m_ui.spinBoxTestsToRun->setEnabled(enabled);
    m_ui.spinBoxHiddenLayerCount->setEnabled(enabled);
    m_ui.spinBoxHiddenLayerNeuronCount->setEnabled(enabled);
    m_ui.doubleSpinBoxLearningRate->setEnabled(enabled);
    m_ui.comboBoxFunction->setEnabled(enabled);
}
} // namespace isp2
