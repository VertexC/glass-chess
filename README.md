### Glass-Chess
This is a bonus part of homework assignment of CMPT361/SFU




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

