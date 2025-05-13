#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include <cstdio>
#include <string>
#include <sstream>
#include<time.h>
#include<iomanip>
#include<iostream>
#include <SDL2/SDL_mixer.h>

TTF_Font* eFont = NULL;
SDL_Window *eWindow = NULL;
SDL_Renderer *eRenderer = NULL;
SDL_Event eevent;
using namespace std;

//screen size
const int SCREENWIDTH = 480;
const int SCREENHEIGHT = 600;


//SDL parameters
SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
SDL_Event event;
Mix_Music *music;
Mix_Music *effect;
TTF_Font* gFont = NULL;
//global parameters
int score;
TTF_Font *font = TTF_OpenFont("./font/arial.ttf", 25);
SDL_Color color = {255, 255, 255};

//used to load pictures
class LTexture
{
	public:
    	//Initializes variables
    	LTexture();
    	//Deallocates memory
    	~LTexture();
    	//Loads image at specified path
    	bool loadFromFile( std::string path );
    	bool loadFromRenderedText( std::string textureText, SDL_Color textColor );
    	//Deallocates texture
    	void free();
    	//Renders texture at given point
	    void render( int x, int y );
	    void render2( int x, int y );
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

//time
class LTimer
{
    public:
		//Initializes variables
		LTimer();

		//The various clock actions
		void start();
		void stop();
		void pause();
		void unpause();

		//Gets the timer's time
		Uint32 getTicks();

		//Checks the status of the timer
		bool isStarted();
		bool isPaused();

    private:
		//The clock time when the timer started
		Uint32 mStartTicks;

		//The ticks stored when the timer was paused
		Uint32 mPausedTicks;

		//The timer status
		bool mPaused;
		bool mStarted;
};
LTimer::LTimer()
{
    //Initialize the variables
    mStartTicks = 0;
    mPausedTicks = 0;

    mPaused = false;
    mStarted = false;
}

void LTimer::start()
{
    //Start the timer
    mStarted = true;

    //Unpause the timer
    mPaused = false;

    //Get the current clock time
    mStartTicks = SDL_GetTicks();
	mPausedTicks = 0;
}

void LTimer::stop()
{
    //Stop the timer
    mStarted = false;

    //Unpause the timer
    mPaused = false;

	//Clear tick variables
	mStartTicks = 0;
	mPausedTicks = 0;
}



Uint32 LTimer::getTicks()
{
	//The actual timer time
	Uint32 time = 0;

    //If the timer is running
    if( mStarted )
    {
        //If the timer is paused
        if( mPaused )
        {
            //Return the number of ticks when the timer was paused
            time = mPausedTicks;
        }
        else
        {
            //Return the current time minus the start time
            time = SDL_GetTicks() - mStartTicks;
        }
    }

    return time;
}

LTexture cover;
LTexture background;
LTexture platforms[4];
LTexture player; 
LTexture alphabet;


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
bool LTexture::loadFromFile( std::string path )
{
    //Get rid of preexisting texture
    free();
    //The final texture
    SDL_Texture* newTexture = NULL;
    //Load image at specified path
    SDL_Surface* loadedSurface = IMG_Load( path.c_str() );
    if( loadedSurface == NULL )
    {
        printf( "Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError() );
    }
    else
    {
        //Color key image
        //SDL_SetColorKey( loadedSurface, SDL_TRUE, SDL_MapRGB( loadedSurface->format, 0, 0xFF, 0xFF ) );
        //Create texture from surface pixels
        newTexture = SDL_CreateTextureFromSurface( renderer, loadedSurface );
        if( newTexture == NULL )
        {
            printf( "Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError() );
        }
        else
        {
            //Get image dimensions
            mWidth = loadedSurface->w;
            mHeight = loadedSurface->h;
        }
        //Get rid of old loaded surface
        SDL_FreeSurface( loadedSurface );
    }
    //Return success
    mTexture = newTexture;
    return mTexture != NULL;
}
#if defined(_SDL_TTF_H) || defined(SDL_TTF_H_)
bool LTexture::loadFromRenderedText( std::string textureText, SDL_Color textColor )
{
	//Get rid of preexisting texture
	free();
 
	//Render text surface
	SDL_Surface* textSurface2 = TTF_RenderText_Solid( eFont, textureText.c_str(), textColor );
	if( textSurface2 != NULL )
	{
		//Create texture from surface pixels
        mTexture = SDL_CreateTextureFromSurface( eRenderer, textSurface2 );
		if( mTexture == NULL )
		{
			printf( "Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError() );
		}
		else
		{
			//Get image dimensions
			mWidth = textSurface2->w;
			mHeight = textSurface2->h;
		}

		//Get rid of old surface
		SDL_FreeSurface( textSurface2 );
	}
	else
	{
		printf( "Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError() );
	}


	//Return success
	return mTexture != NULL;
}
#endif
void LTexture::free()
{
    //Free texture if it exists
    if( mTexture != NULL )
    {
        SDL_DestroyTexture( mTexture );
        mTexture = NULL;
        mWidth = 0;
        mHeight = 0;
    }
}
void LTexture::render( int x, int y )
{
    //Set rendering space and render to screen
    SDL_Rect renderQuad = { x, y, mWidth, mHeight };
    SDL_RenderCopy( renderer, mTexture, NULL, &renderQuad );
}

void LTexture::render2( int x, int y)
{
	//Set rendering space and render to screen
	SDL_Rect renderQuad = { x, y, 400, 400 };
    SDL_RenderCopy( eRenderer, mTexture, NULL, &renderQuad );
}

int LTexture::getWidth()
{
    return mWidth;
}
int LTexture::getHeight()
{
    return mHeight;
}

class Life
{
	friend bool check(SDL_Event &event);
	private:
		int lives = 3;
		
	public:
		//Life();
		LTexture* life;
		Life operator--(int k);
		void PrintLife();
		int getLives();
};
Life PlayerLife;
/*Life::life()
{
	
}*/
Life Life::operator--(int k)
{
	lives -= 1;
}
void Life::PrintLife()
{
	life = new LTexture[lives];
	for(int i=0; i<lives; i++)
	{
		life[i].loadFromFile("./img/heart.png");
	}
}
int Life::getLives()
{
	return lives;
}

enum alphabets
{
	A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z
};
class Alphabets
{
	private:
		int min = 0;
		int max = 25;
		int choose;
		
	public:
		bool loadAlphabet();
		bool check(SDL_Event &event);
};
Alphabets letter;
bool Alphabets::loadAlphabet()
{
	bool success = true;
	
	srand((int)time(NULL));
	choose = rand() % (max - min + 1) + min;
	
	switch(choose)
	{
		case A: alphabet.loadFromFile("./img/a.png"); success = false; break;
		case B: alphabet.loadFromFile("./img/b.png"); success = false; break;
		case C: alphabet.loadFromFile("./img/c.png"); success = false; break;
		case D: alphabet.loadFromFile("./img/d.png"); success = false; break;
		case E: alphabet.loadFromFile("./img/e.png"); success = false; break;
		case F: alphabet.loadFromFile("./img/f.png"); success = false; break;
		case G: alphabet.loadFromFile("./img/g.png"); success = false; break;
		case H: alphabet.loadFromFile("./img/h.png"); success = false; break;
		case I: alphabet.loadFromFile("./img/i.png"); success = false; break;
		case J: alphabet.loadFromFile("./img/j.png"); success = false; break;
		case K: alphabet.loadFromFile("./img/k.png"); success = false; break;
		case L: alphabet.loadFromFile("./img/l.png"); success = false; break;
		case M: alphabet.loadFromFile("./img/m.png"); success = false; break;
		case N: alphabet.loadFromFile("./img/n.png"); success = false; break;
		case O: alphabet.loadFromFile("./img/o.png"); success = false; break;
		case P: alphabet.loadFromFile("./img/p.png"); success = false; break;
		case Q: alphabet.loadFromFile("./img/q.png"); success = false; break;
		case R: alphabet.loadFromFile("./img/r.png"); success = false; break;
		case S: alphabet.loadFromFile("./img/s.png"); success = false; break;
		case T: alphabet.loadFromFile("./img/t.png"); success = false; break;
		case U: alphabet.loadFromFile("./img/u.png"); success = false; break;
		case V: alphabet.loadFromFile("./img/v.png"); success = false; break;
		case W: alphabet.loadFromFile("./img/w.png"); success = false; break;
		case X: alphabet.loadFromFile("./img/x.png"); success = false; break;
		case Y: alphabet.loadFromFile("./img/y.png"); success = false; break;
		case Z: alphabet.loadFromFile("./img/z.png"); success = false; break;
	}
	return success;
}
bool Alphabets::check(SDL_Event &event)
{
	switch(choose)
	{
		case A: 
		{
			if(event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_a && event.key.repeat == 0)
			{
				score++;
				return true;
			}
			else{ return false; }
		};
		break;
		case B: 
		{
			if(event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_b && event.key.repeat == 0)
			{
				score++;
				return true;
			}
			else{ return false; }
		};
		break;
		case C: 
		{
			if(event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_c && event.key.repeat == 0)
			{
				score++;
				return true;
			}
			else{ return false; }
		};
		break;
		case D: 
		{
			if(event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_d && event.key.repeat == 0)
			{
				score++;
				return true;
			}
			else{ return false; }
		};
		break;
		case E: 
		{
			if(event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_e && event.key.repeat == 0)
			{
				score++;
				return true;
			}
			else{ return false; }
		};
		break;
		case F: 
		{
			if(event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_f && event.key.repeat == 0)
			{
				score++;
				return true;
			}
			else{ return false; }
		};
		break;
		case G: 
		{
			if(event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_g && event.key.repeat == 0)
			{
				score++;
				return true;
			}
			else{ return false; }
		};
		break;
		case H: 
		{
			if(event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_h && event.key.repeat == 0)
			{
				score++;
				return true;
			}
			else{ return false; }
		};
		break;
		case I: 
		{
			if(event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_i && event.key.repeat == 0)
			{
				score++;
				return true;
			}
			else{ return false; }
		};
		break;
		case J: 
		{
			if(event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_j && event.key.repeat == 0)
			{
				score++;
				return true;
			}
			else{ return false; }
		};
		break;
		case K: 
		{
			if(event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_k && event.key.repeat == 0)
			{
				score++;
				return true;
			}
			else{ return false; }
		};
		break;
		case L: 
		{
			if(event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_l && event.key.repeat == 0)
			{
				score++;
				return true;
			}
			else{ return false; }
		};
		break;
		case M: 
		{
			if(event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_m && event.key.repeat == 0)
			{
				score++;
				return true;
			}
			else{ return false; }
		};
		break;
		case N: 
		{
			if(event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_n && event.key.repeat == 0)
			{
				score++;
				return true;
			}
			else{ return false; }
		};
		break;
		case O: 
		{
			if(event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_o && event.key.repeat == 0)
			{
				score++;
				return true;
			}
			else{ return false; }
		};
		break;
		case P: 
		{
			if(event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_p && event.key.repeat == 0)
			{
				score++;
				return true;
			}
			else{ return false; }
		};
		break;
		case Q: 
		{
			if(event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_q && event.key.repeat == 0)
			{
				score++;
				return true;
			}
			else{ return false; }
		};
		break;
		case R: 
		{
			if(event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_r && event.key.repeat == 0)
			{
				score++;
				return true;
			}
			else{ return false; }
		};
		break;
		case S: 
		{
			if(event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_s && event.key.repeat == 0)
			{
				score++;
				return true;
			}
			else{ return false; }
		};
		break;
		case T: 
		{
			if(event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_t && event.key.repeat == 0)
			{
				score++;
				return true;
			}
			else{ return false; }
		};
		break;
		case U: 
		{
			if(event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_u && event.key.repeat == 0)
			{
				score++;
				return true;
			}
			else{ return false; }
		};
		break;
		case V: 
		{
			if(event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_v && event.key.repeat == 0)
			{
				score++;
				return true;
			}
			else{ return false; }
		};
		break;
		case W: 
		{
			if(event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_w && event.key.repeat == 0)
			{
				score++;
				return true;
			}
			else{ return false; }
		};
		break;
		case X: 
		{
			if(event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_x && event.key.repeat == 0)
			{
				score++;
				return true;
			}
			else{ return false; }
		};
		break;
		case Y: 
		{
			if(event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_y && event.key.repeat == 0)
			{
				score++;
				return true;
			}
			else{ return false; }
		};
		break;
		case Z: 
		{
			if(event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_z && event.key.repeat == 0)
			{
				score++;
				return true;
			}
			else{ return false; }
		};
		break;
	}
}

class PrintWords
{
	public:
		PrintWords();
		~PrintWords();
		bool Print(char* a);
		void free();
		void render(int x, int y);
		
	private:
		SDL_Texture* wTexture;
		int texW = 0;
		int texH = 0;
};
PrintWords::PrintWords()
{
	wTexture = NULL;
	texW = 0;
	texH = 0;
}
PrintWords::~PrintWords()
{
	free();
}
bool PrintWords::Print(char* a)
{
	free();
	SDL_Texture* newTexture = NULL;
	SDL_Surface* wordSurface = TTF_RenderText_Solid(font, a, color);
	
	//SDL_SetColorKey(wordSurface, SDL_TRUE, SDL_MapRGB(wordSurface->format, 0, 0xFF, 0xFF));
	
	newTexture = SDL_CreateTextureFromSurface(renderer, wordSurface);
	
	SDL_FreeSurface(wordSurface);
	wTexture = newTexture;
	return wTexture!=NULL;
}
void PrintWords::free()
{
	if(wTexture!=NULL)
	{
		SDL_DestroyTexture(wTexture);
		wTexture = NULL;
		texW = 0;
		texH = 0;
	}
}
void PrintWords::render(int x, int y)
{
	SDL_QueryTexture(wTexture, NULL, NULL, &texW, &texH);
	SDL_Rect dstrect = {x, y, texW, texH};
	SDL_RenderCopy(renderer, wTexture, NULL, &dstrect);
}

class Level
{
	public:
		SDL_Rect level[40];
		void DrawLevel();
};
Level levelRect;
void Level::DrawLevel()
{
	int Y = 580;
	for(int i=0; i<40; i++)
	{
		level[i].x = 20;
		level[i].y = Y-i*14;
		level[i].w = 20;
		level[i].h = 14;
		SDL_SetRenderDrawColor(renderer, 0xDD, 0x33, 0x00, 0xFF);
		SDL_RenderDrawRect(renderer, &level[i]);
	}
}

//Game functions
int InitGame()
{
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
	{
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't initialize SDL: %s", SDL_GetError());
        return 3;
    }
	TTF_Init();
	int mflags = MIX_INIT_MP3;
    if( (Mix_Init(mflags) & mflags) != mflags ) 
	{
        std::cout << "Mix_Init: Failed to init required ogg and mod support!\n";
        std::cout << "Mix_Init: %s\n" << Mix_GetError() << std::endl;
        return 3;
    }
    if(Mix_OpenAudio(128000, MIX_DEFAULT_FORMAT, 2, 2048)==-1) 
	{
        std::cout << "Mix_OpenAudio: %s\n" << Mix_GetError() << std::endl;
        return 3;
    }
    // load the MP3 file "music.mp3" to play as music
    //Mix_Music *music;
    music = Mix_LoadMUS("mp3/music.mp3");
    if(!music) 
	{
        std::cout << "Mix_LoadMUS(\"music.mp3\"): %s\n" << Mix_GetError();
        return 3;
    }
    window = SDL_CreateWindow("Type to Space!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREENWIDTH, 
								SCREENHEIGHT, 0);
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	
	score = 0;
	
	cover.loadFromFile("./img/spacecover.png");
	
}
int RunGame()
{
	background.loadFromFile("./img/skyandground.jpg");
	for(int i=0; i<4; i++)
	{
		platforms[i].loadFromFile("./img/platform.png");
	}
	player.loadFromFile("./img/smalljet.png");
	
	letter.loadAlphabet();
	
	levelRect.DrawLevel();
	
	PlayerLife.PrintLife();
}

//main funtion
int main(int argc, char *argv[])
{
	InitGame();	
	bool quit = false;
	//SDL_Event event;
	//RunGame();
	int playerh = 490;
	int stair = 0;
	int count = 0;
	double clock=0;
	//int mouseX = event.motion.x;
	//int mouseY = event.motion.y;
	LTimer timer;
	std::stringstream timeText;
	SDL_Color textColor = { 255, 255, 255, 255 };
	Mix_PlayMusic(music, -1);
	while(!quit)
	{
		
		
		Uint32 a=0;
		SDL_PollEvent(&event);
        
        switch(event.type)
        {
        	
        	case SDL_QUIT:
                quit = true;
                break;
            
            case SDL_MOUSEBUTTONDOWN:
            	switch (event.button.button)
			    {
    			    case SDL_BUTTON_LEFT:
    				{
    					timer.start();
    				    //SDL_ShowSimpleMessageBox(0, "Mouse", "Left button was pressed!", window);	
    					RunGame();	
            			break;	
					}
        			/*case SDL_BUTTON_RIGHT:
            			SDL_ShowSimpleMessageBox(0, "Mouse", "Right button was pressed!", window);
           				break;
        			default:
            			SDL_ShowSimpleMessageBox(0, "Mouse", "Some other button was pressed!", window);
            			break;*/
				}
    			break;
    			
    		/*case SDL_MOUSEMOTION:
    			{
    				int mouseX = event.motion.x;
   					int mouseY = event.motion.y;
 
    				std::stringstream ss;
    				ss << "X: " << mouseX << " Y: " << mouseY;
 
    				SDL_SetWindowTitle(window, ss.str().c_str());
    				break;	
				}*/
		}
		//clear window
        SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
        SDL_RenderClear(renderer);
        
        //Game Init
        cover.render(0, 0);
		
		//Game Run
        background.render(0, 0);
        
        int h = 440;
        for(int i=0; i<4; i++)
        {
        	if(i%2==0)
        	{
        		platforms[i].render(330, h);
        	}
			else
			{
				platforms[i].render(130, h);
			}
			h -= 110;
		}
		
		player.render(220, playerh);
		
		
		//Placing alphabets and checking
		int alphabetX, alphabetY;
		if(count % 2 == 0)
		{
			alphabetX = 330;
		}
		else if(count % 2 == 1)
		{
			alphabetX = 130;
		}
		if(stair == 0 || stair == 4)
		{
			alphabetY = 360;
		}
		else if(stair == 1)
		{
			alphabetY = 250;
		}
		else if(stair == 2)
		{
			alphabetY = 140;
		}
		else if(stair == 3)
		{
			alphabetY = 30;
		}
		alphabet.render(alphabetX, alphabetY);
		
		if(letter.check(event))
		{
			SDL_Delay(20);
			playerh -= 110;
			stair++;
			count++;
			cout<<"stair = "<<stair<<endl;
			cout<<"playerh = "<<playerh<<endl;
			cout<<"count = "<<count<<endl;
			cout<<"score = "<<score<<endl;
			alphabet.free();
			letter.loadAlphabet();
			
			if(stair==4)
    		{
    			stair = 0;
    			playerh = 490;
			}
			if(count>=4 && count<=15)
			{
				background.loadFromFile("./img/sky.jpg");
			}
			if(count>=16 && count<=27)
			{
				background.loadFromFile("./img/starynightsky.png");
			}
			if(count>=28 && count<=39)
			{
				background.loadFromFile("./img/spacebackground.jpg");
			}
			if(count>=40)
			{
				
				
				background.loadFromFile("./img/moon.jpg");
				for(int i=0; i<4; i++)
    			{
			    	platforms[i].free();
				}
				alphabet.free();
				playerh = 160;
				background.render(0, 0);
				SDL_RenderPresent(renderer);
			}
		}
		/*int heartX = 0;
		for(int i=0; i<PlayerLife.getLives(); i++)
		{
			PlayerLife.life[i].render(heartX, 0);
			heartX += 20;
		}*/
		if(count>=40)
		{
			
			clock=timer.getTicks()/1000;
			
			LTexture gTimeTextTexture;

		    if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
			{
				printf( "SDL could not initialize! SDL Error: %s\n", SDL_GetError() );
				
			}
			else
			{
				//Set texture filtering to linear
				if( !SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "1" ) )
				{
					printf( "Warning: Linear texture filtering not enabled!" );
				}
				
		//Create window
				eWindow = SDL_CreateWindow( "SDL Tutorial", 1050, 600, 400,400, SDL_WINDOW_SHOWN );
				if( eWindow == NULL )
					{
						printf( "Window could not be created! SDL Error: %s\n", SDL_GetError() );
					
					}
				else
				{
					//Create vsynced renderer for window
					eRenderer = SDL_CreateRenderer( eWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC );
					if( eRenderer == NULL )
					{
						printf( "Renderer could not be created! SDL Error: %s\n", SDL_GetError() );
					
					}
					else
					{
						//Initialize renderer color
						SDL_SetRenderDrawColor( eRenderer, 0x00, 0x00, 0x00, 0x00 );

						if( TTF_Init() == -1 )
						{
							printf( "SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError() );
						
						}
					}
				}

		
				
				//Open the font
				eFont = TTF_OpenFont( "fonts/GenJyuuGothic-Regular.ttf", 600 );
				
				if( eFont == NULL )
				{
					printf( "Failed to load font! SDL_ttf Error: %s\n", TTF_GetError() );
			
				}
				else
				{
					timeText.str( "" );
					timeText <<clock <<"s";
					//eFont = TTF_OpenFont( "fonts/GenJyuuGothic-Regular.ttf", 20 );
					
					
					
					if( !gTimeTextTexture.loadFromRenderedText( timeText.str().c_str(), textColor ) )
					{
						printf( "Unable to render time texture!\n" );
					}
					
				}
				bool end=false;
				while (!end) 
				{
					
			        while( SDL_PollEvent( &event ) != 0 )
				    {
	
				        if( eevent.type == SDL_QUIT )
				        {
				            quit = true;
				        }
				    }
				    
					
			        SDL_SetRenderDrawColor(eRenderer, 0x19, 0x19, 0x70, 0xFF);
			        SDL_RenderClear(eRenderer);
			        gTimeTextTexture.render2(0,0);
			        SDL_RenderPresent(eRenderer);
			        
			        //SDL_RenderCopy(eRenderer, etexture, NULL, NULL);
			        if(event.type==SDL_MOUSEBUTTONDOWN && event.button.button==SDL_BUTTON_LEFT)
				
					{	
						
			    		eFont = NULL;
					
			    		SDL_DestroyRenderer(eRenderer);
			    		SDL_DestroyWindow(eWindow);
				    	count=0;
				    	score=0;
				    	clock=0;
				    	end=true;
				    	timer.stop();
				    	
				    }
				}
			
			}
		}
		
		SDL_RenderPresent(renderer);

	}
	
	
    background.free();
    for(int i=0; i<4; i++)
    {
    	platforms[i].free();
	}
	player.free();
	alphabet.free();
	cover.free();
	
	TTF_CloseFont(font);
	SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

	TTF_Quit();
    SDL_Quit();
    return 0;
	
}
