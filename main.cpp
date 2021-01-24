#include <iostream>
#include <fstream>

typedef unsigned short WORD;
typedef unsigned long DWORD;
typedef long LONG;
typedef unsigned char BYTE;

#pragma pack(push, 1)
struct BITMAPFILEHEADER
{
    WORD bfType;
    DWORD bfSize;
    WORD bfReserved1;
    WORD bfReserved2;
    DWORD bfOffBits;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct BITMAPINFOHEADER
{
    DWORD biSize;
    LONG biWidth;
    LONG biHeight;
    WORD biPlanes;
    WORD biBitCount;
    DWORD biCompression;
    DWORD biSizeImage;
    LONG biXPelsPerMeter;
    LONG biYPelsPerMeter;
    DWORD biClrUsed;
    DWORD biClrImportant;
};
#pragma pack(pop)

using namespace std;

const WORD BMP_FORMAT = 0x4D42;

class Bitmap
{
private:
    BYTE* data[3];
    BITMAPFILEHEADER h_file[3];
    BITMAPINFOHEADER h_info[3];

public:
    void load(const char*,int);
    void merge(int,int,float,int);
    void save(const char*,int);
};

void Bitmap::load(const char* filename, int n)
{
    std::ifstream file;

    file.open(filename, std::fstream::binary);
    if (!file.is_open()) {
        cout << "File not found !- " << filename;
        exit(0);
    }

    file.read((char*)&(h_file[n]), sizeof(BITMAPFILEHEADER));

    //
    if (h_file[n].bfType != BMP_FORMAT)
    {
        cout << "File not valid !- " << filename;
    }

    file.read((char*)&h_info[n], sizeof(BITMAPINFOHEADER));

    //
    file.seekg(h_file[n].bfOffBits);

    data[n] = new BYTE[h_info[n].biSizeImage];

    if (!data[n])
    {
        cout << "Bitmap file corrupt !- " << filename;
        delete data[n];
        file.close();
        exit(0);
    }

    file.read((char*)data[n], h_info[n].biSizeImage);

    if (data[n] == NULL)
    {
        cout << "Unable to read data from " << filename;
        file.close();
        exit(0);
    }

    file.close();
}

void Bitmap::merge(int i1, int i2, float r, int im)
{

    h_info[im].biBitCount = 24;
    h_info[im].biClrImportant = 0;
    h_info[im].biClrUsed = 0;
    h_info[im].biCompression = 0;
    h_info[im].biHeight = h_info[i1].biHeight; if (h_info[im].biHeight < h_info[i2].biHeight)h_info[im].biHeight = h_info[i2].biHeight;
    h_info[im].biWidth = h_info[i1].biWidth; if (h_info[im].biWidth < h_info[i2].biWidth)h_info[im].biWidth = h_info[i2].biWidth;
    h_info[im].biPlanes = 1;
    h_info[im].biSize = 40;
    h_info[im].biSizeImage = h_info[im].biHeight * h_info[im].biWidth * 3;
    h_info[im].biXPelsPerMeter = 0;
    h_info[im].biYPelsPerMeter = 0;

    h_file[im].bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
    h_file[im].bfReserved1 = 0;
    h_file[im].bfReserved2 = 0;
    h_file[im].bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + (sizeof(h_info[im].biBitCount) * (static_cast<unsigned long long>(h_info[im].biWidth) * static_cast<unsigned long long>(h_info[im].biHeight)));
    h_file[im].bfType = BMP_FORMAT;

    data[im] = new BYTE[h_info[im].biSizeImage];

    /* When both files are of same resolution */
    if (h_info[i2].biWidth == h_info[i1].biWidth && h_info[i2].biHeight == h_info[i1].biHeight) {
        for (unsigned int i = 0; i < h_info[im].biSizeImage; i++) {
            data[im][i] = BYTE(data[i1][i] * r + data[i2][i] * (1 - r));
        }
    }
    /* When image 2 is wider */
    else if (h_info[i1].biWidth < h_info[i2].biWidth && h_info[i1].biHeight == h_info[i2].biHeight) {
        for (unsigned int i = 0; i < h_info[im].biSizeImage; i++) {
            if ((signed int)(i % (h_info[i2].biWidth * 3)) < (h_info[i1].biWidth * 3)) {
                BYTE tmp = (data[i1][(i / (h_info[i2].biWidth * 3)) * (h_info[i1].biWidth * 3) + (i % (h_info[i2].biWidth * 3))]);
                data[im][i] = BYTE(tmp * r + data[i2][i] * (1 - r));
            }
            else data[im][i] = BYTE(data[i2][i]);
        }
    }
    /* When image 1 is wider */
    else if (h_info[i2].biWidth < h_info[i1].biWidth && h_info[i2].biHeight == h_info[i1].biHeight) {
        for (unsigned int i = 0; i < h_info[im].biSizeImage; i++) {
            if ((signed int)(i % (h_info[i1].biWidth * 3)) < (h_info[i2].biWidth * 3)) {
                BYTE tmp = (data[i2][(i / (h_info[i1].biWidth * 3)) * (h_info[i2].biWidth * 3) + (i % (h_info[i1].biWidth * 3))]);
                data[im][i] = BYTE(tmp * (1 - r) + data[i1][i] * r);
            }
            else data[im][i] = BYTE(data[i1][i]);
        }
    }
    /* When image 2 is taller */
    else if (h_info[i1].biHeight < h_info[i2].biHeight && h_info[i1].biWidth == h_info[i2].biWidth) {
        for (unsigned int i = 0; i < h_info[im].biSizeImage; i++) {
            if ((signed int)i < h_info[i1].biSizeImage ) {
                BYTE tmp = (data[i1][i]);
                data[im][i] = BYTE(tmp * r + data[i2][i] * (1 - r));
            }
            else data[im][i] = BYTE(data[i2][i]);
        }
    }
    /* When image 1 is taller */
    else if (h_info[i2].biHeight < h_info[i1].biHeight && h_info[i2].biWidth == h_info[i1].biWidth) {
        for (unsigned int i = 0; i < h_info[im].biSizeImage; i++) {
            if ((signed int)i < h_info[i2].biSizeImage) {
                BYTE tmp = (data[i2][i]);
                data[im][i] = BYTE(tmp * (1 - r) + data[i1][i] * r);
            }
            else data[im][i] = BYTE(data[i1][i]);
        }
    }
    /* When image 1 is taller and wider */
    else if (h_info[i1].biHeight < h_info[i2].biHeight && h_info[i1].biWidth < h_info[i2].biWidth) {
        for (unsigned int i = 0; i < h_info[im].biSizeImage; i++) {
            if ((signed int)i < (h_info[i1].biHeight * h_info[i2].biWidth * 3) && (signed int)(i % (h_info[i2].biWidth * 3)) < (h_info[i1].biWidth * 3)) {
                BYTE tmp = (data[i1][(i / (h_info[i2].biWidth * 3)) * (h_info[i1].biWidth * 3) + (i % (h_info[i2].biWidth * 3))]);
                data[im][i] = BYTE(tmp * r + data[i2][i] * (1 - r));
            }
            else data[im][i] = BYTE(data[i2][i]);
        }
    }
    /* When image 2 is taller and wider */
    else if (h_info[i2].biHeight < h_info[i1].biHeight && h_info[i2].biWidth < h_info[i1].biWidth) {
        for (unsigned int i = 0; i < h_info[im].biSizeImage; i++) {
            if ((signed int)i < (h_info[i2].biHeight * h_info[i1].biWidth * 3) && (signed int)(i % (h_info[i1].biWidth * 3)) < (h_info[i2].biWidth * 3)) {
                BYTE tmp = (data[i2][(i / (h_info[i1].biWidth * 3)) * (h_info[i2].biWidth * 3) + (i % (h_info[i1].biWidth * 3))]);
                data[im][i] = BYTE(tmp * (1- r) + data[i1][i] * r);
            }
            else data[im][i] = BYTE(data[i1][i]);
        }
    }
    /* When image 1 is taller and image 2 is wider */
    else if (h_info[i1].biHeight > h_info[i2].biHeight && h_info[i1].biWidth < h_info[i2].biWidth) {
        for (unsigned int i = 0; i < h_info[im].biSizeImage; i++) {
            data[im][i] = 0;
        }
        for (unsigned int i = 0; i < h_info[im].biSizeImage; i++) {
            if ((signed int)(i % (h_info[i2].biWidth * 3)) < (h_info[i1].biWidth * 3)) {
                BYTE tmp = (data[i1][(i / (h_info[i2].biWidth * 3)) * (h_info[i1].biWidth * 3) + (i % (h_info[i2].biWidth * 3))]);
                data[im][i] = BYTE(tmp * r);
            }
        }
        for (unsigned int i = 0; i < h_info[im].biSizeImage; i++) {
            if ((signed int)i < h_info[i2].biSizeImage) {
                BYTE tmp = (data[i2][i]);
                data[im][i] += BYTE(tmp * (1 - r));
            }
        }
    }
    /* When image 2 is taller and image 1 is wider */
    else if (h_info[i1].biHeight < h_info[i2].biHeight && h_info[i1].biWidth > h_info[i2].biWidth) {
        for (unsigned int i = 0; i < h_info[im].biSizeImage; i++) {
            data[im][i] = 0;
        }
        for (unsigned int i = 0; i < h_info[im].biSizeImage; i++) {
            if ((signed int)(i % (h_info[i1].biWidth * 3)) < (h_info[i2].biWidth * 3)) {
                BYTE tmp = (data[i2][(i / (h_info[i1].biWidth * 3)) * (h_info[i2].biWidth * 3) + (i % (h_info[i1].biWidth * 3))]);
                data[im][i] = BYTE(tmp * (1 - r));
            }
        }
        for (unsigned int i = 0; i < h_info[im].biSizeImage; i++) {
            if ((signed int)i < h_info[i1].biSizeImage) {
                BYTE tmp = (data[i1][i]);
                data[im][i] += BYTE(tmp * r);
            }
        }
    }
}

void Bitmap::save(const char* filename, int n)
{
    std::ofstream file;

    if (!data[n]) {
        cout << "Data for new file corrupt ! ";
        exit(0);
    }

    file.open(filename, std::fstream::binary);
    if (!file.is_open()) {
        cout << "Unable to write file !- " << filename;
        exit(0);
    }

    file.write((char*)&(h_file[n]), sizeof(BITMAPFILEHEADER));

    file.write((char*)&h_info[n], sizeof(BITMAPINFOHEADER));

    file.write((char*)data[n], h_info[n].biSizeImage);

    file.close();
}

int main(int argc, char* argv[])
{
    if (argc == 5) {

        float ratio = 0.0;
        int tmp = sscanf(argv[3], "%f", &ratio);
        if (ratio < 0.00f || ratio > 1.00f) {
            std::cout << "Wrong ratio " << argv[3] << " !" << std::endl;
            std::cout << "Must between 0.0 to 1.0 " << " !" << std::endl;
            return 0;
        }

        Bitmap x{};

        x.load(argv[1],0);
        x.load(argv[2],1);
        x.merge(0, 1, ratio, 2);
        x.save(argv[4],2);
    }
    else if (argc == 3) {

        float ratio = 0.5;

        Bitmap x{};

        x.load(argv[1], 0);
        x.load(argv[2], 1);
        x.merge(0, 1, ratio, 2);
        x.save("merge.bmp", 2);
    }
    else {
        cout << "Invalid input !";
        cin.ignore(1);
    }
}