#pragma once

class GLFWwindow;

namespace Controls
{
    /*
     This function serves as the callback parameter for
        glfwSetKeyCallback function used in the main function

     Esc - close the app
     Space - Reload Shader


    TODO: ADD CAMERA CONTROLS
     w - angle alpha smaller (check handout)
     s - angle alpha bigger (check handout)
     a - angle beta smaller (check handout)
     d - angle beta bigger (check handout)
     e - bigger distance to target (zoom out) (check handout)
     q - smaller distance to target (zoom out)  (check handout)

    TODO: ADD/DECRESE SLICES
     +/z - Increase Slices. Reloads Models
     -/x - Decrease Slices. Reloads Models
    TODO: TRIGGER WIREFRAME
     m - Wireframe On/Off
    TODO: TRIGGER TEXTURE
     T - Texture Mapping On/Off
    TODO: TRIGGER NORMALS RENDER
     N - Rendering Normals On/Off
    TODO: TRIGGER NORMALS AVERAGE
     F - Average Normals On/Off

    */
    void keyCallback(GLFWwindow* pWindow, int key, int scancode, int action, int mods);
    void RejustSlice(const float& cf_in);
}