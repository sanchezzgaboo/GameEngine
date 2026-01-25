#include <SFML/Graphics.hpp>
#include <algorithm>
#include <imgui.h>
#include <imgui-SFML.h>
#include <iostream>
#include "Camera.h"
#include "Mesh.h"
#include "Mat4.h"

unsigned int screenwidth = 600;
unsigned int screenheight = 600;



int main()
{
    mesh meshCube;
    mat4x4 matProj;
    Camera cam;
    float fYaw;
    float movementX;
    float movementY;
    int prevX;
    int prevY;
    bool isPanning = false;
    sf::Vector2i lastMousePosition;
    
    
    //load File
    //projection matrix
    matProj = Matrix_MakeProjection(90.0f, (float)screenheight / (float)screenwidth, 0.1f, 1000.0f);
    float fTheta = 0.0f;
    float rotationSpeed = 0.5f;
    
    
    sf::RenderWindow window(sf::VideoMode({screenwidth, screenheight}), "SFML works!");
    // Set the frame rate limit to 60 FPS
    //window.setFramerateLimit(60);

    // get the local mouse position (relative to a window)
    bool leftButtonPressed = false;
    //Imgui Stuff
    ImGui::SFML::Init(window);
    bool checkboxState = false; // Initial state of the checkbox
    bool cartesianState = false; // Initial state of the checkbox
    bool lightState = false; // Initial state of the checkbox

    
    float mousePanSens = 0.2f;
    float Res = 2.0f;

    meshCube.generatePlane(10, 5, 1.0f/Res);

    sf::Clock clock;
    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            // Pass the polled event to ImGui properly (one event at a time)
            ImGui::SFML::ProcessEvent(window, *event);
            if (event->is<sf::Event::Closed>())
                window.close();
            if (const auto* mouseButtonPressed = event->getIf<sf::Event::MouseButtonPressed>())
            {
                if (mouseButtonPressed->button == sf::Mouse::Button::Right)
                {
                    std::cout << "the right button was pressed" << std::endl;
                }

                if (mouseButtonPressed->button == sf::Mouse::Button::Middle)
                {
                    std::cout << "mouse x: " << mouseButtonPressed->position.x << std::endl;
                    std::cout << "mouse y: " << mouseButtonPressed->position.y << std::endl;
                }

                if (mouseButtonPressed->button == sf::Mouse::Button::Left)
                {
                    std::cout << "the left button was pressed" << std::endl;
                    leftButtonPressed = true;
                }
                

            }
            if(const auto* mouseButtonReleased = event->getIf<sf::Event::MouseButtonReleased>()){
                if (mouseButtonReleased->button == sf::Mouse::Button::Left){
                    leftButtonPressed = false;
                }
            }
        
        }

        sf::Time elapsedTime = clock.restart();

        ImGui::SFML::Update(window, elapsedTime);
        
        // --- Global top toolbar ---
        if (ImGui::BeginMainMenuBar()) {
            if (ImGui::BeginMenu("File")) {
                if (ImGui::MenuItem("Exit")) {
                    window.close();
                }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Graph")) {
                if (ImGui::MenuItem("Add Node")) {  }
                if (ImGui::MenuItem("Add Edge")) { /* ... */ }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("View")) {
                if (ImGui::MenuItem("Zoom In")) { /* ... */ }
                if (ImGui::MenuItem("Zoom Out")) { /* ... */ }
                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }
        
        ImGui::Begin("Debug Console");
        ImGuiIO& io = ImGui::GetIO();

        ImGui::Checkbox("Enable Lightning", &lightState);
        ImGui::Checkbox("Enable WireFrame", &checkboxState);
        ImGui::Checkbox("Enable Cartesian Plane", &cartesianState);

        ImGui::SliderFloat("Mouse Pan Sensitivity", &mousePanSens, 0.0f, 0.5f);
        ImGui::SliderFloat("Resolution", &Res, 1.0, 5.0f);
        ImGui::TextColored(ImVec4(1,1,0,1), "Important Stuff");
        ImGui::BeginChild("Scrolling");
        ImGui::Text("Camera Pos:  %.2f,  %.2f,  %.2f", cam.vCamera.x, cam.vCamera.y, cam.vCamera.z);
        ImGui::Text("Camera Look Vector:  %.2f,  %.2f,  %.2f", cam.vLookDir.x, cam.vLookDir.y, cam.vLookDir.z);
        ImGui::Text("Camera Up Vector:  %.2f,  %.2f,  %.2f", cam.vUp.x, cam.vUp.y, cam.vUp.z);
        ImGui::Text("Camera Up Vector:  %.2f,  %.2f,  %.2f", cam.vTarget.x, cam.vTarget.y, cam.vTarget.z);
        ImGui::EndChild();
        ImGui::End();

        window.clear();

        
        
        mat4x4 matRotZ, matRotX;
        float deltaTime = elapsedTime.asSeconds();
        if(!io.WantCaptureMouse){
            if(leftButtonPressed){
            if(!isPanning){
                isPanning = true;
                lastMousePosition = sf::Mouse::getPosition(window);
            }
            else{
                sf::Vector2i currentMousePosition = sf::Mouse::getPosition(window);
                sf::Vector2f delta = static_cast<sf::Vector2f>(currentMousePosition-lastMousePosition);
                cam.vCamera.x -= delta.x * mousePanSens/10;
                cam.vCamera.y += delta.y * mousePanSens/10;
                lastMousePosition = currentMousePosition;
            }

            }
            else{
                isPanning = false;
            }
        }
        //fTheta += rotationSpeed * deltaTime;
        matRotZ = Matrix_MakeRotationZ(fTheta * 0.5f);
		matRotX = Matrix_MakeRotationX(fTheta);

        mat4x4 matTrans;
		matTrans = Matrix_MakeTranslation(0.0f, 0.0f, 8.0f);

		mat4x4 matWorld;
		matWorld = Matrix_MakeIdentity();	// Form World Matrix
		matWorld = Matrix_MultiplyMatrix(matRotZ, matWorld); // Transform by rotation
		matWorld = Matrix_MultiplyMatrix(matWorld, matTrans); // Transform by translation
        cam.vTarget = cam.vCamera + cam.vLookDir;
        mat4x4 matCamera = Matrix_PointAt(cam.vCamera, cam.vTarget, cam.vUp);

        //VIEW MATRIX
        mat4x4 matView = Matrix_QuickInverse(matCamera);
        
        std::vector<triangle> vecTrianglestoRaster;

        for(auto tri: meshCube.tris){


            triangle triProjected, triTranslated, triViewed;
            triTranslated.p[0]=Matrix_MultiplyVector(matWorld, tri.p[0], false);
            triTranslated.p[1]=Matrix_MultiplyVector(matWorld, tri.p[1], false);
            triTranslated.p[2]=Matrix_MultiplyVector(matWorld, tri.p[2], false);
            

            //calculate normals

            sf::Vector3f normal, line1, line2; //Para hacer producto cruz entre 2 vectores de un triangulo (clockwise)
            line1 = triTranslated.p[1]-triTranslated.p[0];
            line2 = triTranslated.p[2]-triTranslated.p[0];
            
            //producto cruz
            normal = line1.cross(line2);
            
            
            //normalizar
            normal = normal.normalized();

            sf::Vector3f vCameraRay = triTranslated.p[0]-cam.vCamera;
            
            //if(normal.z < 0)//si el normal apunta hacia la camara ( NO ES MUY UTIL)
            if(normal.dot(vCameraRay) < 0.0f) //producto punto entre el normal y la proyeccion a la camara.
            { 
                if(lightState){
                    //illumination
                    sf::Vector3f light_direction = {0.0f, 0.0f, -1.0f};
                    light_direction = light_direction.normalized();
                    float dp = std::max(0.1f, light_direction.dot(normal));
                    
                    triProjected.r *= dp;
                    triProjected.g *= dp;
                    triProjected.b *= dp;
                }
                else{
                    triProjected.r = 20;
                    triProjected.g *= 20;
                    triProjected.b *= 20;
                }
                

                //Primero convertir espacio global -> vista camara
                triViewed.p[0]=Matrix_MultiplyVector(matView, triTranslated.p[0], true);
                triViewed.p[1]=Matrix_MultiplyVector(matView,triTranslated.p[1], true);
                triViewed.p[2]=Matrix_MultiplyVector(matView, triTranslated.p[2], true);
            


                //camara -> pantalla
                triProjected.p[0]=Matrix_MultiplyVector(matProj, triViewed.p[0], true);
                triProjected.p[1]=Matrix_MultiplyVector(matProj,triViewed.p[1], true);
                triProjected.p[2]=Matrix_MultiplyVector(matProj, triViewed.p[2], true);
            
                //offset into visible normalised space
                sf::Vector3f vOffsetView(1,1,0);
                triProjected.p[0] += vOffsetView;
                triProjected.p[1] += vOffsetView;   
                triProjected.p[2] += vOffsetView;   


                triProjected.p[0].x *=0.5f*(float)screenwidth;
                triProjected.p[0].y *=0.5f*(float)screenheight;
                triProjected.p[1].x *=0.5f*(float)screenwidth;
                triProjected.p[1].y *=0.5f*(float)screenheight;
                triProjected.p[2].x *=0.5f*(float)screenwidth;
                triProjected.p[2].y *=0.5f*(float)screenheight;

                //Pintar de atras hacia adelante para evitar fallos

                vecTrianglestoRaster.push_back(triProjected);

                
            }
            
        }

        std::sort(vecTrianglestoRaster.begin(), vecTrianglestoRaster.end(), [](triangle &t1, triangle &t2){
            float z1 = (t1.p[0].z + t1.p[1].z + t1.p[2].z)/3.0f;
            float z2 = (t2.p[0].z + t2.p[1].z + t2.p[2].z)/3.0f;
            return z1 > z2;
        });
        for(auto &triProjected:vecTrianglestoRaster){
            sf::VertexArray currentTriangle(sf::PrimitiveType::Triangles, 3);
        
            currentTriangle[0].position = sf::Vector2f(triProjected.p[0].x, triProjected.p[0].y);
            currentTriangle[1].position = sf::Vector2f(triProjected.p[1].x, triProjected.p[1].y);
            currentTriangle[2].position = sf::Vector2f(triProjected.p[2].x, triProjected.p[2].y);
            
            
            currentTriangle[0].color = sf::Color(triProjected.r, triProjected.g, triProjected.b);
            currentTriangle[1].color = sf::Color(triProjected.r, triProjected.g, triProjected.b);
            currentTriangle[2].color = sf::Color(triProjected.r, triProjected.g, triProjected.b);
            
            if(checkboxState){ //mostrar wireframe
                sf::VertexArray currentWireFrame(sf::PrimitiveType::LineStrip, 4);
                currentWireFrame[0].position = sf::Vector2f(triProjected.p[0].x, triProjected.p[0].y);
                currentWireFrame[1].position = sf::Vector2f(triProjected.p[1].x, triProjected.p[1].y);
                currentWireFrame[2].position = sf::Vector2f(triProjected.p[2].x, triProjected.p[2].y);
                currentWireFrame[3].position = sf::Vector2f(triProjected.p[0].x, triProjected.p[0].y);

                currentWireFrame[0].color = sf::Color::Yellow;
                currentWireFrame[1].color = sf::Color::Yellow;
                currentWireFrame[2].color = sf::Color::Yellow;
                currentWireFrame[3].color = sf::Color::Yellow;
                window.draw(currentWireFrame);
            }
            if(cartesianState){
                // Draw simple Cartesian axes (avoid invalid indices)
                sf::VertexArray axes(sf::PrimitiveType::Lines, 4);
                // Horizontal axis
                axes[0].position = sf::Vector2f(0.0f, (float)screenheight / 2.0f);
                axes[1].position = sf::Vector2f((float)screenwidth, (float)screenheight / 2.0f);
                // Vertical axis
                axes[2].position = sf::Vector2f((float)screenwidth / 2.0f, 0.0f);
                axes[3].position = sf::Vector2f((float)screenwidth / 2.0f, (float)screenheight);
                for (int i = 0; i < 4; ++i) axes[i].color = sf::Color::Green;
                window.draw(axes);
            }


            window.draw(currentTriangle);
        }
        // Render ImGui once per frame (after drawing scene, before display)
        ImGui::SFML::Render(window);

        window.display();
    }
    ImGui::SFML::Shutdown();
}