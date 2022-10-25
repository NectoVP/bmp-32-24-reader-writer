#pragma once;
#include <vector>
#include <iostream>
#include <fstream>

struct Color {
	float r, g, b, a;
	Color();
	Color(float r, float g, float b, float a);
	~Color();
};
class Image {
public:
	Image(int width, int heigth);
	Image();
	~Image();
	Color GetColor(int x, int y) const;
	void SetColor(const Color& color, int x, int y);
	void Export(const char* path) const;
	void Read(const char* path);


private:
	int temp;
	int m_width;
	int m_height;
	std::vector <Color> m_colors;
	int channels;
};
void Image::Read(const char* path) {
	std::ifstream f;
	f.open(path, std::ios::in | std::ios::binary);
	if (!f.is_open()) {
		std::cout << "eror file has not opened";
		return;
	}
	const int FileHeaderSize = 14;
	const int InfoHeaderSize = 40;
	unsigned char FileHeader[FileHeaderSize];
	f.read(reinterpret_cast<char*>(FileHeader), FileHeaderSize);
	unsigned char InfoHeader[InfoHeaderSize];

	if (FileHeader[0] != 'B' || FileHeader[1] != 'M') {
		std::cout << "that`s not a bit map image\n";
		return;
	}

	f.read(reinterpret_cast<char*>(InfoHeader), InfoHeaderSize);
	int FileSize = FileHeader[2] + (FileHeader[3] << 8) + (FileHeader[4] << 16) + (FileHeader[5] << 24);
	m_width = InfoHeader[4] + (InfoHeader[5] << 8) + (InfoHeader[6] << 16) + (InfoHeader[7] << 24);
	m_height = InfoHeader[8] + (InfoHeader[9] << 8) + (InfoHeader[10] << 16) + (InfoHeader[11] << 24);
	m_colors.resize(m_width * m_height);
	const int padding = (4 - ((m_width * 3) % 4)) % 4;
	channels = (int)InfoHeader[14];
	temp = (int)FileHeader[10];
	f.seekg(temp, f.beg);
	for (int y = 0; y < m_height; y++)
	{
		for (int x = 0; x < m_width; x++) {
			if (channels == 32) {
				unsigned char color[4];
				f.read(reinterpret_cast<char*> (color), 4);
				m_colors[y * m_width + x].a = static_cast<float>(color[3]) / 255.0f;
				m_colors[y * m_width + x].r = static_cast<float>(color[2]) / 255.0f;
				m_colors[y * m_width + x].g = static_cast<float>(color[1]) / 255.0f;
				m_colors[y * m_width + x].b = static_cast<float>(color[0]) / 255.0f;
			}
			else {
				unsigned char color[3];
				f.read(reinterpret_cast<char*> (color), 3);
				m_colors[y * m_width + x].r = static_cast<float>(color[2]) / 255.0f;
				m_colors[y * m_width + x].g = static_cast<float>(color[1]) / 255.0f;
				m_colors[y * m_width + x].b = static_cast<float>(color[0]) / 255.0f;
			}
		}
		f.ignore(padding);

	}
	f.close();
	std::cout << "file read\n";

}
Color::Color()
	: r(0), g(0), b(0), a(0)
{

}

Color::Color(float r, float g, float b, float a)
	: r(r), g(g), b(b), a(a)
{
}

Color::~Color()
{
}

Image::Image(int width, int height)
	: m_width(width), m_height(height), m_colors(std::vector<Color>(width* height))
{
}

Image::~Image()
{
}

Image::Image()
	: m_width(0), m_height(0), m_colors(std::vector<Color>(0))
{
}

Color Image::GetColor(int x, int y) const
{
	return m_colors[y * m_width + x];
}

void Image::SetColor(const Color& color, int x, int y)
{
	m_colors[y * m_width + x].r = color.r;
	m_colors[y * m_width + x].g = color.g;
	m_colors[y * m_width + x].b = color.b;
	m_colors[y * m_width + x].a = color.a;
}

void Image::Export(const char* path) const
{
	std::ofstream f;
	f.open(path, std::ios::out | std::ios::binary);
	if (!f.is_open()) {
		std::cout << "error file has not openned";
		return;
	}
	unsigned char bmpPad[3] = { 0,0,0 };
	const int padding = (4 - ((m_width * 3) % 4)) % 4;
	const int FileHeaderSize = 14;
	const int InfoHeaderSize = 40;
	const int FileSize = FileHeaderSize + InfoHeaderSize + m_width * m_height * channels / 8 + padding * m_height;
	unsigned char FileHeader[FileHeaderSize] = { 'B','M', 0,0,0,0, 0,0, 0,0, 0,0,0,0 };
	unsigned char InfoHeader[InfoHeaderSize] = { 40,0,0,0, 0,0,0,0, 0,0,0,0, 1,0, 0,0 };

	FileHeader[2] = FileSize;
	FileHeader[3] = FileSize >> 8;
	FileHeader[4] = FileSize >> 16;
	FileHeader[5] = FileSize >> 24;
	FileHeader[10] = FileHeaderSize + InfoHeaderSize;
	InfoHeader[4] = m_width;
	InfoHeader[5] = m_width >> 8;
	InfoHeader[6] = m_width >> 16;
	InfoHeader[7] = m_width >> 24;
	InfoHeader[8] = m_height;
	InfoHeader[9] = m_height >> 8;
	InfoHeader[10] = m_height >> 16;
	InfoHeader[11] = m_height >> 24;
	InfoHeader[14] = (unsigned char)channels;

	f.write(reinterpret_cast<char*>(FileHeader), FileHeaderSize);
	f.write(reinterpret_cast<char*>(InfoHeader), InfoHeaderSize);

	for (int y = 0; y < m_height; y++) {
		for (int x = 0; x < m_width; x++)
		{
			unsigned char r = static_cast<unsigned char>(GetColor(x, y).r * 255.0f);
			unsigned char g = static_cast<unsigned char>(GetColor(x, y).g * 255.0f);
			unsigned char b = static_cast<unsigned char>(GetColor(x, y).b * 255.0f);
			unsigned char a = static_cast<unsigned char>(GetColor(x, y).a * 255.0f);
			if (channels == 32) {
				unsigned char color[] = { b, g, r, a };
				f.write(reinterpret_cast<char*>(color), 4);
			}
			else {
				unsigned char color[] = { b, g, r};
				f.write(reinterpret_cast<char*>(color), 3);
			}
		}
		f.write(reinterpret_cast<char*>(bmpPad), padding);

	}
	f.close();
	std::cout << "File created\n";
}
