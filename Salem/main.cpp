#include "Application.h"
int main() {
	//Application has to be called first
	Application app = Application("Salem", 1280, 720);

	AppDisk *salem = new AppDisk();
	salem->addObject("Assets/Cube/cube.obj");
	
	app.SetDisk(salem);
	app.Run();
	return 0;
}