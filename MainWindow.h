#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <vector>

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onGenerateClicked();
    void onExitClicked();

private:
    void displayMatrix(const std::vector<std::vector<int>>& matr, QListWidget* widget);
    bool isAllNegative(const std::vector<int>& row);
    std::vector<int> findNegativeRows(const std::vector<std::vector<int>>& matr);
    std::vector<std::vector<int>> swapRows(std::vector<std::vector<int>> matr, int row1, int row2);
    void updateStatus(const QString& message, bool isError = false);
    bool validateInput();

    QLineEdit *leRows, *leCols, *leMin, *leMax;
    QPushButton *btnGenerate, *btnExit;
    QListWidget *listOriginal, *listResult;
    QLabel *labelStatus;
};

#endif
