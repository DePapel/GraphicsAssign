#include "Controls.h"
#include "Level.h"
#include "Models.h"
#include <GLFW/glfw3.h>


void Controls::RejustSlice(const float& cf_in)
{
    if (auto temp = Level::GetPtr()->getAllObject().begin()[0]) {
        auto SliceTemp = temp->slices;
        SliceTemp += cf_in;
        if (SliceTemp > 3)
        {
            temp->slices = SliceTemp;
        }
    }
    for (auto obj : Level::GetPtr()->getAllObject()) {
        obj->CreateBuffers();
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
        
    }
    else
    {
        Level::GetPtr()->keyStateW = false;
        Level::GetPtr()->keyStateS = false;
        Level::GetPtr()->keyStateA = false;
        Level::GetPtr()->keyStateD = false;
    }

    //TODO: ADD/DECRESE SLICES
    if (action == GLFW_PRESS || action == GLFW_REPEAT)
    {
        if (key == GLFW_KEY_Z)
            RejustSlice(1);
        if (key == GLFW_KEY_X)
            RejustSlice(-1);
    }
    //TODO: TRIGGER WIREFRAME
    if(key == GLFW_KEY_M) 
    {
        //Draw�Ӽ��� Triangles ��� line���� ��ġ�� ����������???
    }
    if (action == GLFW_PRESS)
    {
        //TODO: TRIGGER TEXTURE
        if (key == GLFW_KEY_T)
            Level::GetPtr()->shaderSW = !Level::GetPtr()->shaderSW;
        //TODO: TRIGGER NORMALS RENDER
        //TODO: TRIGGER NORMALS AVERAGE
        if (key == GLFW_KEY_F)
        {
            Level::GetPtr()->normalVecSw = !Level::GetPtr()->normalVecSw;
            for (auto obj : Level::GetPtr()->getAllObject()) 
            {
                //��� obj�� Normals�� �ٽ� ����ϴ� First Row�� �ִ� �����鵵 ����� �Ǵ°� ����.
                //Second Row�� �ִ�(Create�Լ��� ����Ͽ� ����� �����鸸 ��, obj������ �ε��� ���� �ƴ� �ֵ鸸 ����� �ǰ�)
                obj->CalculateNormals();
            }
        }
    }

}
