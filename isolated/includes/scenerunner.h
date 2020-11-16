#include "cookbookogl.h"
#include "scene.h"
#include <GLFW/glfw3.h>
#include "glutils.h"

#define WIN_WIDTH 800
#define WIN_HEIGHT 600

#include <map>
#include <string>
#include <fstream>
#include <iostream>
#include <memory>
#include <iostream>

class SceneRunner {
private:
    GLFWwindow * window;
    int fbw, fbh;
    bool debug;           // Set true to enable debug messages
    bool animate;
public:
    SceneRunner(const std::string & windowTitle, int width = WIN_WIDTH, int height = WIN_HEIGHT, int samples = 0) : debug(true), animate(false) {
        // Initialize GLFW
        if( !glfwInit() ) exit( EXIT_FAILURE );

#ifdef __APPLE__
        // Select OpenGL 4.1
        glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 4 );
        glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 1 );
#else
        // Select OpenGL 4.6
        glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 4 );
        glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 5 );
#endif
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
        if(debug)
			glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
        if(samples > 0) {
            glfwWindowHint(GLFW_SAMPLES, samples);
        }

        // Open the window
        window = glfwCreateWindow( WIN_WIDTH, WIN_HEIGHT, windowTitle.c_str(), NULL, NULL );
        if( ! window ) {
			std::cerr << "Unable to create OpenGL context." << std::endl;
            glfwTerminate();
            exit( EXIT_FAILURE );
        }
        glfwMakeContextCurrent(window);

        // Get framebuffer size
        glfwGetFramebufferSize(window, &fbw, &fbh);

        // Load the OpenGL functions.
        if(!gladLoadGL()) { exit(-1); }

        GLUtils::dumpGLInfo();

        // Initialization
        glClearColor(0.5f,0.5f,0.5f,1.0f);
#ifndef __APPLE__
		if (debug) {
			glDebugMessageCallback(GLUtils::debugCallback, nullptr);
			glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, GL_TRUE);
			glDebugMessageInsert(GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_MARKER, 0,
				GL_DEBUG_SEVERITY_NOTIFICATION, -1, "Start debugging");
		}
#endif
    }

    int run(std::unique_ptr<Scene> scene) {        
        // Enter the main loop
        mainLoop(window, std::move(scene));

#ifndef __APPLE__
		if( debug )
			glDebugMessageInsert(GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_MARKER, 1,
				GL_DEBUG_SEVERITY_NOTIFICATION, -1, "End debug");
#endif

		// Close window and terminate GLFW
		glfwTerminate();

        // Exit program
        return EXIT_SUCCESS;
    }

    static std::string parseCLArgs(int argc, char ** argv, std::map<std::string, std::string> & sceneData) {
        if( argc < 2 ) {
            printHelpInfo(argv[0], sceneData);
            exit(EXIT_FAILURE);
        }

        std::string recipeName = argv[1];
        auto it = sceneData.find(recipeName);
        if( it == sceneData.end() ) {
            printf("Unknown recipe: %s\n\n", recipeName.c_str());
            printHelpInfo(argv[0], sceneData);
            exit(EXIT_FAILURE);
        }

        return recipeName;
    }

private:

    static void printHelpInfo(const char * exeFile,  std::map<std::string, std::string> & sceneData) {
        printf("Usage: %s recipe-name\n\n", exeFile);
        printf("Recipe names: \n");
        for( auto it : sceneData ) {
            printf("  %11s : %s\n", it.first.c_str(), it.second.c_str());
        }
    }

    void mainLoop(GLFWwindow * window, std::unique_ptr<Scene> scene) {
        int width, height;
        double xpos, ypos;
        std::string keypress;
        scene->setDimensions(fbw, fbh);
        scene->initScene();
        scene->resize(fbw, fbh);

        while( ! glfwWindowShouldClose(window) && !glfwGetKey(window, GLFW_KEY_ESCAPE) ) {
            keypress = "";

            GLUtils::checkForOpenGLError(__FILE__,__LINE__);

            processKeypress(window, keypress);
            scene->update(float(glfwGetTime()), keypress);
            if (glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED)
            {
                glfwGetCursorPos(window, &xpos, &ypos);
                scene->updateMouseMovement(xpos, ypos);
            }
            scene->render();

            glfwSwapBuffers(window);
            glfwGetFramebufferSize(window, &width, &height);
            scene->resize( width, height);

            glfwPollEvents();
			
			
			scene->animate(animate);
        }
    }

    void processKeypress(GLFWwindow* window, std::string& keypress)
    {
        if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
            keypress = "gold";
        else if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
            keypress = "copper";
        else if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
            keypress = "aluminum";
        else if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS)
            keypress = "titanium";
        else if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS)
            keypress = "silver";
        else if (glfwGetKey(window, GLFW_KEY_6) == GLFW_PRESS)
            keypress = "noMetal";
        else if (glfwGetKey(window, GLFW_KEY_7) == GLFW_PRESS)
            keypress = "minus_roughness";
        else if (glfwGetKey(window, GLFW_KEY_8) == GLFW_PRESS)
            keypress = "plus_roughness";
        else if (glfwGetKey(window, GLFW_KEY_9) == GLFW_PRESS)
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        else if (glfwGetKey(window, GLFW_KEY_0) == GLFW_PRESS)
        {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            keypress = "mouseEnable";
        }
        else if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            keypress = "forward";
        else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            keypress = "backward";
        else if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            keypress = "left";
        else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            keypress = "right";
        else if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS)
            keypress = "meshOne";
        else if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS)
            keypress = "meshTwo";
        else if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
            keypress = "meshThree";
        else if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
            keypress = "arrow_up";
        else if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
            keypress = "arrow_down";
        else if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
            keypress = "arrow_left";
        else if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
            keypress = "arrow_right";
        else if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
            animate = false;
        else if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
            animate = true;
    }
};
