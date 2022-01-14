# EveryCulling

This library integrate multiple culling system into One library.      

This System contain **ViewFrustumCulling**, **Masked SW Occlusion Culling**, **Distance Culling**            
Most of Systems in this library is actually used in the commercial game engines.       

This project tries to integrate them into one system and make them easy to use.  

- [Core Feature](https://github.com/SungJJinKang/EveryCulling#core-feature)
- [Fully implemented features List](https://github.com/SungJJinKang/EveryCulling#fully-implemented-features)
- [View Frustum Culling from Frostbite Engine of EA Dice](https://github.com/SungJJinKang/EveryCulling#view-frustum-culling-from-frostbite-engine-of-ea-dice--100-)
- [Masked SW ( CPU ) Occlusion Culling From Intel](https://github.com/SungJJinKang/EveryCulling#masked-sw--cpu--occlusion-culling-from-intel--100-)
- [Distance Culling From Unreal Engine](https://github.com/SungJJinKang/EveryCulling#distance-culling-from-unreal-engine--100-)
- [Code Example](https://github.com/SungJJinKang/EveryCulling#code-example)

## Core Feature 
This library is targeting Maximizing **SIMD, Cache hit, Multi Threading.**                 
1. SIMD : Data is stored for using SIMD Intrinsics ( Object's Datas has SoA layout, check [EntityBlock.h](https://github.com/SungJJinKang/EveryCulling/blob/main/DataType/EntityBlock.h) ) ( Require AVX2 ( _mm256 ) )                       
2. Cache Hit : SoA!! ( Structure of Arrays, check [EntityBlock.h](https://github.com/SungJJinKang/EveryCulling/blob/main/DataType/EntityBlock.h) )           
3. Multi Threading : Data of entities is separately stored in entity block, Then Threads works on a entity block. These structure prevent data race and cache coherency ( false sharing, check [EntityBlock.h](https://github.com/SungJJinKang/EveryCulling/blob/main/DataType/EntityBlock.h), [VertexData.h](https://github.com/SungJJinKang/EveryCulling/blob/main/DataType/VertexData.h) ), Locking is not required.               
                                 
               
## Fully implemented features
- View Frustum Culling from Frostbite Engine of EA Dice ( [video](https://youtu.be/G-IFukD2bNg) )    
- Masked SW ( CPU ) Occlusion Culling from Intel ( [video 1](https://youtu.be/tMgokVljvAY), [video 2](https://youtu.be/1IKTXsSLJ5g), [reference paper](https://software.intel.com/content/dam/develop/external/us/en/documents/masked-software-occlusion-culling.pdf) )                                       
- Distance Culling from Unreal Engine ( [reference](https://docs.unrealengine.com/en-US/RenderingAndGraphics/VisibilityCulling/CullDistanceVolume/index.html) )       

Procedure : **Distance Culling -> View Frustum Culing -> Masked SW Occlusion Culling**              
                            
## View Frustum Culling from Frostbite Engine of EA Dice ( 100% )
         
[Source code](https://github.com/SungJJinKang/EveryCulling/tree/main/CullingModule/ViewFrustumCulling)          
            
[Video](https://youtu.be/G-IFukD2bNg)         
[Slide Resource](https://www.ea.com/frostbite/news/culling-the-battlefield-data-oriented-design-in-practice)        
[GDC Talk Video](https://www.gdcvault.com/play/1014491/Culling-the-Battlefield-Data-Oriented)   
[한국어 블로그 글](https://sungjjinkang.github.io/doom/c++/computergraphics/game/2021/04/02/viewfrustumculling.html)    

MultiThreaded View Frustum Culling is 8ms faster than SingleThreaded View Frustum Culling ( in Stress Test )

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

**Because Each Entity Blocks is seperated, Threads can check whether object is culled without data race.** 

To minimize waiting time(wait calculating cull finish) , Passing cull job to thread should be placed at foremost of rendering loop.      
In My experiment, Waiting time is near to zero.

## Masked SW ( CPU ) Occlusion Culling From Intel ( 100% )             
             
[Source Code](https://github.com/SungJJinKang/EveryCulling/tree/main/CullingModule/MaskedSWOcclusionCulling)         

Stage 1 : Solve Mesh Role Stage ( Choose occluder based on object's screen space aabb area )             
Stage 2 : Bin Occluder Triangle Stage ( Dispatch(Bin) triangles to screen tiles based on triangle's screen space vertex data for following rasterizer stage. This is for not using lock when multiple threads rasterize triangles on depth buffer )               
Stage 3 : Multithread Rasterize Occluder Triangles ( Threads do job rasterizing each tile's binned triangles, calculate max depth value of tile )             
Stage 4 : Multithread Query depth buffer ( Compare aabb of occludee's min depth value with tile depth buffer. check 52p https://www.ea.com/frostbite/news/culling-the-battlefield-data-oriented-design-in-practice )              

[Reference paper](https://software.intel.com/content/dam/develop/external/us/en/documents/masked-software-occlusion-culling.pdf)             
[개발 일지](https://sungjjinkang.github.io/computerscience/computergraphics/2021/12/31/masked_sw_occlusion_culling.html)                
[Video1](https://youtu.be/tMgokVljvAY), [Video2](https://youtu.be/1IKTXsSLJ5g)                       
동작 원리 한국어 설명 : ["Masked Software Occlusion Culling"는 어떻게 작동하는가?](https://github.com/SungJJinKang/EveryCulling/blob/main/CullingModule/MaskedSWOcclusionCulling/MaskedSWOcclusionCulling_HowWorks.md)                 
References : [references 1](https://software.intel.com/content/dam/develop/external/us/en/documents/masked-software-occlusion-culling.pdf), [references 2](https://www.slideshare.net/IntelSoftware/masked-software-occlusion-culling), [references 3](https://www.slideshare.net/IntelSoftware/masked-occlusion-culling)                
                

Please read this :        
SW Occlusion Culling doesn't make always good performance. It can makes rendering slower than not using it. It depends on scene.           
If there is many occludees, it's good to use it. If not, It's better not to use it because Cost of rasterizing occluder can be more expensive than cost of rendering occludees. It decrease maximum fps but increase minimum fps. I think it's good trade-off.       

As you know, CPU isn't good at this computation. So this algorithm should be used carefully              

This algorithm is used in [MS Flight Simulator 2020](https://medium.com/intel-tech/above-and-beyond-intels-leading-performance-in-microsoft-flight-simulator-2020-6977516d27f8).                       

## Distance Culling From Unreal Engine ( 100% )
         
Code directory : [https://github.com/SungJJinKang/EveryCulling/tree/main/CullingModule/DistanceCulling](https://github.com/SungJJinKang/EveryCulling/tree/main/CullingModule/DistanceCulling)         
            
This feature is referenced from Unreal Engine.        
You can see How this feature works from [here](https://docs.unrealengine.com/en-US/RenderingAndGraphics/VisibilityCulling/CullDistanceVolume/index.html)       
Objects's visibility is decided based on distance between object and camera. ( Distance is computed using SIMD for performance )                  
With this feature, You can make tiny object not to be rendered when it is far from camera.      
If Object's DesiredMaxDrawDistance is larger than distance between object and camera, The object is culled.             


## Code Example

1. Initialize CullingSystem
```
EveryCulling::SetThreadCount( Thread Count to do cull job );
EveryCulling::mMaskedSWOcclusionCulling->mSolveMeshRoleStage.mOccluderAABBScreenSpaceMinArea = Screen Space AABB Minimum Area in Masked SW Occlusion Culling ( Used for deciding occluders of entities )
```
                
2. Initialize entity data for culling                 
```
culling::EntityBlockViewer entityBlockViewer = EveryCulling::AllocateNewEntity();
entityBlockViewer.SetMeshVertexData // Used in Masked SW Occlusion Culling
(
    Vertex Data,
    Vertice Count,
    MeshIndices Data,
    MeshIndices Count ,
    Vertex Stride
);
entityBlockViewer.SetDesiredMaxDrawDistance(mDesiredMaxDrawDistance); // Used in Distance Culling
```            
             
                
3. Update datas for cull job ( Should be done every frame )            
```
Update Camera Data

culling::EveryCulling::GlobalDataForCullJob cullingSettingParameters;
std::memcpy(cullingSettingParameters.mViewProjectionMatrix.data(), Camera ViewProjection Matrix ( Mat4x4, Column major ), sizeof(culling::Mat4x4));
cullingSettingParameters.mFieldOfViewInDegree = Camera FOV;
cullingSettingParameters.mCameraFarPlaneDistance = Camera ClippingPlaneFar;
cullingSettingParameters.mCameraNearPlaneDistance = Camera ClippingPlaneNear;
std::memcpy(cullingSettingParameters.mCameraWorldPosition.data(), Camera World Position Data, sizeof(culling::Vec3));                  
std::memcpy(cullingSettingParameters.mCameraRotation.data(), Camera Rotation Data ( Quaternion ), sizeof(culling::Vec4));                  

EveryCulling::UpdateGlobalDataForCullJob(camera->CameraIndexInCullingSystem, cullingSettingParameters);            
EveryCulling::PreCullJob();             

------------------
Update Entity Data

for(entity : EntityList)
{
    entity.EntityBlockViewer.UpdateEntityData
    (
        Entity World Position Data,
        Entity AABB LowerBound World Position Data,
        Entity AABB UpperBound World Position Data,
        Entity Model Matrix Data ( Mat4x4 )
    );
}
```                
                
                   
3. Update Front to Back Sorting Data of entities ( Optional. Important for Peforamance of Masked SW Occlusion Culling )  ( Should be updated every frame )              
```
int entityOrder = 0;
for(entity : Front to Back Sorted Entity List)
{
    EveryCulling.SetSortedEntityInfo
    (
        CameraIndex,
        entityOrder,
        entity.EntityBlockViewer
    );

    entityOrder++;
}
```

4. Pass Culling Job to SubThreads
```
Your Job System.PassJobToThread(EveryCulling::GetThreadCullJobInLambda(CameraIndex));        
```

## References

- https://www.ea.com/frostbite/news/culling-the-battlefield-data-oriented-design-in-practice
- https://www.gdcvault.com/play/1014491/Culling-the-Battlefield-Data-Oriented
- https://software.intel.com/content/dam/develop/external/us/en/documents/masked-software-occlusion-culling.pdf
- http://www.sunshine2k.de/articles/Derivation_DotProduct_R2.pdf
- http://www.sunshine2k.de/articles/Notes_PerpDotProduct_R2.pdf
- http://www.sunshine2k.de/coding/java/PointInTriangle/PointInTriangle.html#cramer
