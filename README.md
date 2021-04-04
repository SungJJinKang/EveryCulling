# Frostbite_CullingSystem

This library is implementation of Culling the Battlefield: Data Oriented Design in Practice Talk of EA DICE in 2011         
[Slide Resource](https://www.ea.com/frostbite/news/culling-the-battlefield-data-oriented-design-in-practice)      
[GDC Talk Video](https://www.gdcvault.com/play/1014491/Culling-the-Battlefield-Data-Oriented)        
[한국어 블로그 글](https://sungjjinkang.github.io/doom/2021/04/02/viewfrustumculling.html)

## Feature

- View Frustum Culling using SIMD
- Software Occlusion Culling
- Screen Space AABB Area Culling ( Project Entity's AABB bount to Screen Space, if Aread of Projected AABB is less than setting, Cull it )

## View Frustum Culling using SIMD, Multithreading
Transform Data of Entities is stored linearlly to maximize utilizing SIMD.        
**Data oriented Design!!!!!!**    
For calculating Object is In Frustum, Objects Position data is stored linearlly....    
This will increase cache hitting!!   

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

## Software Occlusion Culling

references : https://www.gdcvault.com/play/1017837/Why-Render-Hidden-Objects-Cull

## Screen Space AABB Area Culling

This is really easy to understand.    
When Area Size of Screen space projected AABB is less than setting value, It will be culled.

## Required dependency

[LightMath_Cpp](https://github.com/SungJJinKang/LightMath_Cpp)
