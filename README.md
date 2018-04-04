### Glass-Chess
This is a bonus part of homework assignment of CMPT361/SFU

#### Technique Implemented
- SMF reader
- parallel ray tracing via multiple thread
- bounding volume hierarchy with AABB(Axis-Aligned minimum bounding box)

#### Build Guide
```bash
make
./raycast step_max <options>
```
- step_max: max depth of ray tracing
- options:
    + +p: turn on parallel ray tracing
    + +b: turn on bounding volume hierarchy


#### Result & ScreenShot

```bash
bowenc@bowenc-MS-7A59:~/cmpt361/glass-chess$ ./raycast 6 +b +p
Num of intersection: 3924721
Ray Trace done in 0.151097 s
```
<div>
<img src="screenshot/6_bp.png" width="60%">
</div>

```bash
bowenc@bowenc-MS-7A59:~/cmpt361/glass-chess$ ./raycast 6 +p
Num of intersection: 641162400
Ray Trace done in 5.058907 s
```
<div>
<img src="screenshot/6_p.png" width="60%">
</div>

```bash
bowenc@bowenc-MS-7A59:~/cmpt361/glass-chess$ ./raycast 1 +b
Num of intersection: 676182
Ray Trace done in 0.133134 s
```
<div>
<img src="screenshot/1_b.png" width="60%">
</div>

#### Implementation Detail
##### OOP
It turns out that the origin skeleton code is messed up with global and extern variable. It would be more elegant if we reorganized everything into class and treat tracer, triangle, plane and others as object.
##### Parallel Ray Tracing
The main idea of it is to assign different region of image for each tracer, and let tracers do ray tracing in thread independently, where each tracer only casts ray through pixels inside its assigned region. The idea to wrap tracer suff into a class is really beneficial here.
##### Bounding Volume Hierachy with 
The basic idead of BVH is to split the space recursively and wrap the objects into a bounding box. When it comes to intersection, we can intersect with bounding box first and then triangles inside.

#### Reference




#### Tricks

##### ptr allocatation inside function
For ptr of C++, in the following case
```c++
void A(){
    int * p;
    init(p)
}
void init(int * p){
    p = new int[100];
}
```
The p in A() will not have access to the newed array. As ptr p is copied to init(), so its value(adress of space it points to) will not change.
The righy way is to use reference of ptr.
```c++
void A(){
    int * p;
    init(p)
}
void init(int * &p){
    p = new int[100];
}
```

##### glm
When deal with glm, it's better to declare value ends in *.*f, otherwise there could be error like
```bash
note: ‘glm::vec3 {aka glm::tvec3<float, (glm::precision)0u>}’ is not derived from ‘const glm::tmat4x4<T, P>’
```
(relevant reference on stackoverflow)[https://stackoverflow.com/questions/12085109/cant-get-glmrotate-to-work]

