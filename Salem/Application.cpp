#include "Application.h"
#include <string>
#include <SDL.h>
#include <iostream>
#include <GL\glew.h>
#include <memory>
#include <IL\il.h>

using namespace std;

struct Application::impl {
	int prevTick;

	bool running = true;

	unique_ptr<AppDisk> program;
	//Window //
	uint width;
	uint height;

	const char* name;

	SDL_Window *window;
	SDL_GLContext glContext;

	impl() {};
	impl(const char * name, uint width, uint height);
	void SetupRC();
	void Destroy();
	//End of Window //
	// Glew Setup//

	void IntializeOpenGL();

	void Start();
	void Input();
	void Update(float dt);
	void Render();

};

Application::Application()
{
}

Application::Application(const char* name, uint width, uint height) : pImpl( new impl(name, width, height))
{
	pImpl->SetupRC();
	pImpl->IntializeOpenGL();
}

void Application::SetDisk(AppDisk *program)
{
	pImpl->program = unique_ptr<AppDisk>(program);
}

void Application::Run()
{
	pImpl->Start();

	while (pImpl->running) {
		
		int current = SDL_GetTicks();

		float dt = float(current - pImpl->prevTick) / 1000.0f;

		pImpl->prevTick = current;

		pImpl->Input();
		pImpl->Update(dt);
		pImpl->Render();
	}

	pImpl->Destroy();
	
}

Application::impl::impl(const char * name, uint width, uint height): name(name), width(width), height(height)
{
}

Application::~Application() {
	delete pImpl;
}

void Application::impl::SetupRC()
{
	if (SDL_Init(SDL_INIT_VIDEO) < 0) { // Initialize video
		std::cout << "Unable to initailize SDL" << " ";
		exit(1);
	}
	// Request an OpenGL 3.0 context.
	// Not able to use SDL to choose profile (yet), should default to core profile on 3.2 or later
	// If you request a context not supported by your drivers, no OpenGL context will be created

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);  // double buffering on
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4); // Turn on x4 multisampling anti-aliasing (MSAA)

	// Create window
	window = SDL_CreateWindow(name, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		width, height, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
	
	// Disable Cursor from showing on window i.e creates a blank cursor.
	SDL_ShowCursor(SDL_DISABLE);

	if (!window) {
		std::cout << "Unable to create window" << " ";
		exit(1);
	}

	glContext = SDL_GL_CreateContext(window); // Create opengl context and attach to window
	SDL_GL_SetSwapInterval(1); // set swap buffers to sync with monitor's vertical refresh rate
}

void Application::impl::Destroy()
{
	SDL_GL_DeleteContext(glContext);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

void Application::impl::IntializeOpenGL()
{

	// Initialize GLEW to setup the OpenGL Function pointers
	glewInit();
	ilInit();


	GLenum err = glewInit();

	// Set this to true so GLEW knows to use a modern approach to retreiving function pointers and extensions
	glewExperimental = GL_TRUE;

	if (GLEW_OK != err) { // glewInit failed, something is seriously wrong
		std::cout << "glewInit failed, aborting." << endl;
		exit(1);
	}
	cout << glGetString(GL_VERSION) << endl;
	// Setup OpenGL options

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);
	glEnable(GL_DEPTH_TEST); // Enable Z Buffer
	//glEnable(GL_BLEND); // Enable Blending
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void Application::impl::Start()
{
	program->Start();
}

void Application::impl::Input()
{
	SDL_Event sdlEvent;

	while (SDL_PollEvent(&sdlEvent)) {
		if (sdlEvent.type == SDL_QUIT) {
			running = false;
		}
		switch (sdlEvent.type) {
		case SDL_KEYDOWN:
			switch (sdlEvent.key.keysym.sym) {
			case SDLK_ESCAPE:
				running = false;
				break;
			}
		}
		program->Input(&sdlEvent);
		SDL_WarpMouseInWindow(window, 640, 360);
	}
}

void Application::impl::Update(float dt)
{

	program->Update(dt);
}

void Application::impl::Render()
{
	glClearColor(0.05f, 0.05f, 0.05f, 1.0f); // Color to clear with (Background color of active scene)
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear the screen

	program->Render();
	SDL_GL_SwapWindow(window);
}
