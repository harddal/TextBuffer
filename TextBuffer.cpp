#include "TextBuffer.h"

TextBuffer::TextBuffer(int width, int height, int char_width, int char_height)
{
	W = width;
	H = height;
	Dirty = true;

	CharW = char_width; 
	CharH = char_height; 

	ForeDataRaw.resize(width * height * 4);
	BackDataRaw.resize(width * height * 4);

	sf::Image foreImage;
	foreImage.create(W, H);
	ForeData.loadFromImage(foreImage);
	ForeData.setSmooth(false);

	sf::Image backImage;
	backImage.create(W, H);
	BackData.loadFromImage(backImage);
	BackData.setSmooth(false);

	RT.create(W * CharW, H * CharH);
	RT.setSmooth(false);

	Sprite.setTexture(RT.getTexture());

	TextBufferShader.loadFromMemory(TextBufferVert, TextBufferFrag);
	TextStates.shader = &TextBufferShader;

	ScreenQuad.resize(4);
	ScreenQuad[0] = sf::Vertex(sf::Vector2f(0, 0), sf::Color::White, sf::Vector2f(0, 0));
	ScreenQuad[1] = sf::Vertex(sf::Vector2f(RT.getSize().x, 0), sf::Color::White, sf::Vector2f(1, 0));
	ScreenQuad[2] = sf::Vertex(sf::Vector2f(RT.getSize().x, RT.getSize().y), sf::Color::White, sf::Vector2f(1, 1));
	ScreenQuad[3] = sf::Vertex(sf::Vector2f(0, RT.getSize().y), sf::Color::White, sf::Vector2f(0, 1));

	Clear();
}
TextBuffer::~TextBuffer()
{

}

void TextBuffer::SetFontTexture(sf::Texture Fnt, int CharW, int CharH)
{
	this->CharW = CharW;
	this->CharH = CharH;
	ASCIIFont = Fnt;
	Dirty = true;
}

void TextBuffer::Set(int X, int Y, char C, sf::Color Fg, sf::Color Bg)
{
	Set(Y * W + X, C, Fg, Bg);
}

void TextBuffer::Set(int X, int Y, sf::Color Fg, sf::Color Bg)
{
	Set(Y * W + X, Fg, Bg);
}

void TextBuffer::Set(int Idx, sf::Color Fg, sf::Color Bg)
{
	Idx *= 4;

	ForeDataRaw[Idx] = Fg.r;
	ForeDataRaw[Idx + 1] = Fg.g;
	ForeDataRaw[Idx + 2] = Fg.b;
	BackDataRaw[Idx] = Bg.r;
	BackDataRaw[Idx + 1] = Bg.g;
	BackDataRaw[Idx + 2] = Bg.b;
	BackDataRaw[Idx + 3] = Bg.a;

	Dirty = true;
}

void TextBuffer::Set(int Idx, char C, sf::Color Fg, sf::Color Bg)
{
	Set(Idx, Fg, Bg);
	ForeDataRaw[Idx * 4 + 3] = C;
	Dirty = true;
}

TextBufferEntry TextBuffer::Get(int X, int Y)
{
	return Get(Y * W + X);
}

TextBufferEntry TextBuffer::Get(int Idx)
{
	Idx *= 4;

	return TextBufferEntry(ForeDataRaw[Idx + 3], 
		sf::Color(ForeDataRaw[Idx], ForeDataRaw[Idx + 1], ForeDataRaw[Idx + 2]), 
		sf::Color(BackDataRaw[Idx], BackDataRaw[Idx + 1], BackDataRaw[Idx + 2], BackDataRaw[Idx + 3]));
}

void TextBuffer::Clear(char C)
{
	Clear(C, sf::Color::White, sf::Color::Black);
}

void TextBuffer::Clear(char C, sf::Color Fg, sf::Color Bg)
{
	for (int i = 0; i < W * H; i++)
	{
		Set(i, C, Fg, Bg);
	}
}

void TextBuffer::Print(int X, int Y, const std::string &Str)
{
	Print(Y * W + X, Str);
}

void TextBuffer::Print(int X, int Y, const std::string &Str, sf::Color Fg, sf::Color Bg)
{
	Print(Y * W + X, Str, Fg, Bg);
}

void TextBuffer::Print(int I, const std::string &Str)
{
	Print(I, Str, sf::Color(192, 192, 192), sf::Color::Black);
}

void TextBuffer::Print(int I, const std::string &Str, sf::Color Fg, sf::Color Bg)
{
	for (int i = 0; i < Str.length(); i++)
	{
		Set(I + i, Str[i], Fg, Bg);
	}
}

void TextBuffer::Update()
{
	if (!Dirty)
		return;

	Dirty = false;

	ForeData.update(ForeDataRaw.data());
	BackData.update(BackDataRaw.data());

	TextBufferShader.setParameter("font", ASCIIFont);
	TextBufferShader.setParameter("foredata", ForeData);
	TextBufferShader.setParameter("backdata", BackData);
	TextBufferShader.setParameter("buffersize", W, H);
	TextBufferShader.setParameter("fontsizes", CharW, CharH, ASCIIFont.getSize().x / CharW, ASCIIFont.getSize().y / CharH);
	
	RT.clear(sf::Color::Transparent);
	RT.draw(ScreenQuad.data(), 4, sf::PrimitiveType::Quads, TextStates);
	RT.display();
}

void TextBuffer::Draw(sf::RenderWindow &window)
{
	Update();

	window.draw(Sprite);
}
