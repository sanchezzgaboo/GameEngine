#include "Renderer.h"

Renderer::Renderer(sf::RenderWindow& window){
    // Set the frame rate limit to 60 FPS
    //window.setFramerateLimit(60);
}

void Renderer::beginFrame(){
    window.clear();
}

void Renderer::renderScene(const Scene& scene, const Camera& camera, const mat4x4& projMat)
{
    mat4x4 matProj = Matrix_MakeProjection(90.0f, (float)screenheight / (float)screenwidth, 0.1f, 1000.0f);

    mat4x4 matTrans;
    matTrans = Matrix_MakeTranslation(0.0f, 0.0f, 8.0f);

    mat4x4 matWorld;
    matWorld = Matrix_MakeIdentity();	// Form World Matrix
	matWorld = Matrix_MultiplyMatrix(matWorld, matTrans); // Transform by translation

    for (auto& obj : scene.objects)
    {
        for (auto& tri : obj.mesh->tris)
        {
            
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

            sf::Vector3f vCameraRay = triTranslated.p[0]-camera.vCamera;
            
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
}

void Renderer::endFrame(){
    window.display();
}
