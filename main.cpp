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

//std::vector<long long> readBMP(const std::string &filename)
//{
//    std::ifstream file(filename, std::ios::binary);
//    if (!file)
//        throw std::runtime_error("Ошибка открытия BMP файла.");

//    BMPHeader header;
//    file.read(reinterpret_cast<char *>(&header), sizeof(header));
//    if (header.bfType != 0x4D42)
//        throw std::runtime_error("Некорректный BMP файл.");

//    // Переходим к пикселям изображения
//    file.seekg(header.bfOffBits, std::ios::beg);

//    // Считываем пиксели (предполагаем, что изображение 10x10 пикселей, 24 бита на пиксель)
//    int pixelCount = 100;
//    std::vector<long long> pixelValues(pixelCount);
//    for (int i = 0; i < pixelCount; ++i) {
//        unsigned char pixel[3];
//        file.read(reinterpret_cast<char *>(pixel), 3);
//        long long value = (pixel[2] << 16) | (pixel[1] << 8) | pixel[0]; // RGB в long long
//        pixelValues[i] = value;
//    }

//    return pixelValues;
//}
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

int main()
{
    try {
        // Загрузка изображения BMP
        QImage image("/home/viktor/Загрузки/data/none/300/masshtab/black-white/1.bmp");
        if (image.isNull()) {
            throw std::runtime_error("Ошибка загрузки изображения.");
        }

        // Преобразование пикселей изображения в long long числа
        std::vector<long long> bmpValues = convertImageToLongLong(image);
        // Чтение BMP файла и преобразование пикселей в long long числа
        //        std::vector<long long> bmpValues = readBMP(
        //            "/home/viktor/Загрузки/data/none/300/masshtab/black-white/1.bmp");

        // Чтение бинарного файла с long long числами
        std::vector<long long> binValues = readBinaryFile(
            "/home/viktor/my_projects_qt_2/sgenerirovaty_long_long_neyroni/random_numbers.bin");

        // Объединение векторов
        std::vector<long long> mergedValues = mergeVectors(bmpValues, binValues);

        // Запись объединенного вектора в новый бинарный файл
        writeBinaryFile("/home/viktor/my_projects_qt_2/podacha_signala_long_long/"
                        "combined_numbers.bin",
                        mergedValues);

        // Проверка содержимого нового бинарного файла
        printBinaryFileContent("/home/viktor/my_projects_qt_2/podacha_signala_long_long/"
                               "combined_numbers.bin");
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    return 0;
}
