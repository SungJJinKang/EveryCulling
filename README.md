# LinearData_ViewFrustumCulling

This library is implementation of Culling the Battlefield: Data Oriented Design in Practice Talk of EA DICE in 2011         
[Slide Resource](https://www.ea.com/frostbite/news/culling-the-battlefield-data-oriented-design-in-practice)      
[GDC Talk Video](https://www.gdcvault.com/play/1014491/Culling-the-Battlefield-Data-Oriented)   


## Feature

**Transform Data of Entities is stored linearlly to maximize utilizing SIMD**.     
**Solve frustum intersection parallelly in multiple threads.** 

This way don't use Acceleration structure like BVH, KDTree.      
     

## Required dependency

[LightMath_Cpp](https://github.com/SungJJinKang/LightMath_Cpp)
