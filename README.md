# EveryCulling

This library integrate Multiple Culling System into One System.      

This System contain **Multithread ViewFrustumCulling**, **Masked SW Occlusion Culling**, **Distance Culling**, **Screen Space AABB Culling**.         
Most of Systems in this project is actually used in popular game engines.       

This project tries to integrate them into one system and make them easy to use.      

## Core Feature 
This library is targeting Maximing SIMD, Cache hit, Multi Threading.       
1. SIMD : Data is stored for using SIMD Intrinsics
2. Cache Hit : SoA!! ( Structure of Arrays )
3. Multi Threading : Data of entities is separately stored in entity block, Then Threads works on a entity block. These structure prevent data race. Don't need locking.

## Feature

#### Currently Supported
- View Frustum Culling using SIMD
- Screen Space AABB Area Culling ( Project Entity's AABB bount to Screen Space, if Aread of Projected AABB is less than setting, Cull it )
- Support AVX2

#### In Develop
- Masked SW Occlusion Culling ( https://software.intel.com/content/dam/develop/external/us/en/documents/masked-software-occlusion-culling.pdf )          
- Distance Culling ( https://docs.unrealengine.com/en-US/RenderingAndGraphics/VisibilityCulling/CullDistanceVolume/index.html )  
- Precomputed Visibility Volume ( https://docs.unrealengine.com/en-US/RenderingAndGraphics/VisibilityCulling/PrecomputedVisibilityVolume/index.html )
- Support AVX1, AVX512

## View Frustum Culling using SIMD, Multithreading

[Slide Resource](https://www.ea.com/frostbite/news/culling-the-battlefield-data-oriented-design-in-practice)        
[GDC Talk Video](https://www.gdcvault.com/play/1014491/Culling-the-Battlefield-Data-Oriented)   
[한국어 블로그 글](https://sungjjinkang.github.io/doom/2021/04/02/viewfrustumculling.html)    

#### Feature 1 : Transform Data of Entities is stored linearlly to maximize utilizing SIMD. ( **Data oriented Design** )       
For Maximizing Cache Hitting, Data is allocated adjacently.     

Ordinary Way
```c++
float objectData[] = { FrustumPlane1-NormalX, FrustumPlane1-NormalY, FrustumPlane1-NormalZ, FrustumPlane1-Distance, FrustumPlane2-NormalX, FrustumPlane2-NormalY, FrustumPlane2-NormalZ, FrustumPlane2-Distance, FrustumPlane3-NormalX, FrustumPlane3-NormalY, FrustumPlane3-NormalZ, FrustumPlane3-Distance, FrustumPlane4-NormalX, FrustumPlane4-NormalY, FrustumPlane4-NormalZ, FrustumPlane4-Distance }
```

Data oriented Design
```c++
float objectData[] = { FrustumPlane1-NormalX, FrustumPlane2-NormalX, FrustumPlane3-NormalX, FrustumPlane4-NormalX, FrustumPlane1-NormalY, FrustumPlane2-NormalY, FrustumPlane3-NormalY, FrustumPlane4-NormalY, FrustumPlane1-NormalZ, FrustumPlane2-NormalZ, FrustumPlane3-NormalZ, FrustumPlane4-NormalZ, FrustumPlane1-Distance, FrustumPlane2-Distance, FrustumPlane3-Distance, FrustumPlane4-Distance }
```

And Frustun checking use SIMD instruction.   

Combine upper two feature!.    
```c++
Plane1 NormalX    Plane2 NormalX    Plane3 NormalX    Plane4 NormalX     
Plane1 NormalY    Plane2 NormalY    Plane3 NormalY    Plane4 NormalY     
Plane1 NormalZ    Plane2 NormalZ    Plane3 NormalZ    Plane4 NormalZ     
Plane1 Distance   Plane2 Distance   Plane3 Distance   Plane4 Distance     

Ojbect PointX     Ojbect PointX     Ojbect PointX      Ojbect PointX
Ojbect PointY     Ojbect PointY     Ojbect PointY      Ojbect PointY
Ojbect PointZ     Ojbect PointZ     Ojbect PointZ      Ojbect PointZ
      1                 1                 1                  1
      
      
Plane5 NormalX    Plane6 NormalX    Plane5 NormalX    Plane6 NormalX     
Plane5 NormalY    Plane6 NormalY    Plane5 NormalY    Plane6 NormalY     
Plane5 NormalZ    Plane6 NormalZ    Plane5 NormalZ    Plane6 NormalZ     
Plane5 Distance   Plane6 Distance   Plane5 Distance   Plane6 Distance     

Ojbect PointX     Ojbect PointX     Ojbect PointX      Ojbect PointX
Ojbect PointY     Ojbect PointY     Ojbect PointY      Ojbect PointY
Ojbect PointZ     Ojbect PointZ     Ojbect PointZ      Ojbect PointZ
      1                 1                 1                  1

                            |
                            | Multiply each row
                            V
                                    
Plane1 NormalX * Ojbect PointX    Plane2 NormalX * Ojbect PointX    Plane3 NormalX * Ojbect PointX    Plane4 NormalX * Ojbect PointX     
Plane1 NormalY * Ojbect PointY    Plane2 NormalY * Ojbect PointY    Plane3 NormalY * Ojbect PointY    Plane4 NormalY * Ojbect PointY
Plane1 NormalZ * Ojbect PointZ    Plane2 NormalZ * Ojbect PointZ    Plane3 NormalZ * Ojbect PointZ    Plane4 NormalZ * Ojbect PointZ
     Plane1 Distance * 1               Plane2 Distance * 1               Plane3 Distance * 1               Plane4 Distance * 1   
     
Plane5 NormalX * Ojbect PointX    Plane6 NormalX * Ojbect PointX    Plane5 NormalX * Ojbect PointX    Plane6 NormalX * Ojbect PointX     
Plane5 NormalY * Ojbect PointY    Plane6 NormalY * Ojbect PointY    Plane5 NormalY * Ojbect PointY    Plane6 NormalY * Ojbect PointY
Plane5 NormalZ * Ojbect PointZ    Plane6 NormalZ * Ojbect PointZ    Plane5 NormalZ * Ojbect PointZ    Plane6 NormalZ * Ojbect PointZ
     Plane1 Distance * 1               Plane2 Distance * 1               Plane3 Distance * 1               Plane4 Distance * 1   

                            |
                            | Sum all row
                            V
                        
Plane1 Dot Object      Plane2 Dot Object      Plane3 Dot Object      Plane4 Dot Object
Plane5 Dot Object      Plane6 Dot Object      Plane5 Dot Object      Plane6 Dot Object

                            |
                            |   If Dot is larget than 0, Set 1
                            V
   1      0      1      1                                                1      1      1      1                                                                  
   1      1      1      1                                                1      1      1      1
                                                                         
             |                                                 or                  |
             |   To be rendered, All value should be 1                             |   To be rendered, All value should be 1
             V                                                                     V
                                                                         
         Culled!!!!!                                                            Rendered
```


#### Feature 2 : Entities is divided to Entity Blocks.        
```
Entity Block 1 : Entity 1 ~ Entity 50 
Entity Block 2 : Entity 51 ~ Entity 100 
Entity Block 3 : Entity 101 ~ Entity 150
      
               |
               |
               V
             
Thread 1 : Check Frustum of Entity Block 1, 4, 7
Thread 2 : Check Frustum of Entity Block 2, 5, 8
```

**Because Each Entity Blocks is seperated, Can Check Is Culled on multiple threads without data race.** 

To minimize waiting time(wait calculating cull finish) , Passing cull job to thread should be placed at foremost of rendering loop.      
In My experiment, Waiting time is near to zero.

## Masked Software Occlusion Culling

references : https://software.intel.com/content/dam/develop/external/us/en/documents/masked-software-occlusion-culling.pdf

## Query Occlusion Culling

Currently This feature is supported only on OpenGL.       

How Work? :       
1. Draw Occluder.        
2. Draw AABB of Complicated Occludeee mesh. ( AABB is much cheaper than complicated mesh )           
3. If Any fragment of AABB is drawed on buffer ( aabb passed depth, stencil test..! ), Draw Complicated Mesh!!!               

In Opengl : Use QueryObject, Conditional Rendering         

references : https://www.khronos.org/registry/OpenGL/extensions/ARB/ARB_occlusion_query.txt , https://www.khronos.org/registry/OpenGL/extensions/NV/NV_conditional_render.txt       

## Screen Space AABB Area Culling

This is really easy to understand.    
When Area Size of AABB projected to screen space is less than setting value, It will be culled.    
I don't recommend using this feature, because The objects pops up and this is really distracting...       
I will add Object fade out feature.    

## Distance Culling

This feature is referenced from Unreal Engine.     
You can see How this feature works from [here](https://docs.unrealengine.com/en-US/RenderingAndGraphics/VisibilityCulling/CullDistanceVolume/index.html)       
Objects become invisible depending on distance between object and camera.          
With this feature, You can make detailed object not to be rendered when it is far from camera.    
According your setting, Objects do popping.    



## References

- https://www.ea.com/frostbite/news/culling-the-battlefield-data-oriented-design-in-practice
- https://www.gdcvault.com/play/1014491/Culling-the-Battlefield-Data-Oriented
- https://software.intel.com/content/dam/develop/external/us/en/documents/masked-software-occlusion-culling.pdf
- http://www.sunshine2k.de/articles/Derivation_DotProduct_R2.pdf
- http://www.sunshine2k.de/articles/Notes_PerpDotProduct_R2.pdf
- http://www.sunshine2k.de/coding/java/PointInTriangle/PointInTriangle.html#cramer
