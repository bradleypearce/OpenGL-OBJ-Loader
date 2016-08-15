#include "Display.h"

/// Constructor 
///
/// For our display's constructor, we need to take in the window title, the width and the height of the window were creating. 
/// We use this to initialise everything in SDL (with some error checking), set up our colour channels so we have 256 types of 
/// red, green, blue and alpha colours. This is the most efficient model of the colour spectrum and provides a good range of colours.
/// We also set up our buffers, for the best result we use a double buffer method and we also use a depth buffer to account for 3D space
/// and then after we've set these up we create our window along with our context. We initialise the GLEW library after which so we can 
/// utilise face culling, a simple depth test and other features of GLEW.
Display::Display(int _width, int _height, const std::string& _title)
{
	if(SDL_Init(SDL_INIT_EVERYTHING) != 0)
	{
		std::cerr << "SDL Error" << std::endl;
	}

	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);

	SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE,16);

	m_window = SDL_CreateWindow(_title.c_str(),SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED, _width, _height, SDL_WINDOW_OPENGL);
	m_glcontext = SDL_GL_CreateContext(m_window);

	GLenum status = glewInit();

	if(status != GLEW_OK)
	{
		std::cerr << "Glew failed to initialise" << std::endl;
	}

	m_isclosed = false;

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_NORMALIZE);
	glEnable(GL_SMOOTH);
	glEnable(GL_CULL_FACE);
	glEnable(GL_LIGHTING);
	glCullFace(GL_BACK);
}

/// Deconstructor 
///
/// Simple calls the delete functions for our GLcontext and our window. 
/// Then it also terminates SDL. 
Display::~Display()
{
	SDL_GL_DeleteContext(m_glcontext);
	SDL_DestroyWindow(m_window);
	SDL_Quit();
}

/// Clear 
///
/// This function simple takes in 4 different values, each representing a colour in the colour channel i.e red, blue ect
/// then it uses the GLclearcolor function to clear the current frame of any other colours, other than those that we passed in. 
/// This is usually 0 for each color and 1 for alpha. We also clear the buffers active in our window. 
void Display::Clear(float _red, float _green, float _blue, float _alpha)
{
	glClearColor(_red,_green,_blue,_alpha);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

/// Update 
///
/// This function simlpy swaps our buffers every frame so whatever was written to the 'double buffer' is written onto screen. 
/// We also use an SDL_Event to check whether the window has been closed and if so we set m_isclosed to true. This helps with closing
/// the application if the user closes the window whilst the program is running. 
void Display::Update()
{
	SDL_GL_SwapWindow(m_window);

	SDL_Event e;

	while(SDL_PollEvent(&e))
	{
		if(e.type == SDL_QUIT)
		{
			m_isclosed = true;
		}
	}
}


/// IsClosed
///
/// Simply returns the value of m_isclosed. 
bool Display::IsClosed()
{
	return m_isclosed;
}