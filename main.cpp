#include <SFML/Graphics.hpp>
#include <cmath>
#include <fstream>
#include <strstream>
#include <algorithm>
#include "imgui/imgui.h"
#include "imgui/imgui-SFML.h"
#include <iostream>

unsigned int screenwidth = 600;
unsigned int screenheight = 600;

struct vec3d
{
	float x = 0;
	float y = 0;
	float z = 0;
	float w = 1; // Need a 4th term to perform sensible matrix vector multiplication
};

struct triangle{
    sf::Vector3f p[3];
    int r = 255;
    int g = 255;
    int b = 255;
};
struct mesh{
    std::vector<triangle> tris;
    bool LoadFromObjectFile(std::string sFilename)
	{
		std::ifstream f(sFilename);
		if (!f.is_open())
			return false;

		// Local cache of verts
		std::vector<sf::Vector3f> verts;

		while (!f.eof())
		{
			char line[128];
			f.getline(line, 128);

			std::stringstream s;
			s << line;

			char junk;

			if (line[0] == 'v')
			{
				sf::Vector3f v;
				s >> junk >> v.x >> v.y >> v.z;
				verts.push_back(v);
			}

			if (line[0] == 'f')
			{
				int f[3];
				s >> junk >> f[0] >> f[1] >> f[2];
				tris.push_back({ verts[f[0] - 1], verts[f[1] - 1], verts[f[2] - 1] });
			}
		}

		return true;
	}

    bool generateLine(int length, int height){
        std::vector<sf::Vector3f> verts;
        for(int i = 0; i < height; i++){ 
            for(int j = 0; j < length; j++){
                sf::Vector3f v;
                v.x = 0;
                v.y=i;
                v.z=j;
                verts.push_back(v);
            }
        }

        // Build two triangles per grid cell (quad) avoiding out-of-range accesses.
        // Iterate rows (height) and columns (length), but stop one short on each
        // dimension because each cell references the next row/column.
        if (length > 1 && height > 1) {
            for (int i = 0; i < height - 1; ++i) {
                for (int j = 0; j < length - 1; ++j) {
                    int idx = i * length + j;
                    // First triangle of the quad
                    tris.push_back({ verts[idx], verts[idx + length], verts[idx + length + 1] });
                    // Second triangle of the quad
                    tris.push_back({ verts[idx], verts[idx + length + 1], verts[idx + 1] });
                }
            }
        }


        return true;

    }
};
struct mat4x4{
    float m[4][4] = {0};
};

void MultiplyMatrixVector(sf::Vector3f &i, sf::Vector3f &o, mat4x4 &m){
    o.x = i.x*m.m[0][0] + i.y*m.m[1][0] + i.z*m.m[2][0]+m.m[3][0];
    o.y = i.x*m.m[0][1] + i.y*m.m[1][1] + i.z*m.m[2][1]+m.m[3][1];
    o.z = i.x*m.m[0][2] + i.y*m.m[1][2] + i.z*m.m[2][2]+m.m[3][2];
    float w = i.x*m.m[0][3] + i.y*m.m[1][3] + i.z*m.m[2][3]+m.m[3][3];

    if(w != 0){
        o.x /= w;
        o.y /= w;
        o.z /= w;
    }
    
}

sf::Vector3f Matrix_MultiplyVector(mat4x4 &m, sf::Vector3f &input, bool normalizeIntoSpace)
{
    vec3d i;
    i.x = input.x;
    i.y = input.y;
    i.z = input.z;
    vec3d v;
    v.x = i.x * m.m[0][0] + i.y * m.m[1][0] + i.z * m.m[2][0] + i.w * m.m[3][0];
    v.y = i.x * m.m[0][1] + i.y * m.m[1][1] + i.z * m.m[2][1] + i.w * m.m[3][1];
    v.z = i.x * m.m[0][2] + i.y * m.m[1][2] + i.z * m.m[2][2] + i.w * m.m[3][2];
    v.w = i.x * m.m[0][3] + i.y * m.m[1][3] + i.z * m.m[2][3] + i.w * m.m[3][3];
    if(normalizeIntoSpace){
        return sf::Vector3f(v.x/v.w,v.y/v.w,v.z/v.w);
    }
    else{
        return sf::Vector3f(v.x,v.y,v.z);
    }
}

mat4x4 Matrix_MakeIdentity()
{
    mat4x4 matrix;
    matrix.m[0][0] = 1.0f;
    matrix.m[1][1] = 1.0f;
    matrix.m[2][2] = 1.0f;
    matrix.m[3][3] = 1.0f;
    return matrix;
}

mat4x4 Matrix_MakeRotationX(float fAngleRad)
{
    mat4x4 matrix;
    matrix.m[0][0] = 1.0f;
    matrix.m[1][1] = cosf(fAngleRad);
    matrix.m[1][2] = sinf(fAngleRad);
    matrix.m[2][1] = -sinf(fAngleRad);
    matrix.m[2][2] = cosf(fAngleRad);
    matrix.m[3][3] = 1.0f;
    return matrix;
}

mat4x4 Matrix_MakeRotationY(float fAngleRad)
{
    mat4x4 matrix;
    matrix.m[0][0] = cosf(fAngleRad);
    matrix.m[0][2] = sinf(fAngleRad);
    matrix.m[2][0] = -sinf(fAngleRad);
    matrix.m[1][1] = 1.0f;
    matrix.m[2][2] = cosf(fAngleRad);
    matrix.m[3][3] = 1.0f;
    return matrix;
}

mat4x4 Matrix_MakeRotationZ(float fAngleRad)
{
    mat4x4 matrix;
    matrix.m[0][0] = cosf(fAngleRad);
    matrix.m[0][1] = sinf(fAngleRad);
    matrix.m[1][0] = -sinf(fAngleRad);
    matrix.m[1][1] = cosf(fAngleRad);
    matrix.m[2][2] = 1.0f;
    matrix.m[3][3] = 1.0f;
    return matrix;
}

mat4x4 Matrix_MakeTranslation(float x, float y, float z)
{
    mat4x4 matrix;
    matrix.m[0][0] = 1.0f;
    matrix.m[1][1] = 1.0f;
    matrix.m[2][2] = 1.0f;
    matrix.m[3][3] = 1.0f;
    matrix.m[3][0] = x;
    matrix.m[3][1] = y;
    matrix.m[3][2] = z;
    return matrix;
}

mat4x4 Matrix_MakeProjection(float fFovDegrees, float fAspectRatio, float fNear, float fFar)
{
    float fFovRad = 1.0f / tanf(fFovDegrees * 0.5f / 180.0f * 3.14159f);
    mat4x4 matrix;
    matrix.m[0][0] = fAspectRatio * fFovRad;
    matrix.m[1][1] = fFovRad;
    matrix.m[2][2] = fFar / (fFar - fNear);
    matrix.m[3][2] = (-fFar * fNear) / (fFar - fNear);
    matrix.m[2][3] = 1.0f;
    matrix.m[3][3] = 0.0f;
    return matrix;
}

mat4x4 Matrix_MultiplyMatrix(mat4x4 &m1, mat4x4 &m2)
{
    mat4x4 matrix;
    for (int c = 0; c < 4; c++)
        for (int r = 0; r < 4; r++)
            matrix.m[r][c] = m1.m[r][0] * m2.m[0][c] + m1.m[r][1] * m2.m[1][c] + m1.m[r][2] * m2.m[2][c] + m1.m[r][3] * m2.m[3][c];
    return matrix;
}

mat4x4 Matrix_PointAt(sf::Vector3f &pos, sf::Vector3f &target, sf::Vector3f &up)
{
    // Calculate new forward direction
    sf::Vector3f newForward = target - pos;
    newForward = newForward.normalized();

    // Calculate new Up direction
    sf::Vector3f a = newForward * up.dot(newForward);
    sf::Vector3f newUp = up - a;
    newUp = newUp.normalized();

    // New Right direction is easy, its just cross product
    sf::Vector3f newRight = newUp.cross(newForward);

    // Construct Dimensioning and Translation Matrix	
    mat4x4 matrix;
    matrix.m[0][0] = newRight.x;	matrix.m[0][1] = newRight.y;	matrix.m[0][2] = newRight.z;	matrix.m[0][3] = 0.0f;
    matrix.m[1][0] = newUp.x;		matrix.m[1][1] = newUp.y;		matrix.m[1][2] = newUp.z;		matrix.m[1][3] = 0.0f;
    matrix.m[2][0] = newForward.x;	matrix.m[2][1] = newForward.y;	matrix.m[2][2] = newForward.z;	matrix.m[2][3] = 0.0f;
    matrix.m[3][0] = pos.x;			matrix.m[3][1] = pos.y;			matrix.m[3][2] = pos.z;			matrix.m[3][3] = 1.0f;
    return matrix;

}

mat4x4 Matrix_QuickInverse(mat4x4 &m) // Only for Rotation/Translation Matrices
{
    mat4x4 matrix;
    matrix.m[0][0] = m.m[0][0]; matrix.m[0][1] = m.m[1][0]; matrix.m[0][2] = m.m[2][0]; matrix.m[0][3] = 0.0f;
    matrix.m[1][0] = m.m[0][1]; matrix.m[1][1] = m.m[1][1]; matrix.m[1][2] = m.m[2][1]; matrix.m[1][3] = 0.0f;
    matrix.m[2][0] = m.m[0][2]; matrix.m[2][1] = m.m[1][2]; matrix.m[2][2] = m.m[2][2]; matrix.m[2][3] = 0.0f;
    matrix.m[3][0] = -(m.m[3][0] * matrix.m[0][0] + m.m[3][1] * matrix.m[1][0] + m.m[3][2] * matrix.m[2][0]);
    matrix.m[3][1] = -(m.m[3][0] * matrix.m[0][1] + m.m[3][1] * matrix.m[1][1] + m.m[3][2] * matrix.m[2][1]);
    matrix.m[3][2] = -(m.m[3][0] * matrix.m[0][2] + m.m[3][1] * matrix.m[1][2] + m.m[3][2] * matrix.m[2][2]);
    matrix.m[3][3] = 1.0f;
    return matrix;
}


int main()
{
    mesh meshCube;
    mat4x4 matProj;
    sf::Vector3f vCamera;
    sf::Vector3f vLookDir;
    float fYaw;
    float movementX;
    float movementY;
    int prevX;
    int prevY;
    bool isPanning = false;
    sf::Vector2i lastMousePosition;
    
    
    //load File
    meshCube.generateLine(2, 2);
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
    
    float mousePanSens = 0.2f;

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
        ImGui::Checkbox("Enable WireFrame", &checkboxState);
        ImGui::Checkbox("Enable Cartesian Plane", &cartesianState);

        ImGui::SliderFloat("Mouse Pan Sensitivity", &mousePanSens, 0.0f, 0.5f);
        ImGui::End();

        window.clear();

        
        mat4x4 matRotZ, matRotX;
        float deltaTime = elapsedTime.asSeconds();
        if(!ImGui::IsWindowHovered()){
            if(leftButtonPressed){
            if(!isPanning){
                isPanning = true;
                lastMousePosition = sf::Mouse::getPosition(window);
            }
            else{
                sf::Vector2i currentMousePosition = sf::Mouse::getPosition(window);
                sf::Vector2f delta = static_cast<sf::Vector2f>(currentMousePosition-lastMousePosition);
                vCamera.x -= delta.x * mousePanSens/10;
                vCamera.y -= delta.y * mousePanSens/10;
                lastMousePosition = currentMousePosition;
            }

            }
            else{
                isPanning = false;
            }
        }
        
        fTheta += rotationSpeed * deltaTime;
        matRotZ = Matrix_MakeRotationZ(fTheta * 0.5f);
		matRotX = Matrix_MakeRotationX(fTheta);

        mat4x4 matTrans;
		matTrans = Matrix_MakeTranslation(0.0f, 0.0f, 8.0f);

		mat4x4 matWorld;
		matWorld = Matrix_MakeIdentity();	// Form World Matrix
		matWorld = Matrix_MultiplyMatrix(matRotZ, matWorld); // Transform by rotation
		matWorld = Matrix_MultiplyMatrix(matWorld, matTrans); // Transform by translation
        vLookDir = {0,0,1};
        sf::Vector3f vUp(0,1,0);
        sf::Vector3f vTarget = vCamera + vLookDir;

        mat4x4 matCamera = Matrix_PointAt(vCamera, vTarget, vUp);

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

            sf::Vector3f vCameraRay = triTranslated.p[0]-vCamera;
            
            //if(normal.z < 0)//si el normal apunta hacia la camara ( NO ES MUY UTIL)
            if(normal.dot(vCameraRay) < 0.0f) //producto punto entre el normal y la proyeccion a la camara.
            { 
                //illumination
                sf::Vector3f light_direction = {0.0f, 0.0f, -1.0f};
                light_direction = light_direction.normalized();
                float dp = std::max(0.1f, light_direction.dot(normal));
                
                triProjected.r *= dp;
                triProjected.g *= dp;
                triProjected.b *= dp;

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