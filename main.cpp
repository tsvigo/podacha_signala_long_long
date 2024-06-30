//#include <QCoreApplication>

//int main(int argc, char *argv[])
//{
//    QCoreApplication a(argc, argv);

//    return a.exec();
//}
//###########################################################################
//
// 1.) Загрузка изображения BMP
// 2.) пиксели изображения в long long числа
// 3.) Чтение бинарного файла с long long числами
// 4.) Объединение векторов
// 5.) Запись объединенного вектора в новый бинарный файл
// 6.) Проверка содержимого нового бинарного файла

#include <QColor>
#include <QImage>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <vector>
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include <QApplication>

#include <QFile>
#include <QDataStream>
#include <QDebug>

#include <QCoreApplication>

#include <QFileDialog>

#include <QStringList>


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Структура для хранения заголовка BMP файла
static_assert(true); // dummy declaration, ends the preamble
#pragma pack(push, 1)
struct BMPHeader
{
    uint16_t bfType;
    uint32_t bfSize;
    uint16_t bfReserved1;
    uint16_t bfReserved2;
    uint32_t bfOffBits;
    uint32_t biSize;
    int32_t biWidth;
    int32_t biHeight;
    uint16_t biPlanes;
    uint16_t biBitCount;
    uint32_t biCompression;
    uint32_t biSizeImage;
    int32_t biXPelsPerMeter;
    int32_t biYPelsPerMeter;
    uint32_t biClrUsed;
    uint32_t biClrImportant;
};
#pragma pack(pop)
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Функция для преобразования пикселя в число типа long long
long long convertPixelToLongLong(const QRgb &pixel) {
    // Пример преобразования пикселя (RGBA) в число long long
    // Здесь мы просто используем значение пикселя напрямую
    // Это может быть изменено в зависимости от специфики преобразования
    return static_cast<long long>(pixel);
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Функция для загрузки изображения и преобразования его пикселей в vector<long long>
std::vector<long long> convertImageToVector(const QString &imagePath) {
    std::vector<long long> pixelData;

    // Загрузка изображения
    QImage image(imagePath);
    if (image.isNull()) {
        qDebug() << "Failed to load image";
        return pixelData;
    }

    // Обход всех пикселей изображения
    for (int y = 0; y < image.height(); ++y) {
        for (int x = 0; x < image.width(); ++x) {
            QRgb pixel = image.pixel(x, y);
            long long value = convertPixelToLongLong(pixel);
            pixelData.push_back(value); // Добавление значения в vector
        }
    }

    return pixelData;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//###########################################################################
// Функция для преобразования пикселей изображения в long long числа
std::vector<long long> convertImageToLongLong(const QImage &image)
{
    if (image.format() != QImage::Format_RGB32 && image.format() != QImage::Format_ARGB32) {
        throw std::runtime_error(
            "Неподдерживаемый формат изображения. Ожидается формат RGB32 или ARGB32.");
    }

    std::vector<long long> pixelValues;
    pixelValues.reserve(image.width() * image.height());

    for (int y = 0; y < image.height(); ++y) {
        for (int x = 0; x < image.width(); ++x) {
            QColor color = image.pixelColor(x, y);
            long long value = (static_cast<long long>(color.red()) << 16)
                              | (static_cast<long long>(color.green()) << 8)
                              | static_cast<long long>(color.blue());
            pixelValues.push_back(value);
        }
    }

    return pixelValues;
}

//###########################################################################
std::vector<long long> readBinaryFile(const std::string &filename)
{
    std::ifstream file(filename, std::ios::binary);
    if (!file)
        throw std::runtime_error("Ошибка открытия бинарного файла.");

    // Определяем размер файла
    file.seekg(0, std::ios::end);
    std::streamsize fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    if (fileSize % sizeof(long long) != 0)
        throw std::runtime_error("Некорректный размер бинарного файла.");

    std::vector<long long> values(fileSize / sizeof(long long));
    file.read(reinterpret_cast<char *>(values.data()), fileSize);

    return values;
}

void writeBinaryFile(const std::string &filename, const std::vector<long long> &values)
{
    std::ofstream file(filename, std::ios::binary);
    if (!file)
        throw std::runtime_error("Ошибка открытия файла для записи.");

    file.write(reinterpret_cast<const char *>(values.data()), values.size() * sizeof(long long));
}

std::vector<long long> mergeVectors(const std::vector<long long> &bmpValues,
                                    const std::vector<long long> &binValues)
{
    std::vector<long long> result(205);
    std::copy(bmpValues.begin(), bmpValues.end(), result.begin());
    std::copy(binValues.begin(), binValues.begin() + (205 - 100), result.begin() + 100);
    return result;
}

void printBinaryFileContent(const std::string &filename)
{
    std::ifstream file(filename, std::ios::binary);
    if (!file)
        throw std::runtime_error("Ошибка открытия бинарного файла для чтения.");

    std::vector<long long> values = readBinaryFile(filename);
    for (const long long &value : values) {
        std::cout << value << std::endl;
    }
}

int main(int argc, char *argv[])
{
    // Используем QApplication вместо QCoreApplication, чтобы поддерживать графические компоненты
    QApplication app(argc, argv);

    // Вызов диалога выбора файла
    QString fileName = QFileDialog::getOpenFileName(nullptr, "Выберите файл",
      "/home/viktor/1_rukoy/scale/",  "bmp Files (*.bmp)");

    // Проверка, был ли файл выбран
    if (!fileName.isEmpty()) {
        qDebug() << "Выбранный файл:" << fileName;
    } else {
        qDebug() << "Файл не был выбран.";
    }

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// вместо диалога выбора файла перебор по списку
    // Получение списка файлов с расширением .bmp в исходном каталоге
        QString sourceDirPath = "/home/viktor/1_rukoy/scale/";
    QDir sourceDir(sourceDirPath);
    QStringList bmpFiles = sourceDir.entryList(QStringList() << "*.bmp", QDir::Files);
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    try {

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        QString imagePath =fileName;
            //"/home/viktor/1_rukoy/Sprite-0001-m.bmp";

        // Преобразование изображения в vector<long long>
        std::vector<long long> pixelData = convertImageToVector(imagePath);

        // Проверка и вывод результата
        if (!pixelData.empty()) {
            qDebug() << "Image converted successfully. Number of pixels:" << pixelData.size();
        } else {
            qDebug() << "Failed to convert image";
        }

        // Чтение бинарного файла с long long числами
        std::vector<long long> binValues = readBinaryFile(
            "/home/viktor/my_projects_qt_2/sgenerirovaty_long_long_neyroni/random_numbers.bin");

        // Объединение векторов
        std::vector<long long> mergedValues = mergeVectors(pixelData, binValues);
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Разбор пути и создание нового имени файла
        QDir dir = QFileInfo(fileName).absoluteDir();
        QString baseName = QFileInfo(fileName).fileName();
        QString newDir = dir.absolutePath() + "/combined_numbers";

        // Убедимся, что директория "combined_numbers" существует
        if (!QDir().exists(newDir)) {
            QDir().mkdir(newDir);
        }

        QString newFileName = newDir + "/" + baseName + "_combined_numbers.bin";

        qDebug() << "Исходный файл:" << fileName;
        qDebug() << "Новый файл:" << newFileName;
        // Преобразование QString в std::string
        std::string stdFileName = newFileName.toStdString();
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // если файл ещё не существует то
        // Запись объединенного вектора в новый бинарный файл
        writeBinaryFile(stdFileName,
        //    "/home/viktor/my_projects_qt_2/podacha_signala_long_long/"
         //               "combined_numbers.bin",
                        mergedValues);

        // Проверка содержимого нового бинарного файла
        printBinaryFileContent(
          //  "/home/viktor/my_projects_qt_2/podacha_signala_long_long/"
                //               "combined_numbers.bin"
            stdFileName
                               );
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    return 0;
  };
