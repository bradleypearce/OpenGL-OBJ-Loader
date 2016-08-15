#pragma once

/// CLASS DISPLAY
///
/// The Dislpay class is responsible for initialising SDL, creating an opengl context and of course 
/// creating a display our graphics card can pipe our graphics to. To function properly, we need a 
/// clear function, an update function and a "isclosed" function. 

#include "Headers.h"

class Display
{
public:
	Display(int _width, int _height, const std::string& _title);
	~Display();

	void Clear(float _red, float _green, float _blue, float _alpha);
	void Update();
	bool IsClosed();
private: 
	SDL_Window* m_window;
	SDL_GLContext m_glcontext;
	GLuint m_depth_texture;

	enum
	{
		DEPTH_VB,

		NUM_BUFFERS
	};

	GLuint m_VBO[NUM_BUFFERS];

	bool m_isclosed;
};

