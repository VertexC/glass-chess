###Tricks
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
