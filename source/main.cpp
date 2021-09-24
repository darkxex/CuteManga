#ifdef __SWITCH__
#include <unistd.h>
#include <switch.h>
#include <dirent.h>
#endif 
#ifndef __SWITCH__
#define _CRT_SECURE_NO_WARNINGS
#endif


//test

#include <SDL.h>
#include <SDL_thread.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <iostream>
#include <string>
#include <curl/curl.h>

#include <math.h> 

#include <errno.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string>
#include <cmath>
#include <iostream>
#include <math.h>  
#include <Vector>
#include <dirent.h>

// Empty strings are invalid.
#ifdef __SWITCH__
SwkbdTextCheckResult Keyboard_ValidateText(char *string, size_t size) {
	if (strcmp(string, "") == 0) {
		strncpy(string, "The name cannot be empty.", size);
		return SwkbdTextCheckResult_Bad;
	}

	return SwkbdTextCheckResult_OK;
}

const char *Keyboard_GetText(const char *guide_text, const char *initial_text) {
	Result ret = 0;
	SwkbdConfig swkbd;
	static char input_string[256];

	if (R_FAILED(ret = swkbdCreate(&swkbd, 0))) {
		swkbdClose(&swkbd);
		return "";
	}

	swkbdConfigMakePresetDefault(&swkbd);

	if (strlen(guide_text) != 0)
		swkbdConfigSetGuideText(&swkbd, guide_text);

	if (strlen(initial_text) != 0)
		swkbdConfigSetInitialText(&swkbd, initial_text);

	swkbdConfigSetTextCheckCallback(&swkbd, Keyboard_ValidateText);

	if (R_FAILED(ret = swkbdShow(&swkbd, input_string, sizeof(input_string)))) {
		swkbdClose(&swkbd);
		return "";
	}

	swkbdClose(&swkbd);
	return input_string;
}
#endif 




bool	isFileExist(const char *file)
{
	struct stat	st = { 0 };

	if (stat(file, &st) == -1) {
		return (false);
	}

	return (true);
}

//////////////////////////////////aqu� empieza el pc.
//Screen dimension constants
const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;
enum states { selectmanga, readmanga };
int statenow = selectmanga;
std::string  urltodownload = "";
int porcendown = 0;
int sizeestimated = 0;
std::string temporallink = "";
bool ahorro = false;
int cancelcurl = 0;
bool preview = false;
int searchchapter = 0;
int selectchapter = 0;
int selectpage = 0;
float neutralsize = 1;
int widthnew = 0;
int heightnew = 0;
int cascademode = 0;
bool helppage = false;
bool existfoldermain = true;
int basexmain = 20;
int baseymain = 20;
bool cascadeactivated = false;
//Texture wrapper class
class LTexture
{
public:
	//Initializes variables
	LTexture();

	//Deallocates memory
	~LTexture();

	//Loads image at specified path
	bool loadFromFile(std::string path);
	bool loadFromFileCustom(std::string path,int h, int w);
	//Creates image from font string
	bool loadFromRenderedText(TTF_Font *fuente, std::string textureText, SDL_Color textColor);
	//Creates image from font string
	bool loadFromRenderedTextWrap(TTF_Font *fuente, std::string textureText, SDL_Color textColor, Uint32 size);
	//Deallocates texture
	void free();

	//Set color modulation
	void setColor(Uint8 red, Uint8 green, Uint8 blue);

	//Set blending
	void setBlendMode(SDL_BlendMode blending);

	//Set alpha modulation
	void setAlpha(Uint8 alpha);

	//Renders texture at given point
	void render(int x, int y, SDL_Rect* clip = NULL, double angle = 0.0, SDL_Point* center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE);
	//Renders texture at given point
	void customrender(int x, int y,int w,int h, SDL_Rect* clip = NULL, double angle = 0.0, SDL_Point* center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE);

	//Gets image dimensions
	int getWidth();
	int getHeight();

private:
	//The actual hardware texture
	SDL_Texture* mTexture;

	//Image dimensions
	int mWidth;
	int mHeight;
};

//Frees media and shuts down SDL
void close();

//The window we'll be rendering to
SDL_Window* gWindow = NULL;

//The window renderer
SDL_Renderer* gRenderer = NULL;

//Globally used font
TTF_Font *gFont = NULL;
TTF_Font *gFontcapit = NULL;
TTF_Font *gFont2 = NULL;
TTF_Font *gFont3 = NULL;
//Rendered texture
LTexture gTextTexture;
LTexture Farest;
LTexture Heart;
LTexture Pagemanga;
LTexture lupa;
LTexture TPreview;

std::vector<LTexture> arraypage;
LTexture::LTexture()
{
	//Initialize
	mTexture = NULL;
	mWidth = 0;
	mHeight = 0;
}

LTexture::~LTexture()
{
	//Deallocate
	free();
}

bool LTexture::loadFromFile(std::string path)
{
	//Get rid of preexisting texture
	free();

	//The final texture
	SDL_Texture* newTexture = NULL;

	//Load image at specified path
	SDL_Surface* loadedSurface = IMG_Load((path.c_str()));
	if (loadedSurface == NULL)
	{
		printf("Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError());
	}
	else
	{
		//Color key image
		SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, 0, 0xFF, 0xFF));

		//Create texture from surface pixels
		newTexture = SDL_CreateTextureFromSurface(gRenderer, loadedSurface);
		if (newTexture == NULL)
		{
			printf("Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
		}
		else
		{
			//Get image dimensions
			mWidth = loadedSurface->w;
			mHeight = loadedSurface->h;
		}

		//Get rid of old loaded surface
		SDL_FreeSurface(loadedSurface);
	}

	//Return success
	mTexture = newTexture;
	return mTexture != NULL;
}


bool LTexture::loadFromFileCustom(std::string path, int h, int w)
{
	//Get rid of preexisting texture
	free();

	//The final texture
	SDL_Texture* newTexture = NULL;

	//Load image at specified path
	SDL_Surface* loadedSurface = IMG_Load(path.c_str());
	if (loadedSurface == NULL)
	{
		printf("Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError());
	}
	else
	{
		//Color key image
		SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, 0, 0xFF, 0xFF));

		//Create texture from surface pixels
		newTexture = SDL_CreateTextureFromSurface(gRenderer, loadedSurface);
		if (newTexture == NULL)
		{
			printf("Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
		}
		else
		{
			//Get image dimensions
			mWidth = w;
			mHeight = h;
		}

		//Get rid of old loaded surface
		SDL_FreeSurface(loadedSurface);
	}

	//Return success
	mTexture = newTexture;
	return mTexture != NULL;
}
bool LTexture::loadFromRenderedText(TTF_Font *fuente, std::string textureText, SDL_Color textColor)
{
	//Get rid of preexisting texture
	free();

	//Render text surface
	SDL_Surface* textSurface = TTF_RenderText_Blended(fuente, textureText.c_str(), textColor);

	if (textSurface == NULL)
	{
		printf("Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError());
	}
	else
	{
		//Create texture from surface pixels
		mTexture = SDL_CreateTextureFromSurface(gRenderer, textSurface);
		if (mTexture == NULL)
		{
			printf("Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError());
		}
		else
		{
			//Get image dimensions
			mWidth = textSurface->w;
			mHeight = textSurface->h;
		}

		//Get rid of old surface
		SDL_FreeSurface(textSurface);
	}

	//Return success
	return mTexture != NULL;
}
bool LTexture::loadFromRenderedTextWrap(TTF_Font *fuente, std::string textureText, SDL_Color textColor, Uint32 size)
{
	//Get rid of preexisting texture
	free();

	//Render text surface
	SDL_Surface* textSurface = TTF_RenderUTF8_Blended_Wrapped(fuente, textureText.c_str(), textColor,size);

	if (textSurface == NULL)
	{
		printf("Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError());
	}
	else
	{
		//Create texture from surface pixels
		mTexture = SDL_CreateTextureFromSurface(gRenderer, textSurface);
		if (mTexture == NULL)
		{
			printf("Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError());
		}
		else
		{
			//Get image dimensions
			mWidth = textSurface->w;
			mHeight = textSurface->h;
		}

		//Get rid of old surface
		SDL_FreeSurface(textSurface);
	}

	//Return success
	return mTexture != NULL;
}

void LTexture::free()
{
	//Free texture if it exists
	if (mTexture != NULL)
	{
		SDL_DestroyTexture(mTexture);
		mTexture = NULL;
		mWidth = 0;
		mHeight = 0;
	}
}

void LTexture::setColor(Uint8 red, Uint8 green, Uint8 blue)
{
	//Modulate texture rgb
	SDL_SetTextureColorMod(mTexture, red, green, blue);
}

void LTexture::setBlendMode(SDL_BlendMode blending)
{
	//Set blending function
	SDL_SetTextureBlendMode(mTexture, blending);
}

void LTexture::setAlpha(Uint8 alpha)
{
	//Modulate texture alpha
	SDL_SetTextureAlphaMod(mTexture, alpha);
}

void LTexture::render(int x, int y, SDL_Rect* clip, double angle, SDL_Point* center, SDL_RendererFlip flip)
{
	//Set rendering space and render to screen
	SDL_Rect renderQuad = { x, y, mWidth, mHeight };

	//Set clip rendering dimensions
	if (clip != NULL)
	{
		renderQuad.w = clip->w;
		renderQuad.h = clip->h;
	}

	//Render to screen
	SDL_RenderCopyEx(gRenderer, mTexture, clip, &renderQuad, angle, center, flip);
}
void LTexture::customrender(int x, int y, int w,int h, SDL_Rect* clip, double angle, SDL_Point* center, SDL_RendererFlip flip)
{
	//Set rendering space and render to screen
	SDL_Rect renderQuad = { x, y, w,h };

	//Set clip rendering dimensions
	if (clip != NULL)
	{
		renderQuad.w = clip->w;
		renderQuad.h = clip->h;
	}

	//Render to screen
	SDL_RenderCopyEx(gRenderer, mTexture, clip, &renderQuad, angle, center, flip);
}

int LTexture::getWidth()
{
	return mWidth;
}

int LTexture::getHeight()
{
	return mHeight;
}





void close()
{
	//Free loaded images
	gTextTexture.free();
	Farest.free();
	Heart.free();
	TPreview.free();
	Pagemanga.free();
	lupa.free();
	for (int x = 0; x < arraypage.size(); x++)
	{
		arraypage[x].free();
	}
	//Free global font
	TTF_CloseFont(gFont);
	gFont = NULL;

	TTF_CloseFont(gFontcapit);
	gFontcapit = NULL;
	TTF_CloseFont(gFont2);
	gFont2 = NULL;
	TTF_CloseFont(gFont3);
	gFont3 = NULL;
	//Destroy window	
	SDL_DestroyRenderer(gRenderer);
	SDL_DestroyWindow(gWindow);
	gWindow = NULL;
	gRenderer = NULL;

	//Quit SDL subsystems
	TTF_Quit();
	IMG_Quit();
	SDL_Quit();
}

static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
	((std::string*)userp)->append((char*)contents, size * nmemb);
	return size * nmemb;
}
size_t write_data(void *ptr, size_t size, size_t nmemb, FILE *stream) {
	size_t written = fwrite(ptr, size, nmemb, stream);
	return written;
}


void replace(std::string& subject, const std::string& search,
	const std::string& replace) {
	size_t pos = 0;
	while ((pos = subject.find(search, pos)) != std::string::npos) {
		subject.replace(pos, search.length(), replace);
		pos += replace.length();
	}
}


void mayus(std::string &s)
{
	bool cap = true;

	for (unsigned int i = 0; i <= s.length(); i++)
	{
		if (isalpha(s[i]) && cap == true)
		{
			s[i] = toupper(s[i]);
			cap = false;
		}
		else if (isspace(s[i]))
		{
			cap = true;
		}
	}
}
int progress_func(void* ptr, double TotalToDownload, double NowDownloaded,
	double TotalToUpload, double NowUploaded)
{


	// ensure that the file to be downloaded is not empty
	// because that would cause a division by zero error later on
	if (TotalToDownload <= 0.0) {
		return 0;
	}

	// how wide you want the progress meter to be
	int totaldotz = 20;
	double fractiondownloaded = NowDownloaded / TotalToDownload;
	// part of the progressmeter that's already "full"
	int dotz = round(fractiondownloaded * totaldotz);

	// create the "meter"
	int ii = 0;
	porcendown = fractiondownloaded * 100;
	sizeestimated = TotalToDownload;
	printf("%3.0f%% [", fractiondownloaded * 100);
	// part  that's full already
	for (; ii < dotz; ii++) {
		printf("=");
	}
	// remaining part (spaces)
	for (; ii < 20; ii++) {
		printf(" ");
	}
	// and back to line begin - do not forget the fflush to avoid output buffering problems!
	printf("]\r");
	fflush(stdout);
	// if you don't return 0, the transfer will be aborted - see the documentation
	if (cancelcurl == 1)
	{
		return 1;
	}

	return 0;

}

std::string gethtml(std::string enlace)
{

	CURL *curl;
	CURLcode res;
	std::string Buffer;

	curl = curl_easy_init();
	if (curl) {

		curl_easy_setopt(curl, CURLOPT_URL, enlace.c_str());
		curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/74.0.3729.169 Safari/537.36");
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &Buffer);
		res = curl_easy_perform(curl);
		curl_easy_cleanup(curl);
		return Buffer;
	}
}
void downloadfile(std::string enlace, std::string directorydown)
{
	
	CURL *curl;
	FILE *fp;
	CURLcode res;

	curl = curl_easy_init();
	if (curl) {

		fp = fopen(directorydown.c_str(), "wb");
		curl_easy_setopt(curl, CURLOPT_URL, enlace.c_str());
		curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/74.0.3729.169 Safari/537.36");
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
		curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);
		// Install the callback function
		curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, progress_func);
		res = curl_easy_perform(curl);
		/* always cleanup */
		curl_easy_cleanup(curl);
		fclose(fp);
	}
}




std::vector<std::string> arraymain;
std::vector<std::string> arraychapter;




int main(int argc, char **argv)

{
#ifdef __SWITCH__
	romfsInit();
	socketInitializeDefault();
	nxlinkStdio();

#endif 
	int basex = 0;
	int basey = 0;

	
	
	
	
	//Start up SDL and create window
	//Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) < 0)
	{
		printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());

	}
	else
	{
		//Set texture filtering to linear
		if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"))
		{
			printf("Warning: Linear texture filtering not enabled!");
		}

		//Create window
#ifdef __SWITCH__
		gWindow = SDL_CreateWindow("sdl2_gles2", 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
#else
		gWindow = SDL_CreateWindow("CuteManga", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
#endif // SWITCH

		
		if (gWindow == NULL)
		{
			printf("Window could not be created! SDL Error: %s\n", SDL_GetError());

		}
		else
		{
			//Create vsynced renderer for window
			gRenderer = SDL_CreateRenderer(gWindow, 0, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
			if (gRenderer == NULL)
			{
				printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());

			}
			else
			{
				//Initialize renderer color
				SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);

				//Initialize PNG loading
				int imgFlags = IMG_INIT_PNG;
				if (!(IMG_Init(imgFlags) & imgFlags))
				{
					printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());

				}

				//Initialize SDL_ttf
				if (TTF_Init() == -1)
				{
					printf("SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());

				}
			}
		}
	}



	

#ifdef __SWITCH__

	gFont = TTF_OpenFont("romfs:/lazy.ttf", 16);
	gFont2 = TTF_OpenFont("romfs:/lazy2.ttf", 150);
	gFontcapit = TTF_OpenFont("romfs:/lazy2.ttf", 100);
	gFont3 = TTF_OpenFont("romfs:/lazy2.ttf", 40);

	
	lupa.loadFromFile("romfs:/lupa.png");
	Heart.loadFromFile("romfs:/heart.png");
	Farest.loadFromFile("romfs:/texture.png");
	//std::string foldercomic = "sdmc:/Kaguya Wants to be Confessed/";
	std::string foldermain = "sdmc:/CuteManga/";
#else
	gFont = TTF_OpenFont("C:\\respaldo2017\\C++\\test\\Debug\\lazy.ttf", 16);
	gFont2 = TTF_OpenFont("C:\\respaldo2017\\C++\\test\\Debug\\lazy2.ttf", 150);
	gFontcapit = TTF_OpenFont("C:\\respaldo2017\\C++\\test\\Debug\\lazy2.ttf", 100);
	gFont3 = TTF_OpenFont("C:\\respaldo2017\\C++\\test\\Debug\\lazy2.ttf", 40);
	Heart.loadFromFile("C:/respaldo2017/C++/CuteManga/Debug/heart.png");
	Farest.loadFromFile("C:/respaldo2017/C++/CuteManga/Debug/texture.png");
	lupa.loadFromFile("C:/respaldo2017/C++/CuteManga/Debug/lupa.png");
	//std::string foldercomic = "C:/respaldo2017/C++/CuteManga/Debug/Kaguya Wants to be Confessed/";
	std::string foldermain = "C:/respaldo2017/C++/CuteManga/Debug/";
	
#endif // SWITCH

	

	arraymain.clear();
	DIR *dirmain;
	struct dirent *entmain;
	if ((dirmain = opendir(foldermain.c_str())) != NULL) {
		/* print all the files and directories within directory */
		while ((entmain = readdir(dirmain)) != NULL) {
			if (strcmp(entmain->d_name, ".") == 0 || strcmp(entmain->d_name, "..") == 0)
				continue;
			if (entmain->d_type == DT_DIR)
			{
				printf("%s\n", entmain->d_name);

				std::string namefile(entmain->d_name);

				arraymain.push_back(namefile);
			}
		}
		closedir(dirmain);
	}
	else {
		existfoldermain = false;
		/* could not open directory */
		//perror("");
		//return EXIT_FAILURE;
	}
	


	bool cascade = false;
	int cascadesize = 0;
	bool separation = false;
	bool adjust = true;
	int speed = 15;
	int zoom = 1;
	SDL_Color textColor = { 50, 50, 50 };

	
	//Main loop flag
	int quit = 0;

	//Event handler
	SDL_Event e;
#ifdef __SWITCH__
	for (int i = 0; i < 2; i++) {
		if (SDL_JoystickOpen(i) == NULL) {
			SDL_Log("SDL_JoystickOpen: %s\n", SDL_GetError());
			SDL_Quit();
			return -1;
		}
	}


	  // Configure our supported input layout: a single player with standard controller styles
    padConfigureInput(1, HidNpadStyleSet_NpadStandard);

    // Initialize the default gamepad (which reads handheld mode inputs as well as the first connected controller)
    PadState pad;
    padInitializeDefault(&pad);
#endif // SWITCH

	
	//While application is running
#ifdef __SWITCH__
	while ( appletMainLoop() && !quit)
#else
	while (!quit )
#endif // SWITCH
	
	{
		
		
#ifdef __SWITCH__
		 // Scan the gamepad. This should be done once for each frame
        padUpdate(&pad);

        // padGetButtonsDown returns the set of buttons that have been
        // newly pressed in this frame compared to the previous one
        u64 kDown = padGetButtonsDown(&pad);

        // padGetButtons returns the set of buttons that are currently pressed
        u64 kHeld = padGetButtons(&pad);

        // padGetButtonsUp returns the set of buttons that have been
        // newly released in this frame compared to the previous one
        u64 kUp = padGetButtonsUp(&pad);

		if (kDown & HidNpadButton_Plus)
		{
			quit = 1;
			
		}
		if (kHeld & HidNpadButton_StickLDown)
		{
			switch (statenow)
			{
			case readmanga:

				if (cascade == false) {

					if ((basey) > (SCREEN_HEIGHT - heightnew))
					{
						basey -= speed;
						if (basey <= SCREEN_HEIGHT - heightnew)
						{
							basey = SCREEN_HEIGHT - heightnew;
						}
					}
				}
				else
				{
					basey -= speed;
				}
				break;

			}

		}
		if (kDown & HidNpadButton_StickLDown)
		{
			switch (statenow)
			{
			

			case selectmanga:
				if (selectchapter < arraymain.size() - 1)
				{
					selectchapter++;

					std::cout << selectchapter << std::endl;
					baseymain = baseymain - 20;
				}
				else {
					selectchapter = 0;

					baseymain = 30;
				}
				break;
			}

		}
		if (kHeld & HidNpadButton_StickLUp)
		{
			switch (statenow)
			{
			case readmanga:

				if (cascade == false) {
					if (basey <= 0)
					{
						basey += speed;
						if (basey > 0)
						{
							basey = 0;
						}
					}
				}
				else
				{
					if (basey <= 0)
					{
						basey += speed;
						if (basey > 0)
						{
							basey = 0;
						}
					}
				}
				break;
			}

		}
		if (kDown & HidNpadButton_StickLUp)
		{
			switch (statenow)
			{
			case selectmanga:
				if (selectchapter > 0)
				{
					selectchapter--;
					std::cout << selectchapter << std::endl;
					baseymain = baseymain + 20;
				}
				else {
					baseymain = 30 - 20 * (arraymain.size() - 1);
					selectchapter = arraymain.size() - 1;
				}
				break;
			}

		}
		if (kHeld & HidNpadButton_StickLLeft)
		{

			switch (statenow)
			{
			case readmanga:

			{
				if (zoom == 2)
				{
					basex += speed;
				}
			}
			break;
			}



		}

		if (kHeld & HidNpadButton_StickLRight)
		{
			switch (statenow)
			{
			case readmanga:
				if (zoom == 2)
				{
					basex -= speed;
				}
				break;
			}


		}

		if (kHeld & HidNpadButton_ZL)
		{
			switch (statenow)
			{
			case readmanga:
				if (neutralsize > 0.05) {
					neutralsize = neutralsize - 0.01;
				}
				break;
			}
		}

		if (kHeld & HidNpadButton_ZR)
		{
			switch (statenow)
			{
			case readmanga:

				neutralsize = neutralsize + 0.01;

				break;
			}
		}

		if (kDown & HidNpadButton_L)
		{
			switch (statenow)
			{
			case readmanga:
			{

				if (cascade == false)
				{
					basey = 0;
					basex = 0;
					zoom = 1;
					if (selectpage > 0)
					{
						Pagemanga.free();
						selectpage--;
						Pagemanga.loadFromFile(arraychapter[selectpage]);

					}
					else
					{
						Pagemanga.free();
						selectpage = 0;
						Pagemanga.loadFromFile(arraychapter[selectpage]);
					}
				}
				else
				{
					zoom = 1;
				}
			}

			break;
			}
		}

		if (kDown & HidNpadButton_R)
		{
			switch (statenow)
			{
			case readmanga:

				if (cascade == false)
				{
					basey = 0;
					basex = 0;
					zoom = 1;
					if (selectpage < arraychapter.size() - 1)
					{
						Pagemanga.free();
						selectpage++;
						Pagemanga.loadFromFile(arraychapter[selectpage]);

					}
				}
				else
				{

					zoom = 1;
				}
				break;
			}

		}

		if (kDown & HidNpadButton_StickR)
		{

			switch (statenow)
			{
			case readmanga:
				if (cascade == false)
				{
					if (zoom == 1)
					{
						zoom = 2;
					}
					else {
						zoom = 1;
						basex = 0;
						basey = 0;
					}
				}
				else
				{
					if (zoom == 1)
					{
						zoom = 2;
					}
					else {
						zoom = 1;
						basex = 0;

					}
				}
				break;
			}

		}

		if (kDown & HidNpadButton_A)
		{
			if (existfoldermain == true) {
				switch (statenow)
				{
				case readmanga:
					if (cascade == false)
					{
						basey = 0;
						adjust = !adjust;
					}
					else
					{

						adjust = !adjust;
					}
					break;
				case selectmanga:
					std::string foldertoread = foldermain + arraymain[selectchapter] + "/";
					arraychapter.clear();
					DIR *dir;
					struct dirent *ent;
					if ((dir = opendir(foldertoread.c_str())) != NULL) {

						while ((ent = readdir(dir)) != NULL) {
							if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0)
								continue;
							printf("%s\n", ent->d_name);

							std::string namefile(ent->d_name);
							if (namefile.find(".jpg") != -1 || namefile.find(".jpeg") != -1 || namefile.find(".png") != -1 || namefile.find(".bmp") != -1 || namefile.find(".gif") != -1 || namefile.find(".webp") != -1)
								arraychapter.push_back(foldertoread + namefile);
						}
						closedir(dir);
					}
					else {

					}




					Pagemanga.loadFromFile(arraychapter[selectpage]);
					arraypage.resize(arraychapter.size());

					if (cascadeactivated == true)
					{
						for (int x = 0; x < arraychapter.size(); x++)
						{
							arraypage[x].loadFromFile(arraychapter[x]);
						}
					}
					std::cout << arraypage.size() << std::endl;
					statenow = readmanga;
					helppage = true;

					break;
				}
			}


		}

		if (kDown & HidNpadButton_X)
		{
			switch (statenow)
			{
			case readmanga:
				if (cascadeactivated == true)
				{
					if (cascademode < 2)

					{
						cascademode++;
					}
					else { cascademode = 0; }


					if (cascademode == 1)
					{
						separation = false;
						cascade = true;
					}
					if (cascademode == 2)
					{
						cascade = true;
						separation = true;
					}
					if (cascademode == 0)
					{

						cascade = false;
						separation = false;
						basex = 0;
						basey = 0;
					}
					std::cout << cascademode << std::endl;
				}


				break;
			case selectmanga:
				cascadeactivated = !cascadeactivated;
				break;
			}
		}

		if (kDown & HidNpadButton_StickL)
		{
			switch (statenow)
			{
			case readmanga:
				helppage = !helppage;
				break;
			case selectmanga:
				helppage = !helppage;

				break;
			}
		}
		
		if (kDown & HidNpadButton_B)
		{
			switch (statenow)
			{
			case readmanga:
				statenow = selectmanga;
				Pagemanga.free();

				for (int x = 0; x < arraypage.size(); x++)
				{
					arraypage[x].free();
				}
				cascade = false;
				separation = false;
				adjust = true;
				basex = 0;
				basey = 0;
				neutralsize = 1;
				zoom = 1;
				selectpage = 0;
				break;
			case selectmanga:

				break;
			}
		}

#endif


		//Handle events on queue
		while (SDL_PollEvent(&e))
		{
			//User requests quit
			if (e.type == SDL_QUIT)
			{
				cancelcurl = 1;
				quit = 1;
			}
			//User presses a key
			else if (e.type == SDL_KEYDOWN)
			{
				//Select surfaces based on key press
				switch (e.key.keysym.sym)
				{
				case SDLK_DOWN:

					switch (statenow)
					{ case readmanga:
						
						if (cascade == false) {

							if ((basey) > (SCREEN_HEIGHT - heightnew))
							{
								basey -= speed;
								if (basey <= SCREEN_HEIGHT - heightnew)
								{
									basey = SCREEN_HEIGHT - heightnew;
								}
							}
						}
						else
						{
							basey -= speed;
						}
						break;

					case selectmanga:
						if (selectchapter < arraymain.size() - 1)
						{
							selectchapter++;

							std::cout << selectchapter << std::endl;
							baseymain = baseymain - 20;
						}
						else {
							selectchapter = 0;
							
							 baseymain = 30;
						}
						break;
					}
					
					break;

				case SDLK_UP:
					switch (statenow)
					{
					case readmanga:

						if (cascade == false) {
							if (basey <= 0)
							{
								basey += speed;
								if (basey > 0)
								{
									basey = 0;
								}
							}
						}
						else
						{
							if (basey <= 0)
							{
								basey += speed;
								if (basey > 0)
								{
									basey = 0;
								}
							}
						}
						break;
					case selectmanga:
						if (selectchapter > 0)
						{
							selectchapter--;
							std::cout << selectchapter << std::endl;
							baseymain =  baseymain + 20;
						}
						else {
							baseymain =  30 -20 * (arraymain.size() - 1) ;
							selectchapter = arraymain.size() - 1;
						}
						break;
					}
					break;
				case SDLK_a:
					if (existfoldermain == true) {
						switch (statenow)
						{
						case readmanga:
							if (cascade == false)
							{
								basey = 0;
								adjust = !adjust;
							}
							else
							{

								adjust = !adjust;
							}
							break;
						case selectmanga:
							std::string foldertoread = foldermain + arraymain[selectchapter] + "/";
							arraychapter.clear();
							DIR *dir;
							struct dirent *ent;
							if ((dir = opendir(foldertoread.c_str())) != NULL) {

								while ((ent = readdir(dir)) != NULL) {
									if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0)
										continue;
									printf("%s\n", ent->d_name);

									std::string namefile(ent->d_name);
									if (namefile.find(".jpg") != -1 || namefile.find(".jpeg") != -1 || namefile.find(".png") != -1 || namefile.find(".bmp") != -1 || namefile.find(".gif") != -1 || namefile.find(".webp") != -1)
										arraychapter.push_back(foldertoread + namefile);
								}
								closedir(dir);
							}
							else {

							}




							Pagemanga.loadFromFile(arraychapter[selectpage]);
							arraypage.resize(arraychapter.size());

							if (cascadeactivated == true)
							{for (int x = 0; x < arraychapter.size(); x++)
							{
								arraypage[x].loadFromFile(arraychapter[x]);
							}}
							std::cout << arraypage.size() << std::endl;
							statenow = readmanga;
							helppage = true;
							break;
						}
					}


					break;

				
				case SDLK_MINUS:
					switch (statenow)
					{
					case readmanga:
						helppage = !helppage;
						break;
					case selectmanga:
						helppage = !helppage;
			
						break;
					}

					break;
				case SDLK_b:
					switch (statenow)
					{case readmanga:
						statenow = selectmanga;
						Pagemanga.free();

						for (int x = 0; x < arraypage.size(); x++)
						{
							arraypage[x].free();
						}
						cascade = false;
						separation = false;
						adjust = true;
						basex = 0;
						basey = 0;
						neutralsize = 1;
						zoom = 1;
						selectpage = 0;
						break;
					case selectmanga:

						break;
					}
					
					break;

				case SDLK_l:
				
					switch (statenow)
					{
					case readmanga:
					{

						if (cascade == false)
						{
							basey = 0;
							basex = 0;
							zoom = 1;
							if (selectpage > 0)
							{
								Pagemanga.free();
								selectpage--;
								Pagemanga.loadFromFile(arraychapter[selectpage]);

							}
							else
							{
								Pagemanga.free();
								selectpage = 0;
								Pagemanga.loadFromFile(arraychapter[selectpage]);
							}
						}
						else
						{
							zoom = 1;
						}
					}

					break;
					}
				break;

				case SDLK_r:
					switch (statenow)
					{
					case readmanga:

						if (cascade == false)
						{
							basey = 0;
							basex = 0;
							zoom = 1;
							if (selectpage < arraychapter.size() - 1)
							{
								Pagemanga.free();
								selectpage++;
								Pagemanga.loadFromFile(arraychapter[selectpage]);

							}
						}
						else
						{

							zoom = 1;
						}
						break;
					}


					break;

				case SDLK_z:
					

					break;
				case SDLK_x:
					switch (statenow)
					{
					case readmanga:
						if (cascadeactivated == true)
						{
							if (cascademode < 2)

							{
								cascademode++;
							}
							else { cascademode = 0; }


							if (cascademode == 1)
							{
								separation = false;
								cascade = true;
							}
							if (cascademode == 2)
							{
								cascade = true;
								separation = true;
							}
							if (cascademode == 0)
							{

								cascade = false;
								separation = false;
								basex = 0;
								basey = 0;
							}
							std::cout << cascademode << std::endl;
						}
						

						break;
					case selectmanga:
						cascadeactivated = !cascadeactivated;
						break;
					}
					break;
				case SDLK_w:
					switch (statenow)
					{
					case readmanga:
						if (cascade == false)
						{
							if (zoom == 1)
							{
								zoom = 2;
							}
							else {
								zoom = 1;
								basex = 0;
								basey = 0;
							}
						}
						else
						{
							if (zoom == 1)
							{
								zoom = 2;
							}
							else {
								zoom = 1;
								basex = 0;

							}
						}
						break;
					}
					break;

				case SDLK_LEFT:
					switch (statenow)
					{
					case readmanga:

					{
						if (zoom == 2)
						{
							basex += speed;
						}
					}
					break;
					}

				break;



				case SDLK_RIGHT:
					switch (statenow)
					{
					case readmanga:
						if (zoom == 2)
						{
							basex -= speed;
						}
						break;
					}

					break;


				case SDLK_m:
					switch (statenow)
					{
					case readmanga:
						
							neutralsize = neutralsize + 0.05;
						
						break;
					}
					break;

				case SDLK_n:
					switch (statenow)
					{
					case readmanga:
						if (neutralsize > 0.05) {
							neutralsize = neutralsize - 0.05;
						}
						break;
					}
					break;

				default:

					break;
				}
			}



		}









		//Clear screen
		SDL_SetRenderDrawColor(gRenderer, 233, 234, 237, 0xFF);
		SDL_RenderClear(gRenderer);

		//Render current frame
		//wallpaper
		
		
		
		int ycascade = 0;
		switch (statenow)
		{
		case readmanga:
			

			if (cascade == true)
			{
				for (int x = 0; x < arraypage.size(); x++)

				{

					if (adjust == true)
					{
						widthnew = SCREEN_WIDTH * zoom;
						heightnew = ((arraypage[x].getHeight() * SCREEN_WIDTH) / arraypage[x].getWidth()) * zoom;
						arraypage[x].customrender((SCREEN_WIDTH / 2 - widthnew / 2) + basex, ycascade + basey, widthnew, heightnew);
					}
					else
					{
						widthnew = arraypage[x].getWidth()*zoom*neutralsize;
						heightnew = arraypage[x].getHeight()*zoom*neutralsize;
						if (widthnew >= SCREEN_WIDTH)
						{

							widthnew = SCREEN_WIDTH*zoom;
							heightnew = ((arraypage[x].getHeight() * SCREEN_WIDTH) / arraypage[x].getWidth())*zoom;

						}

						arraypage[x].customrender((SCREEN_WIDTH / 2 - widthnew / 2) + basex, ycascade + basey, widthnew, heightnew);
					}
					if (separation == false)
					{
						ycascade = ycascade + heightnew;
					}
					else
					{
						ycascade = ycascade + heightnew + 10;
					}


				}








			}
			else

			{
				if (adjust == true)
				{
					widthnew = SCREEN_WIDTH * zoom;
					heightnew = ((Pagemanga.getHeight() * SCREEN_WIDTH) / Pagemanga.getWidth()) * zoom;
					Pagemanga.customrender((SCREEN_WIDTH / 2 - widthnew / 2) + basex, 0 + basey, widthnew, heightnew);
				}
				else
				{
					
					widthnew = Pagemanga.getWidth()*zoom * neutralsize;
					heightnew = Pagemanga.getHeight()*zoom * neutralsize;
					if (widthnew >= SCREEN_WIDTH)
					{
						
						widthnew = SCREEN_WIDTH*zoom;
						heightnew = ((Pagemanga.getHeight() * SCREEN_WIDTH) / Pagemanga.getWidth())*zoom;
						
					}
					Pagemanga.customrender((SCREEN_WIDTH / 2 - widthnew / 2) + basex, 0 + basey, widthnew, heightnew);

				}
			}


			if (zoom == 2) {
				lupa.render(SCREEN_WIDTH - lupa.getWidth() - 10, SCREEN_HEIGHT - lupa.getHeight() - 10);
			}

			if (helppage == true)
			{
				SDL_Rect fillRect2 = { SCREEN_WIDTH / 2 + 110, SCREEN_HEIGHT / 2 +130  , 502, 202 };
				SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 0);
				SDL_RenderFillRect(gRenderer, &fillRect2);
				SDL_Rect fillRect = { SCREEN_WIDTH / 2 +100, SCREEN_HEIGHT / 2 +120  , 500, 200 };
				SDL_SetRenderDrawColor(gRenderer, 255, 255, 255, 255);
				SDL_RenderFillRect(gRenderer, &fillRect);
				if (cascadeactivated == true)
				{
					gTextTexture.loadFromRenderedText(gFont, "Press \"X\" for Cascade Mode (Separation or not).", { 0, 0, 0 });
					gTextTexture.render(SCREEN_WIDTH / 2 - gTextTexture.getWidth() / 2 + 350, (SCREEN_HEIGHT / 2 - gTextTexture.getHeight() / 2) + 150);
				}
				else
				{
					gTextTexture.loadFromRenderedText(gFont, "Cascade is disabled, enable it in the Main Menu.", { 0, 0, 0 });
					gTextTexture.render(SCREEN_WIDTH / 2 - gTextTexture.getWidth() / 2 + 350, (SCREEN_HEIGHT / 2 - gTextTexture.getHeight() / 2) + 150);
				}
				gTextTexture.loadFromRenderedText(gFont, "Press \"A\" for Fit Mode (On/Off).", { 0, 0, 0 });
				gTextTexture.render(SCREEN_WIDTH / 2 - gTextTexture.getWidth() / 2 + 350, 30 + (SCREEN_HEIGHT / 2 - gTextTexture.getHeight() / 2) +150);
				gTextTexture.loadFromRenderedText(gFont, "Press \"L\" for Previous page and \"R\" for Next page.", { 0, 0, 0 });
				gTextTexture.render(SCREEN_WIDTH / 2 - gTextTexture.getWidth() / 2 + 350, 60 + (SCREEN_HEIGHT / 2 - gTextTexture.getHeight() / 2) +150);
				gTextTexture.loadFromRenderedText(gFont, "Press \"ZL, ZR and R3\" for Zoom Mode.", { 0, 0, 0 });
				gTextTexture.render(SCREEN_WIDTH / 2 - gTextTexture.getWidth() / 2 + 350, 90 + (SCREEN_HEIGHT / 2 - gTextTexture.getHeight() / 2) +150);
				gTextTexture.loadFromRenderedText(gFont, "Press \"L3\" for show/hide this.", { 0, 0, 0 });
				gTextTexture.render(SCREEN_WIDTH / 2 - gTextTexture.getWidth() / 2 + 350, 120 + (SCREEN_HEIGHT / 2 - gTextTexture.getHeight() / 2) +150);
			}
		
		break;


		case selectmanga:
			Farest.render((0), (0));
			
			
			
			if (existfoldermain == false)
			{
				gTextTexture.loadFromRenderedText(gFont3, "Please create the folder called \"CuteManga\" in", { 0, 0, 0 });
				gTextTexture.render(SCREEN_WIDTH / 2 - gTextTexture.getWidth() / 2, 100);
				gTextTexture.loadFromRenderedText(gFont3, "your SD and put your \"Comic / Manga / Webtoon\" inside", { 0, 0, 0 });
				gTextTexture.render(SCREEN_WIDTH / 2 - gTextTexture.getWidth() / 2, 130);
				
					gTextTexture.loadFromRenderedText(gFont3, "in a folder without Latin characters.", { 0, 0, 0 });
				gTextTexture.render(SCREEN_WIDTH / 2 - gTextTexture.getWidth() / 2, 160);
				
			}
		

			for (int x = 0; x < arraymain.size(); x++) {

				
				if (x == selectchapter) {
					Heart.render(basexmain + 12,  baseymain + (x * 22));
				

				}
				



				

				if (x == selectchapter) {
					gTextTexture.loadFromRenderedText(gFont, arraymain[x], { 120, 120, 120 });
					gTextTexture.render(basexmain + 30, baseymain +(x * 22));

				}
				else
				{

					gTextTexture.loadFromRenderedText(gFont, arraymain[x], { 50, 50, 50 });
					gTextTexture.render(basexmain, baseymain + (x * 22));

				}

			}
			{SDL_Rect fillRect = { 0, SCREEN_HEIGHT - 35, 1280, 25 };
			SDL_SetRenderDrawColor(gRenderer, 255, 255, 255, 255);

			SDL_RenderFillRect(gRenderer, &fillRect); }

			textColor = { 50, 50, 50 };
			if (cascadeactivated == false)
			{
				gTextTexture.loadFromRenderedText(gFont, "\"A\" to read folder - \"X\" for Enable Cascade Mode (Load slow and High Memory use). ", textColor);
				gTextTexture.render(basexmain, SCREEN_HEIGHT - 30);
			}
			else
			{
				gTextTexture.loadFromRenderedText(gFont, "\"A\" to read folder - \"X\" for Disable Cascade Mode (Instant load and Low Memory use). ", textColor);
				gTextTexture.render(basexmain, SCREEN_HEIGHT - 30);
			}


		

			break;
		


		}



		//Update screen
		SDL_RenderPresent(gRenderer);


	}
	
	

	//Free resources and close SDL
#ifdef __SWITCH__
	socketExit();
	romfsExit();
#endif // SWITCH


	
	close();

	return 0;

}
