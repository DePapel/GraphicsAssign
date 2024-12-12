#include "Controls.h"
#include "Level.h"
#include "Models.h"
#include <GLFW/glfw3.h>


void Controls::RejustSlice(const float& cf_in)
{
    if (auto temp = Level::GetPtr()->getAllObject().begin()[0]) 
    {
        auto SliceTemp = temp->slices;
        SliceTemp += cf_in;
        if (SliceTemp > 3)
        {
            temp->slices = SliceTemp;
        }
    }
    for (auto obj : Level::GetPtr()->getAllObject()) 
    {
        if (obj->transf.mesh == "CONE" || obj->transf.mesh == "CYLINDER" || obj->transf.mesh == "SPHERE")
            obj->CreateModels();
    }
}

void Controls::keyCallback(GLFWwindow* pWindow, int key, int scancode, int action, int mods)
{
    //ESC - CLOSE WINDOW
    if (action == GLFW_PRESS)
        if (key == GLFW_KEY_ESCAPE)
            glfwSetWindowShouldClose(pWindow, GL_TRUE);

    //SPACE - RELOAD SHADER
    if (action == GLFW_PRESS)
        if (key == GLFW_KEY_SPACE)
            Level::GetPtr()->ReloadShaderProgram();


    //TODO: ADD CAMERA CONTROLS
    if (action == GLFW_PRESS || action == GLFW_REPEAT)
    {
        if (key == GLFW_KEY_W)
            Level::GetPtr()->keyStateW = true;
        if (key == GLFW_KEY_S)
            Level::GetPtr()->keyStateS = true;
        if (key == GLFW_KEY_A)
            Level::GetPtr()->keyStateA = true;
        if (key == GLFW_KEY_D)
            Level::GetPtr()->keyStateD = true;
        if (key == GLFW_KEY_E)
            Level::GetPtr()->Joom(1.f);
        if (key == GLFW_KEY_Q)
            Level::GetPtr()->Joom(-1.f);

        //TODO: ADD/DECRESE SLICES
        if (key == GLFW_KEY_Z)
            RejustSlice(1);
        if (key == GLFW_KEY_X)
            RejustSlice(-1);
    }
    else
    {
        Level::GetPtr()->keyStateW = false;
        Level::GetPtr()->keyStateS = false;
        Level::GetPtr()->keyStateA = false;
        Level::GetPtr()->keyStateD = false;
    }
    
    if (action == GLFW_PRESS)
    {
        //TODO: TRIGGER WIREFRAME
        if (key == GLFW_KEY_M)
            Level::GetPtr()->wireSW = !Level::GetPtr()->wireSW;

        //TODO: TRIGGER TEXTURE
        if (key == GLFW_KEY_T)
            Level::GetPtr()->shaderSW++;

        //TODO: TRIGGER NORMALS RENDER
        if (key == GLFW_KEY_N)
            Level::GetPtr()->norOnOffSW = !Level::GetPtr()->norOnOffSW;

        //TODO: TRIGGER NORMALS AVERAGE
        if (key == GLFW_KEY_F)
        {
            Level::GetPtr()->comavgSw = !Level::GetPtr()->comavgSw;
            for (auto obj : Level::GetPtr()->getAllObject()) 
            {
                //모든 obj의 Normals을 다시 계산하니 First Row에 있는 도형들도 계산이 되는거 같음.
                //Second Row에 있는(Create함수를 사용하여 만드는 도형들만 즉, obj파일을 로드한 것이 아닌 애들만 계산이 되게)
                obj->CalculateNormals();
            }
        }
        
    }

}
