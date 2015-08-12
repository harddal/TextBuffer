#ifndef TEXT_BUFFER_H
#define TEXT_BUFFER_H

#include <array>
#include <vector>

#include "SFML/Graphics.hpp"

struct TextBufferEntry
{
	char Char;
	sf::Color Fore, Back;

	TextBufferEntry(char Char, sf::Color Fore, sf::Color Back) 
	{
		this->Char = Char;
		this->Fore = Fore;
		this->Back = Back;
	}

	TextBufferEntry(char Char) {
		TextBufferEntry(Char, sf::Color(192, 192, 192), sf::Color::Black); }

	TextBufferEntry(sf::Color Fore, sf::Color Back) {
		TextBufferEntry((char)0, Fore, Back); }

	TextBufferEntry(sf::Color Colors) {
		TextBufferEntry(Colors, Colors); }
};

class TextBuffer
{
public:
	TextBuffer(int width, int height, int char_width = 8, int char_heigh= 12);
	~TextBuffer();

	const int &getBufferWidth() const { return W; }
	const int &getBufferHeight() const { return H; }
	const int &getCharWidth() const { return CharW; }
	const int &getCharHeight() const { return CharH; }

	sf::Sprite getSprite() const { return Sprite; }
	void setSprite(sf::Sprite value) { Sprite = value; }

	void SetFontTexture(sf::Texture Fnt, int CharW = 8, int CharH = 12);

	void Set(int X, int Y, char C, sf::Color Fg, sf::Color Bg);
	void Set(int X, int Y, sf::Color Fg, sf::Color Bg);
	void Set(int Idx, sf::Color Fg, sf::Color Bg);
	void Set(int Idx, char C, sf::Color Fg, sf::Color Bg);

	TextBufferEntry Get(int X, int Y);
	TextBufferEntry Get(int Idx);
	TextBufferEntry &operator [](int Idx) { return Get(Idx); }
	TextBufferEntry &operator ()(int X, int Y) { return Get(X, Y); }

	void Clear(char C = 0);
	void Clear(char C, sf::Color Fg, sf::Color Bg);

	void Print(int X, int Y, const std::string &Str);
	void Print(int X, int Y, const std::string &Str, sf::Color Fg, sf::Color Bg);
	void Print(int I, const std::string &Str);
	void Print(int I, const std::string &Str, sf::Color Fg, sf::Color Bg);

	void Update(sf::RenderWindow &window);

private:
	sf::Shader TextBufferShader;

	int W, H, CharW, CharH;
	bool Dirty;

	sf::RenderTexture RT;
	std::vector<sf::Vertex> ScreenQuad;
	sf::RenderStates TextStates;
	sf::Texture ForeData, BackData, ASCIIFont;
	std::vector<sf::Uint8> ForeDataRaw, BackDataRaw;
	sf::Sprite Sprite;

	const char* TextBufferVert =
	R"(
		#version 110

		void main() {
		gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
		gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0;
		gl_FrontColor = gl_Color;}
	)";

	const char* TextBufferFrag =
		R"(
		#version 120

		uniform sampler2D font;
		uniform sampler2D foredata;
		uniform sampler2D backdata;
		uniform vec2 buffersize;
		uniform vec4 fontsizes;

		void main() {
		vec4 fore = texture2D(foredata, gl_TexCoord[0].xy);
		vec4 back = texture2D(backdata, gl_TexCoord[0].xy);
		float chr = 255.0f * fore.a;

		vec2 fontpos = vec2(floor(mod(chr, fontsizes.z)) * fontsizes.x, floor(chr / fontsizes.w) * fontsizes.y);
		vec2 offset = vec2(mod(floor(gl_TexCoord[0].x * (buffersize.x * fontsizes.x)), fontsizes.x),
			mod(floor(gl_TexCoord[0].y * (buffersize.y * fontsizes.y)) + 0.5f, fontsizes.y));

		vec4 fontclr = texture2D(font, (fontpos + offset) / vec2(fontsizes.x * fontsizes.z, fontsizes.y * fontsizes.w));
		gl_FragColor = mix(back, vec4(fore.rgb, 1.0f), fontclr.r);}
	)";

};

#endif