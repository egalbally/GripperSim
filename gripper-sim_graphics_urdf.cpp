// This example application loads a URDF world file and simulates a finger robot
// with physics and contact in a Dynamics3D virtual world. 
// A graphics model of it is also shown using Chai3D.

#include "Model/ModelInterface.h"  // SEE ME this is all in SAI source, is there a way to include the files from another repo?
#include "Simulation/SimulationInterface.h"
#include "Graphics/GraphicsInterface.h"

#include <GLFW/glfw3.h> //must be loaded after loading opengl/glew

#include <iostream>
#include <string>

using namespace std;

const string world_file = "resources/world.urdf";
const string robot_file = "resources/pbot.urdf";
const string robot_name = "finger";
const string camera_name = "camera_fixed";

// callback to print glfw errors
void glfwError(int error, const char* description);

// callback when a key is pressed
void keySelect(GLFWwindow* window, int key, int scancode, int action, int mods);


//----------------------------------------------------------------------------------------
int main() 
{
	cout << "Loading URDF world model file: " << world_file << endl;  /*COUT is used to print info on screen*/

	// load simulation world
	auto sim = new Simulation::SimulationInterface(world_file, Simulation::dynamics3d, Simulation::urdf, false);

	/*AUTO specifies that the type of the variable that is being declared will be automatically deduced from its initializer*/

	// load graphics scene
	auto graphics = new Graphics::GraphicsInterface(world_file, Graphics::chai, Graphics::urdf, true);

	// load robot
	auto robot = new Model::ModelInterface(robot_file, Model::rbdl, Model::urdf, false);

	// joint initialization
	sim->getJointPositions(robot_name, robot->_q); //SEE ME shortcut to get to the function definition?
	
	/*NOTE TO SELF
	foo->bar is equivalent to (*foo).bar, i.e. it gets the member called bar from the struct that foo points to*/

	/*------- Set up visualization -------*/
    // set up error callback
    glfwSetErrorCallback(glfwError);

    // initialize GLFW
    glfwInit();

    // retrieve resolution of computer display and position window accordingly
    GLFWmonitor* primary = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(primary);

    // information about computer screen and GLUT display window
	int screenW = mode->width;
    int screenH = mode->height;
    int windowW = 0.8 * screenH;
    int windowH = 0.5 * screenH;
    int windowPosY = (screenH - windowH) / 2;
    int windowPosX = windowPosY;

    // create window and make it current
    glfwWindowHint(GLFW_VISIBLE, 0);
    GLFWwindow* window = glfwCreateWindow(windowW, windowH, "gripper-sim_graphics_urdf", NULL, NULL);
	glfwSetWindowPos(window, windowPosX, windowPosY);
	glfwShowWindow(window);
    glfwMakeContextCurrent(window);
	glfwSwapInterval(1);

    // set callbacks
	glfwSetKeyCallback(window, keySelect);

    // while window is open:
    while (!glfwWindowShouldClose(window))
	{
		// update simulation by 10ms
		sim->integrate(0.01);

		// update kinematic models
		sim->getJointPositions(robot_name, robot->_q);
		sim->getJointVelocities(robot_name, robot->_dq);
		robot->updateModel();

		// update graphics. this automatically waits for the correct amount of time
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);
		graphics->updateGraphics(robot_name, robot);
		graphics->render(camera_name, width, height);

		// swap buffers
		glfwSwapBuffers(window);

		// wait until all GL commands are completed
		glFinish();

		// check for any OpenGL errors
		GLenum err;
		err = glGetError();
		assert(err == GL_NO_ERROR);

	    // poll for events
	    glfwPollEvents();
	}

    // destroy context
    glfwDestroyWindow(window);

    // terminate
    glfwTerminate();

	return 0;
}

//------------------------------------------------------------------------------

void glfwError(int error, const char* description) 
{
	cerr << "GLFW Error: " << description << endl;  /*CERR standard error stream*/
	exit(1);
}

//------------------------------------------------------------------------------

void keySelect(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    // option ESC: exit
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        // exit application
         glfwSetWindowShouldClose(window, 1);
    }
}