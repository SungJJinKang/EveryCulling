TO-DO List

#### Skip Occlusion Culling when the number of visible objects is low            
Rasterizing occluder mesh is really expensive.         
If the number of visible objects is low at the time(Frustum culling already culled a lot of objects), it's better to skip occlusion culling.        

#### Support Mesh LOD           
Rasterizing occluder mesh is really expensive.           
You can lower the cost of rasterizing with lod.           

#### When a camera is in a mesh, ignore the mesh when rasterize occluder

#### When i profiled, it shows threads are waiting for a lot of time until other threads finished their job.           
This should be fixed up.       

#### Merge Occludee Bounding Box
When test bounding box of occludee aginst a depth buffer, the cost of the test can be decreased by merging bounding box of occuldee


#### Implement merging depth buffer method in masked sw occlusion culling
Currently, Binned rendering ( Dispatch(Bin) triangles to screen tiles based on triangle's screen space vertex data for following rasterizer stage. After that, Each thread rasterize triangles binned in the tiles ) is only supported.       
![merging is better](https://user-images.githubusercontent.com/33873804/222908420-db47806c-faf7-47fa-8b59-f1f6cd1bd046.png)
