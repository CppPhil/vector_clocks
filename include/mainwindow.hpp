#pragma once
#include "idx_file.hpp" // isp2::IdxFile
#include "thread.hpp"   // isp2::Thread
#include "ui_mainwindow.h"
#include <QPixmap> // QPixmap
#include <memory>  // std::unique_ptr

namespace isp2 {
/*!
 * \brief The MainWindow GUI.
 **/
class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    /*!
     * \brief Fills the global buffers with IDX file data.
     * \param parent The QObject parent.
     **/
    explicit MainWindow(QWidget* parent = nullptr);

private slots:
    /*!
     * \brief Slot to handle progress reports from the thread.
     * \param currentImage The current image processed.
     * \param totalImages The total amount of images.
     **/
    void onNewProgress(int currentImage, int totalImages);

    /*!
     * \brief Slot to handle a new test image having been
     *        processed by the thread.
     * \param bitmap The actual image that can be rendered.
     * \param expectedLabel The number it's supposed to be.
     * \param zero How much the neural network thinks it's a 0.
     * \param one How much the neural network thinks it's a 1.
     * \param two How much the neural network thinks it's a 2.
     * \param three How much the neural network thinks it's a 3.
     * \param four How much the neural network thinks it's a 4.
     * \param five How much the neural network thinks it's a 5.
     * \param six How much the neural network thinks it's a 6.
     * \param seven How much the neural network thinks it's a 7.
     * \param eight How much the neural network thinks it's a 8.
     * \param nine How much the neural network thinks it's a 9.
     **/
    void onNewTestImageProcessed(
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
        double   nine);

    /*!
     * \brief Callback for the start buttons OnClick event.
     *
     * Starts the thread.
     **/
    void onStartButtonClicked();

    /*!
     * \brief Callback for a cell in the table being clicked.
     * \param row The row of the cell.
     * \param column The column of the cell.
     *
     * Shows the info for the selected MNIST image selected
     * on the left hand side of the GUI.
     **/
    void onTableCellClicked(int row, int column);

private:
    /*!
     * \brief Structure for the data to show on the left hand side.
     **/
    struct TestImageData {
        TestImageData(
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
            double    p_nine);

        /*!
         * \brief Which is closest to one.
         * \return [0,9]
         **/
        int closestToOne() const;

        QPixmap  bitmap;
        pl::byte expectedLabel;
        double   zero;
        double   one;
        double   two;
        double   three;
        double   four;
        double   five;
        double   six;
        double   seven;
        double   eight;
        double   nine;
    };

    /*!
     * \brief Initialization routine.
     **/
    void init();

    /*!
     * \brief Establishes connections for the callbacks
     *        for the GUI controls.
     **/
    void establishConnections();

    /*!
     * \brief Enables / disables GUI controls.
     * \param enabled true if the GUI controls shall be enabled,
     *        otherwise false.
     **/
    void setGuiControlsEnabled(bool enabled);

    Ui::MainWindow          m_ui;
    IdxFile                 m_trainingSetLabelFile;
    IdxFile                 m_trainingSetImageFile;
    IdxFile                 m_testSetLabelFile;
    IdxFile                 m_testSetImageFile;
    std::unique_ptr<Thread> m_thread;
    std::vector<TestImageData>
        m_testImageDataEntries; /*!< Holds the data for the left hand side
                                 *   visualization thingie in the GUI.
                                 **/
};
} // namespace isp2
