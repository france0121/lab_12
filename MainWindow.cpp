#include "MainWindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QListWidget>
#include <QGroupBox>
#include <QMessageBox>
#include <random>
#include <algorithm>

using namespace std;

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    setWindowTitle("ЛР 12 — Вариант 29");
    resize(1000, 650);

    // ========== ГРУППА ВВОДА ==========
    leRows = new QLineEdit;
    leRows->setPlaceholderText("1-50");
    leCols = new QLineEdit;
    leCols->setPlaceholderText("1-50");
    leMin = new QLineEdit;
    leMin->setPlaceholderText("-100");
    leMax = new QLineEdit;
    leMax->setPlaceholderText("100");

    QGroupBox *inputGroup = new QGroupBox("Параметры матрицы");
    QGridLayout *gridLayout = new QGridLayout;
    gridLayout->addWidget(new QLabel("Строки (M):"), 0, 0);
    gridLayout->addWidget(leRows, 0, 1);
    gridLayout->addWidget(new QLabel("Столбцы (N):"), 1, 0);
    gridLayout->addWidget(leCols, 1, 1);
    gridLayout->addWidget(new QLabel("Мин. значение:"), 2, 0);
    gridLayout->addWidget(leMin, 2, 1);
    gridLayout->addWidget(new QLabel("Макс. значение:"), 3, 0);
    gridLayout->addWidget(leMax, 3, 1);
    inputGroup->setLayout(gridLayout);

    // ========== ГРУППА ВЫВОДА ==========
    listOriginal = new QListWidget;
    listResult = new QListWidget;

    QGroupBox *outputGroup = new QGroupBox("Результаты");
    QHBoxLayout *outputLayout = new QHBoxLayout;

    QVBoxLayout *leftLayout = new QVBoxLayout;
    leftLayout->addWidget(new QLabel("Исходная матрица M:"));
    leftLayout->addWidget(listOriginal);

    QVBoxLayout *rightLayout = new QVBoxLayout;
    rightLayout->addWidget(new QLabel("Результат N (после замены):"));
    rightLayout->addWidget(listResult);

    outputLayout->addLayout(leftLayout);
    outputLayout->addLayout(rightLayout);
    outputGroup->setLayout(outputLayout);

    // ========== КНОПКИ ==========
    btnGenerate = new QPushButton("Сгенерировать и решить");
    btnExit = new QPushButton("Выход");

    QHBoxLayout *btnLayout = new QHBoxLayout;
    btnLayout->addStretch();
    btnLayout->addWidget(btnGenerate);
    btnLayout->addWidget(btnExit);
    btnLayout->addStretch();

    // ========== СТАТУСНАЯ СТРОКА ==========
    labelStatus = new QLabel("Введите параметры и нажмите 'Сгенерировать'");
    labelStatus->setAlignment(Qt::AlignCenter);
    labelStatus->setMinimumHeight(40);
    labelStatus->setStyleSheet("QLabel { padding: 10px; border-radius: 5px; background-color: #ecf0f1; color: #34495e; }");

    // ========== ОСНОВНОЙ МАКЕТ ==========
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(inputGroup);
    mainLayout->addWidget(outputGroup);
    mainLayout->addLayout(btnLayout);
    mainLayout->addWidget(labelStatus);

    QWidget *central = new QWidget;
    central->setLayout(mainLayout);
    setCentralWidget(central);

    // ========== ПОДКЛЮЧЕНИЕ СИГНАЛОВ ==========
    connect(btnGenerate, &QPushButton::clicked, this, &MainWindow::onGenerateClicked);
    connect(btnExit, &QPushButton::clicked, this, &MainWindow::onExitClicked);
}

MainWindow::~MainWindow() {}

// Проверка ввода
bool MainWindow::validateInput() {
    bool ok;
    int rows = leRows->text().toInt(&ok);
    if (!ok || rows <= 0 || rows > 50) {
        updateStatus("Ошибка: строки от 1 до 50", true);
        return false;
    }
    int cols = leCols->text().toInt(&ok);
    if (!ok || cols <= 0 || cols > 50) {
        updateStatus("Ошибка: столбцы от 1 до 50", true);
        return false;
    }
    int minVal = leMin->text().toInt(&ok);
    if (!ok) return false;
    int maxVal = leMax->text().toInt(&ok);
    if (!ok) return false;
    if (minVal >= maxVal) {
        updateStatus("Ошибка: минимум должен быть меньше максимума", true);
        return false;
    }
    return true;
}

// Обновление статуса
void MainWindow::updateStatus(const QString& message, bool isError) {
    labelStatus->setText(message);
    if (isError) {
        labelStatus->setStyleSheet("QLabel { background-color: #f8d7da; color: #721c24; padding: 10px; border-radius: 5px; }");
    } else {
        labelStatus->setStyleSheet("QLabel { background-color: #d4edda; color: #155724; padding: 10px; border-radius: 5px; }");
    }
}

// Проверка, все ли элементы строки отрицательные
bool MainWindow::isAllNegative(const vector<int>& row) {
    for (int val : row) {
        if (val >= 0) return false;
    }
    return true;
}

// Поиск индексов строк, состоящих только из отрицательных чисел
vector<int> MainWindow::findNegativeRows(const vector<vector<int>>& matr) {
    vector<int> indices;
    for (size_t i = 0; i < matr.size(); i++) {
        if (isAllNegative(matr[i])) {
            indices.push_back(static_cast<int>(i));
        }
    }
    return indices;
}

// Перестановка двух строк
vector<vector<int>> MainWindow::swapRows(vector<vector<int>> matr, int row1, int row2) {
    swap(matr[row1], matr[row2]);
    return matr;
}

// Вывод матрицы в QListWidget
void MainWindow::displayMatrix(const vector<vector<int>>& matr, QListWidget* widget) {
    widget->clear();
    if (matr.empty()) {
        widget->addItem("Матрица пуста");
        return;
    }

    for (size_t i = 0; i < matr.size(); i++) {
        QString rowStr;
        for (size_t j = 0; j < matr[i].size(); j++) {
            rowStr += QString("%1 ").arg(matr[i][j], 4);
        }
        widget->addItem(QString("Строка %1: %2").arg(i).arg(rowStr));
    }
}

// Генерация случайной матрицы
vector<vector<int>> generateRandomMatrix(int rows, int cols, int minVal, int maxVal) {
    vector<vector<int>> matr(rows, vector<int>(cols));
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<int> dist(minVal, maxVal);

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            matr[i][j] = dist(gen);
        }
    }
    return matr;
}

// Обработчик кнопки
void MainWindow::onGenerateClicked() {
    listOriginal->clear();
    listResult->clear();

    if (!validateInput()) return;

    int rows = leRows->text().toInt();
    int cols = leCols->text().toInt();
    int minVal = leMin->text().toInt();
    int maxVal = leMax->text().toInt();

    // Генерация исходной матрицы
    vector<vector<int>> matrix = generateRandomMatrix(rows, cols, minVal, maxVal);
    displayMatrix(matrix, listOriginal);

    // Поиск строк с отрицательными элементами
    vector<int> negativeRows = findNegativeRows(matrix);

    // Проверка условия: более одной такой строки
    if (negativeRows.size() <= 1) {
        updateStatus(QString("Условие не выполнено: найдено %1 строк из отрицательных чисел (нужно >1)").arg(negativeRows.size()), true);
        listResult->addItem("(преобразование не выполнено)");
        return;
    }

    // Меняем местами последнюю строку и первую из найденных
    int lastRow = rows - 1;
    int firstNegativeRow = negativeRows[0];

    vector<vector<int>> resultMatrix = swapRows(matrix, lastRow, firstNegativeRow);
    displayMatrix(resultMatrix, listResult);

    updateStatus(QString("Выполнено. Поменяны местами строка %1 (последняя) и строка %2 (первая из отрицательных)").arg(lastRow).arg(firstNegativeRow));
}

void MainWindow::onExitClicked() {
    close();
}
