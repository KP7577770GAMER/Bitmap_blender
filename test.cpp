#include <Windows.h>
#include <iostream>
#include <fstream>

using namespace std;

typedef BITMAPINFOHEADER infoheader;
typedef BITMAPFILEHEADER fileheader;

const WORD BMP_FORMAT = 0x4D42;

float ratio;

class Bitmap2
{
private:
    BYTE* data_1;
    infoheader h_info_1;
    fileheader h_file_1;

    BYTE* data_2;
    infoheader h_info_2;
    fileheader h_file_2;

    BYTE* data_m;
    infoheader h_info_m;
    fileheader h_file_m;

    WORD Type_1();
    DWORD OffBits_1();
    DWORD SizeInBytes_1();

    WORD Type_2();
    DWORD OffBits_2();
    DWORD SizeInBytes_2();
public:
    void Load_1(const char*);
    void Load_2(const char*);
    void Save(const char*);
};

void Bitmap2::Load_1(const char* filename)
{
    std::ifstream file;

    file.open(filename, std::fstream::binary);
    if (!file.is_open()) {
        cout << "File not found !- " << filename;
        exit(0);
    }

    file.read((char*)&(h_file_1), sizeof(fileheader));

    //
    if (Type_1() != BMP_FORMAT)
    {
        file.close();
        return;
    }

    file.read((char*)&h_info_1, sizeof(infoheader));

    //
    file.seekg(OffBits_1());

    data_1 = new BYTE[SizeInBytes_1()];

    if (!data_1)
    {
        delete data_1;
        file.close();
        return;
    }

    file.read((char*)data_1, SizeInBytes_1());

    if (data_1 == NULL)
    {
        file.close();
        return;
    }

    file.close();
}

void Bitmap2::Load_2(const char* filename)
{
    std::ifstream file;

    file.open(filename, std::fstream::binary);
    if (!file.is_open())
        return;

    file.read((char*)&(h_file_2), sizeof(fileheader));

    //
    if (Type_2() != BMP_FORMAT)
    {
        file.close();
        return;
    }

    file.read((char*)&h_info_2, sizeof(infoheader));

    //
    file.seekg(OffBits_2());

    data_2 = new BYTE[SizeInBytes_2()];

    if (!data_2)
    {
        delete data_2;
        file.close();
        return;
    }

    file.read((char*)data_2, SizeInBytes_2());

    if (data_2 == NULL)
    {
        file.close();
        return;
    }

    file.close();
}

void Bitmap2::Save(const char* filename)
{
    h_info_m.biWidth = h_info_1.biWidth;
    if (h_info_m.biWidth < h_info_2.biWidth)h_info_m.biWidth = h_info_2.biWidth;
    h_info_m.biHeight = h_info_1.biHeight;
    if (h_info_m.biHeight < h_info_2.biHeight)h_info_m.biHeight = h_info_2.biHeight;
    h_info_m.biSizeImage = h_info_m.biWidth * h_info_m.biHeight * 3;

    data_m = new BYTE[h_info_m.biSizeImage];

    std::cout << h_info_1.biSizeImage << std::endl;
    std::cout << h_info_2.biSizeImage << std::endl;
    std::cout << h_info_m.biSizeImage << std::endl;

    for (unsigned int i = 0; i < h_info_m.biSizeImage; i++) {
        //data_m[i] = BYTE(data_1[i] * ratio + data_2[i] * (1-ratio));
        if (h_info_2.biWidth == h_info_1.biWidth && h_info_2.biHeight == h_info_1.biHeight) {
            data_m[i] = BYTE(data_1[i] * ratio + data_2[i] * (1 - ratio));
        }
        else if (h_info_2.biWidth > h_info_1.biWidth) {
            int tmp = h_info_2.biWidth - h_info_1.biWidth;

            if (i % (h_info_2.biWidth) <= h_info_1.biWidth)
                data_m[i] = BYTE(data_1[i] * ratio + data_2[i] * (1 - ratio));
            else data_m[i] = BYTE(data_2[i]);
        }
    }

    std::ofstream file;

    if (!data_m/**/)
        return;

    file.open(filename, std::fstream::binary);
    if (!file.is_open())
        return;

    file.write((char*)&(h_file_1/**/), sizeof(fileheader));

    //
    if (Type_1/**/() != BMP_FORMAT)
    {
        file.close();
        return;
    }

    file.write((char*)&h_info_1/**/, sizeof(infoheader));

    file.write((char*)data_m/**/, SizeInBytes_1/**/());

    file.close();
}

WORD Bitmap2::Type_1()
{
    return h_file_1.bfType;
}

DWORD Bitmap2::OffBits_1()
{
    return h_file_1.bfOffBits;
}

DWORD Bitmap2::SizeInBytes_1()
{
    return h_info_1.biSizeImage;
}

WORD Bitmap2::Type_2()
{
    return h_file_2.bfType;
}

DWORD Bitmap2::OffBits_2()
{
    return h_file_2.bfOffBits;
}

DWORD Bitmap2::SizeInBytes_2()
{
    return h_info_2.biSizeImage;
}

int main(int argc, char* argv[])
{
    if (argc == 5) {

        int tmp = sscanf(argv[3], "%f", &ratio);
        if (ratio < 0.00f || ratio > 1.00f) {
            std::cout << "Wrong ratio " << argv[3] << " !" << std::endl;
            std::cout << "Must between 0.0 to 1.0 " << " !" << std::endl;
            return 0;
        }

        Bitmap2 x{};

        x.Load_1(argv[1]);
        x.Load_2(argv[2]);
        x.Save(argv[4]);
    }
    else {
        std::cout << "Wrong input";
    }
}