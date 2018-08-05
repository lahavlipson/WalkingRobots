#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader_s.h"

#include "camera.h"
#include "mass.h"
#include "spring.h"
#include "robot.h"
#include <thread>         // std::thread
#include <mutex>          // std::mutex, std::unique_lock, std::defer_lock

#include <iostream>
#include <unordered_map>
#include <random>
#include <string>
#include <sstream>

#include "learn.h"
#include "starting_models.h"

#ifdef enable_graphics
#include <GLFW/glfw3.h>
#include "glp.h"
#include "unstructured_neural_network.h"
#include "individual.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
int renderRob();
#endif

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

std::mutex mtx;           // mutex for critical section
std::unique_lock<std::mutex> lck (mtx,std::defer_lock);
Robot rob(&mtx, 2.0f);

// camera
Camera camera(glm::vec3(0.75f, 2.0f, 8.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// lighting
glm::vec3 lightPos(6.2f, 7.0f, 5.0f);

void runSim(Robot *rob){
  //  std::cout << *(rob->network);
    rob->simulate(1000,10000);
}

int main()
{
    srand(time(0));
    rand();
    
    
    UnstructuredNeuralNetwork nn(40);
//    MultilayerNeuralNetwork nn(19, 1, 21);
//    std::cout << nn;return 0;
    rob = starting_models::getArrow();
    rob.setNN(&nn);
    
//    MultilayerNeuralNetwork nn2(helper::csvToVec("/Users/lahavlipson/Downloads/vibrating/bestNN.csv"));
//    rob = starting_models::getArrow();
//    rob.setNN(&nn2);

//    rob = learn::learnNeuralNetworkPareto(1);
    
    #ifdef enable_graphics
    std::thread thrd = std::thread(runSim, &rob);
    renderRob();
    thrd.join();
    #endif
    
    return 0;
    
}

#ifdef enable_graphics

int renderRob(){
    
    
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif
    
    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    
    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    
    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);
    
    // build and compile our shader zprogram
    // ------------------------------------
    Shader lightingShader("/Users/lahavlipson/Personal_Projects/Learn_OpenGL/Learn_OpenGL_Demo/Learn_OpenGL_Demo/myvertexshader.glsl", "/Users/lahavlipson/Personal_Projects/Learn_OpenGL/Learn_OpenGL_Demo/Learn_OpenGL_Demo/myfragshader.glsl");
    
    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------

    // first, configure the cube's VAO (and VBO)
    
    
    // second, configure the light's VAO (VBO stays the same; the vertices are the same for the light object which is also a 3D cube)
    
    const double SPHERE_SIZE = 0.2;
    const double SPRING_WIDTH = 0.05;
    
    lightingShader.use();
    lightingShader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
    lightingShader.setVec3("lightPos", lightPos);
    
    //FLOOR*********************
    const float FLOOR_RADIUS = 100;
    float Y = GROUND_LEVEL - SPHERE_SIZE;
    float floorVertices[] = {
        // first triangle
        -FLOOR_RADIUS,  Y, -FLOOR_RADIUS, 0.0f, 1.0f, 0.0f, // top left
        
        FLOOR_RADIUS, Y, FLOOR_RADIUS, 0.0f, 1.0f, 0.0f, // bottom right
        FLOOR_RADIUS,  Y, -FLOOR_RADIUS, 0.0f, 1.0f, 0.0f,  // top right
        // second triangle
        -FLOOR_RADIUS,  Y, -FLOOR_RADIUS, 0.0f, 1.0f, 0.0f,  // top left
        -FLOOR_RADIUS, Y, FLOOR_RADIUS, 0.0f, 1.0f, 0.0f, // bottom left
        FLOOR_RADIUS, Y, FLOOR_RADIUS, 0.0f, 1.0f, 0.0f // bottom right
        
    };
    unsigned int floor_VBO, floor_VAO;
    glGenVertexArrays(1, &floor_VAO);
    glGenBuffers(1, &floor_VBO);
    glBindVertexArray(floor_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, floor_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(floorVertices), floorVertices, GL_STATIC_DRAW);
    
    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    
    
    const double GAP_SIZE = 5;
    std::vector<double> gridPoints;
    for (double x = -FLOOR_RADIUS+GAP_SIZE; x < FLOOR_RADIUS; x += GAP_SIZE){
        gridPoints.push_back(x);
        gridPoints.push_back(Y+0.05);
        gridPoints.push_back(-FLOOR_RADIUS);
        
        gridPoints.push_back(x);
        gridPoints.push_back(Y+0.05);
        gridPoints.push_back(FLOOR_RADIUS);
    }
    for (double z = -FLOOR_RADIUS+GAP_SIZE; z < FLOOR_RADIUS; z += GAP_SIZE){
        gridPoints.push_back(-FLOOR_RADIUS);
        gridPoints.push_back(Y+0.05);
        gridPoints.push_back(z);
        
        gridPoints.push_back(FLOOR_RADIUS);
        gridPoints.push_back(Y+0.05);
        gridPoints.push_back(z);
    }
    unsigned int grid_VBO, grid_VAO;
    glGenVertexArrays(1, &grid_VAO);
    glGenBuffers(1, &grid_VBO);
    glBindVertexArray(grid_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, grid_VBO);
    glBufferData(GL_ARRAY_BUFFER, gridPoints.size()*sizeof(double), gridPoints.data(), GL_STATIC_DRAW);
    
    // position attribute
    glVertexAttribPointer(0, 3, GL_DOUBLE, GL_FALSE, 3 * sizeof(double), (void*)0);
    glEnableVertexAttribArray(0);
    
    
    //FLOOR**********************
    
    
    //Masses
    std::vector<double> sphereVerts = glp::sphere(3, SPHERE_SIZE);
    unsigned int sphere_VBO, sphere_VAO;
    glGenVertexArrays(1, &sphere_VAO);
    glGenBuffers(1, &sphere_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, sphere_VBO);
    glBufferData(GL_ARRAY_BUFFER, sphereVerts.size()*sizeof(double), sphereVerts.data(), GL_STATIC_DRAW);
    glBindVertexArray(sphere_VAO);
    // position attribute
    glVertexAttribPointer(0, 3, GL_DOUBLE, GL_FALSE, 6 * sizeof(double), (void*)0);
    glEnableVertexAttribArray(0);
    // normal attribute
    glVertexAttribPointer(1, 3, GL_DOUBLE, GL_FALSE, 6 * sizeof(double), (void*)(3 * sizeof(double)));
    glEnableVertexAttribArray(1);
    
    
    int frameNum = 0;
    
    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        
        frameNum += 1;
        //std::cout << "frameNum: " << frameNum << std::endl;
        
        // per-frame time logic
        // --------------------
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        
        // input
        // -----
        processInput(window);
        
        // render
        // ------
        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // camera
        // ------
        lightingShader.setVec3("viewPos", camera.Position);
        
        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        lightingShader.setMat4("projection", projection);
        lightingShader.setMat4("view", view);
        
        
        // render the masses
        lightingShader.setVec3("objectColor", 1.0f, 0.5f, 0.31f);
        glBindVertexArray(sphere_VAO);
        glm::mat4 model;
        for (Mass *m : rob.masses) {
            model = glm::mat4();
            lck.lock();
            model = glm::translate(model, glm::vec3(m->pos));
            lck.unlock();
            lightingShader.setMat4("model", model);
            glDrawArrays(GL_TRIANGLES, 0, sphereVerts.size()/6);
        }

        //create the springs
        const int numSprings = rob.springsMap.size();
        std::vector<double> cylinderPoints[numSprings];
        unsigned int cylinder_VBO[numSprings];
        unsigned int cylinder_VAO[numSprings];
        glGenVertexArrays(numSprings, cylinder_VAO);
        glGenBuffers(numSprings, cylinder_VBO);
        /*glGenVertexArrays(1, &cylinder_VAO[0]);
         glGenBuffers(1, &cylinder_VBO[0]);*/
        
        
        for (int i=0; i<rob.springsMap.size(); i++){
            cylinderPoints[i] = glp::cylinder(20, rob.getSprings()[i]->calcLength(), SPRING_WIDTH);
            
            glBindBuffer(GL_ARRAY_BUFFER, cylinder_VBO[i]);
            glBufferData(GL_ARRAY_BUFFER, cylinderPoints[i].size()*sizeof(double), cylinderPoints[i].data(), GL_STATIC_DRAW);
            glBindVertexArray(cylinder_VAO[i]);
            // position attribute
            glVertexAttribPointer(0, 3, GL_DOUBLE, GL_FALSE, 6 * sizeof(double), (void*)0);
            glEnableVertexAttribArray(0);
            // normal attribute
            glVertexAttribPointer(1, 3, GL_DOUBLE, GL_FALSE, 6 * sizeof(double), (void*)(3 * sizeof(double)));
            glEnableVertexAttribArray(1);
            
        }
        
        //render the springs
        lightingShader.setVec3("objectColor", 0.1f, 0.5f, 0.81f);
        std::vector<Spring *> springs = rob.getSprings();
        for (int i=0; i<springs.size(); i++){
            glBindVertexArray(cylinder_VAO[i]);
            lck.lock();
            glm::vec3 diff = glm::normalize(*(springs[i]->p2)-*(springs[i]->p1));
            glm::vec3 crossProd = glm::cross(glm::vec3(0,1,0),diff);
            float angleDiff = float(acos(glm::dot(glm::vec3(0,1,0), diff)));
            model = glm::mat4();
            model = glm::translate(model, glm::vec3(*(springs[i]->p1)));
            lck.unlock();
            model = glm::rotate(model, angleDiff, crossProd);
            lightingShader.setMat4("model", model);
            glDrawArrays(GL_TRIANGLES, 0, cylinderPoints[i].size()/6);
        }
        
        
        //RENDER THE FLOOR
        lightingShader.setVec3("objectColor", 0.65f, 0.65f, 0.65f);
        model = glm::mat4();
        lightingShader.setMat4("model", model);
        glBindVertexArray(floor_VAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        
        //RENDER THE GRID
        lightingShader.setVec3("objectColor", 0.15f, 0.15f, 0.15f);
        glBindVertexArray(grid_VAO);
        glDrawArrays(GL_LINES, 0, gridPoints.size()/3);
        
        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
//    glDeleteVertexArrays(1, &cubeVAO);
//    glDeleteVertexArrays(1, &lightVAO);
//    glDeleteBuffers(1, &VBO);
    
    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    rob.stopSim = true;
    //thrd.join();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        camera.ProcessKeyboard(DOWN, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        camera.ProcessKeyboard(UP, deltaTime);
    
    
    
    rob.pushForce = glm::dvec3();
    if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS)
        rob.pushForce = glm::dvec3(0,0.1,-5);
    if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS)
        rob.pushForce = glm::dvec3(0,0.1,5);
    if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
        rob.pushForce = glm::dvec3(-5,0.1,0);
    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
        rob.pushForce = glm::dvec3(5,0.1,0);
    if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS)
        rob.pushForce = glm::dvec3(0,5,0);
    static int oldDeletedState = GLFW_PRESS;
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS && oldDeletedState != GLFW_PRESS){
        rob.removeMass(*(rob.masses.begin()));
    }
    oldDeletedState = glfwGetKey(window, GLFW_KEY_R);
    
    
    static int oldAttachedState = GLFW_PRESS;
    if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS && oldAttachedState != GLFW_PRESS){
        rob.mutateMasses();
    }
    oldAttachedState = glfwGetKey(window, GLFW_KEY_T);
    
    
    
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}


// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }
    
    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
    
    lastX = xpos;
    lastY = ypos;
    
    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}

#endif
