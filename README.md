# LinearData_ViewFrustumCulling

This library is implementation of Culling the Battlefield: Data Oriented Design in Practice Talk of EA DICE in 2011         
[Slide Resource](https://www.ea.com/frostbite/news/culling-the-battlefield-data-oriented-design-in-practice)      
[GDC Talk Video](https://www.gdcvault.com/play/1014491/Culling-the-Battlefield-Data-Oriented)        
[한국어 블로그 글](https://sungjjinkang.github.io/doom/2021/04/02/viewfrustumculling.html)

## Feature

- View Frustum Culling using SIMD
- Software Occlusion Culling
- Screen Space AABB Area Culling ( Project Entity's AABB bount to Screen Space, if Aread of Projected AABB is less than setting, Cull it )

## Why this system is fast
Transform Data of Entities is stored linearlly to maximize utilizing SIMD.
Data oriented Design!!!!!!    
For calculating Object is In Frustum, Objects Position data is stored linearlly....    
This will increase cache hitting!!   
like this   
```c++
float objectData[] = {Entity1-PosX, Entity1-PosY, Entity1-PosZ, Entity1-SphereRadius(Bound Sphere), Entity2-PosX, Entity2-PosY, Entity2-PosZ, Entity2-SphereRadius,
, Entity3-PosX, Entity3-PosY, Entity3-PosZ, Entity3-SphereRadius, Entity4-PosX, Entity4-PosY, Entity4-PosZ, Entity4-SphereRadius}
```

And Frustun checking use SIMD instruction.   

Combine upper two feature!.    
```c++
Frustum Plane1-PosX      Frustum Plane2-PosX      Frustum Plane3-PosX      Frustum Plane4-PosX
Frustum Plane1-PosY      Frustum Plane2-PosY      Frustum Plane3-PosY      Frustum Plane4-PosY
Frustum Plane1-PosZ      Frustum Plane2-PosZ      Frustum Plane3-PosZ      Frustum Plane4-PosZ
Frustum Plane1-Dist      Frustum Plane2-Dist      Frustum Plane3-Dist      Frustum Plane4-Dist

   Ojbect PointX           Ojbect PointX            Ojbect PointX            Ojbect PointX
   Ojbect PointY           Ojbect PointY            Ojbect PointY            Ojbect PointY
   Ojbect PointZ           Ojbect PointZ            Ojbect PointZ            Ojbect PointZ
   Ojbect Radius           Ojbect Radius            Ojbect Radius            Ojbect Radius
   
                                      |
                                      |
                                      V
   
SIMD MultiPly Each Row Computation And Add All Row --> Dot Result of Each Plane and Object
   
```

**Solve frustum intersection parallelly in multiple threads.** 

This way don't use Acceleration structure like BVH, KDTree.      
     

## Required dependency

[LightMath_Cpp](https://github.com/SungJJinKang/LightMath_Cpp)
