#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <learnopengl/filesystem.h>
#include <learnopengl/shader_m.h>
#include <learnopengl/camera.h>
#include <vector>
#include <iostream>
#include <cmath>
#include <random>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(10.0f, 8.0f, 10.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

float height(float x, float z) {
    return sinf(x * 0.3f) * cosf(z * 0.3f);
}

std::vector<glm::vec3> generateTerrain(int size, float step) {
    std::vector<glm::vec3> v;
    for (float x = -size; x <= size; x += step) {
        for (float z = -size; z <= size; z += step) {
            v.emplace_back(x, height(x, z), z);
        }
    }
    return v;
}
std::vector<glm::vec3> generateRing(float r, float y, int seg) {
    std::vector<glm::vec3> v;
    for (int i = 0; i < seg; i++) {
        float t = (float)i / seg * glm::two_pi<float>();
        v.emplace_back(r * cos(t)*1.1f, y*1.1f, r * sin(t)*1.1f);
    }
    return v;
}

float cubeVertices[] = {
    // positions
    -0.5f,-0.5f,-0.5f,
     0.5f,-0.5f,-0.5f,
     0.5f, 0.5f,-0.5f,
     0.5f, 0.5f,-0.5f,
    -0.5f, 0.5f,-0.5f,
    -0.5f,-0.5f,-0.5f,

    -0.5f,-0.5f, 0.5f,
     0.5f,-0.5f, 0.5f,
     0.5f, 0.5f, 0.5f,
     0.5f, 0.5f, 0.5f,
    -0.5f, 0.5f, 0.5f,
    -0.5f,-0.5f, 0.5f,

    -0.5f, 0.5f, 0.5f,
    -0.5f, 0.5f,-0.5f,
    -0.5f,-0.5f,-0.5f,
    -0.5f,-0.5f,-0.5f,
    -0.5f,-0.5f, 0.5f,
    -0.5f, 0.5f, 0.5f,

     0.5f, 0.5f, 0.5f,
     0.5f, 0.5f,-0.5f,
     0.5f,-0.5f,-0.5f,
     0.5f,-0.5f,-0.5f,
     0.5f,-0.5f, 0.5f,
     0.5f, 0.5f, 0.5f,

    -0.5f,-0.5f,-0.5f,
     0.5f,-0.5f,-0.5f,
     0.5f,-0.5f, 0.5f,
     0.5f,-0.5f, 0.5f,
    -0.5f,-0.5f, 0.5f,
    -0.5f,-0.5f,-0.5f,

    -0.5f, 0.5f,-0.5f,
     0.5f, 0.5f,-0.5f,
     0.5f, 0.5f, 0.5f,
     0.5f, 0.5f, 0.5f,
    -0.5f, 0.5f, 0.5f,
    -0.5f, 0.5f,-0.5f
};


int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
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
    Shader ourShader("7.4.camera.vs", "7.4.camera.fs");

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    auto terrain = generateTerrain(10, 0.5f);
    auto ring = generateRing(3.0f, 2.0f, 64);


    unsigned int cubeVAO, cubeVBO;
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);

    glBindVertexArray(cubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);



    // tell opengl for each sampler to which texture unit it belongs to (only has to be done once)
    // -------------------------------------------------------------------------------------------
    ourShader.use();


    float angle = 0.0f;
    glPointSize(4.0f);
    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.14f, 0.27f, 0.57f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        // activate shader
        ourShader.use();
        ourShader.setVec3("cameraPos", camera.Position);
        // pass projection matrix to shader (note that in this case it could change every frame)
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);
        ourShader.setMat4("projection", projection);

        // camera/view transformation
        glm::mat4 view = camera.GetViewMatrix();
        ourShader.setMat4("view", view);

        glEnable(GL_STENCIL_TEST);
        glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);



        // render boxes


      float time = glfwGetTime();
      glBindVertexArray(cubeVAO);
            glm::vec3 windAxis = glm::normalize(glm::vec3(1.0f, 0.0f, 0.3f)); // wind dir


        for (const auto& pos : terrain)
        {
             float sway = sin(time + pos.x * 0.5f + pos.z * 0.5f) * 0.4f;
             float dist = glm::distance(camera.Position, pos);

             // tune these
             float nearDist = 2.0f;
             float farDist = 6.0f;

             float t = glm::clamp(
                 1.0f - (dist - nearDist) / (farDist - nearDist),
                 0.0f, 1.0f
             );

             glm::vec3 toCam = camera.Position - pos;
             toCam.y = 0.0f;

             float len = glm::length(toCam);
             if (len > 0.0001f)
                 toCam /= len;
             float bendStrength = t * 0.4f;
             glm::vec3 windAxis = glm::normalize(glm::vec3(1.0f, 0.0f, 0.3f));
             glm::vec3 bendAxis = glm::normalize(glm::cross(toCam, glm::vec3(0, 1, 0)));

             glm::vec3 finalAxis = glm::normalize(
                 glm::mix(windAxis, bendAxis, t*2.0f)
             );
             float angle = sway * (1.0f - t) + bendStrength;


            glm::mat4 model = glm::mat4(1.0f);

            // move to world position
            model = glm::translate(model, pos);

            // move pivot to base
            model = glm::translate(model, glm::vec3(0.0f, -0.5f, 0.0f));

            // rotate around wind axis
            model = glm::rotate(model, angle, finalAxis);

            // move pivot back
            model = glm::translate(model, glm::vec3(0.0f, 0.5f, 0.0f));

            // make it tall and thin
            model = glm::scale(model, glm::vec3(0.2f, 1.2f, 0.2f));
            float h = pos.y;
            glm::vec4 baseColor(
                0.25f + h * 0.10f ,
                0.45f + h * 0.04f ,
                0.23f + h * 0.5f ,
                1.0f
            );

            // close-up tint (pick a vibe)
            glm::vec4 closeColor = glm::vec4(0.64f, 0.85f, 0.98f, 1.0f);

            glm::vec4 finalColor = glm::mix(baseColor, closeColor, t);

            ourShader.setVec4("ourColor", finalColor);

            ourShader.setMat4("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }


        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteBuffers(1, &cubeVBO);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
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
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

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
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}
