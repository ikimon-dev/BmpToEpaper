#include <iostream>
#include <vector>
#include <cassert>
#include <fstream>
#include <string.h>
#include <cstddef>
#include <cstdint>

// https://any-programming.hatenablog.com/entry/2017/04/27/110723

#pragma pack(2) // https://www.mm2d.net/main/prog/c/image_io-05.html

struct BitmapHeader
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
    int32_t biXPixPerMeter;
    int32_t biYPixPerMeter;
    uint32_t biClrUsed;
    uint32_t biClrImporant;
};

inline uint16_t swap16(uint16_t value)
{
    uint16_t ret;
    ret = value << 8;
    ret |= value >> 8;
    return ret;
}

inline uint32_t swap32(uint32_t value)
{
    uint32_t ret;
    ret = value << 24;
    ret |= (value & 0x0000FF00) << 8;
    ret |= (value & 0x00FF0000) >> 8;
    ret |= value >> 24;
    return ret;
}

inline int32_t swap32(int32_t value)
{
    int32_t ret;
    ret = value << 24;
    ret |= (value & 0x0000FF00) << 8;
    ret |= (value & 0x00FF0000) >> 8;
    ret |= value >> 24;
    return ret;
}

int ToBitmapStep(int step)
{
    int paddings[] = {0, 3, 2, 1};
    return step + paddings[step % 4];
}

void LoadBitmap24(std::string path, int *width, int *height, std::vector<unsigned char> *pixels)
{
    std::ifstream ifs(path, std::ios::binary);
    assert(ifs.is_open());

    BitmapHeader header;
    ifs.read((char *)&header, sizeof(header));
    int w = header.biWidth;
    int h = header.biHeight;
    int step = ToBitmapStep(3 * w);

    std::vector<unsigned char> originalPixels(step * h);
    ifs.read((char *)originalPixels.data(), originalPixels.size());

    pixels->resize(3 * w * h);
    for (int i = 0; i < h; ++i)
        memcpy(pixels->data() + i * 3 * w, originalPixels.data() + i * step, 3 * w);

    *width = w;
    *height = h;
}

void SaveBitmap24(std::string path, int width, int height, const unsigned char *pixels)
{
    std::ofstream ofs(path, std::ios::binary);
    assert(ofs.is_open());

    int step = ToBitmapStep(3 * width);

    BitmapHeader header;
    header.bfType = 0x4d42; // "BM"
    header.bfSize = step * height + 54;
    header.bfReserved1 = 0;
    header.bfReserved2 = 0;
    header.bfOffBits = 54;
    header.biSize = 40;
    header.biWidth = width;
    header.biHeight = height;
    header.biPlanes = 1;
    header.biBitCount = 24;
    header.biCompression = 0;
    header.biSizeImage = 0;
    header.biXPixPerMeter = 0;
    header.biYPixPerMeter = 0;
    header.biClrUsed = 0;
    header.biClrImporant = 0;
    ofs.write((char *)&header, sizeof(header));

    std::vector<unsigned char> bmpPixels(step * height);
    for (int i = 0; i < height; ++i)
        memcpy(bmpPixels.data() + i * step, pixels + i * 3 * width, 3 * width);
    ofs.write((char *)bmpPixels.data(), bmpPixels.size());
}

int main(int, char **)
{

    int width, height;
    std::vector<uint8_t> pixels;
    LoadBitmap24("../parrots.bmp", &width, &height, &pixels);

    uint8_t data = 0;

    for (int i = 0; i < width * height; ++i)
    {

        data = 0.0722 * pixels[3 * i] + 0.7152 * pixels[3 * i + 1] + 0.2126 * pixels[3 * i + 2]; // https://daeudaeu.com/grayscale/#RGB:~:text=%E3%81%84%E3%81%A8%E6%80%9D%E3%81%84%E3%81%BE%E3%81%99%E3%80%82-,%E3%82%B0%E3%83%AC%E3%83%BC%E3%82%B9%E3%82%B1%E3%83%BC%E3%83%AB%E5%8C%96%E5%BE%8C%E3%81%AE%E8%BC%9D%E5%BA%A6%E5%80%A4,-/*%20r%3AR%E3%81%AE
        if (data > 0)
        {
            pixels[3 * i] = data;
            pixels[3 * i + 1] = data;
            pixels[3 * i + 2] = data;
        }
        else
        {

            pixels[3 * i] = 0;
            pixels[3 * i + 1] = 0;
            pixels[3 * i + 2] = 0;
        }
    }

    // std::swap(pixels[3 * i], pixels[3 * i + 2]);

    SaveBitmap24("output.bmp", width, height, pixels.data());
    return 0;

    std::cout << "Hello, world!\n";
}
