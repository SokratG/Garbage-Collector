# Garbage-Collector

### Project Garbage Collector and Memory Leak Detector(MLD) Data Structure.

<ol>
    <li>Garbage Collector based on counting reference(RAII) pointer. Implementation avoid cycle reference and collect data in                     background thread. Project is header only and used ISO C++17.</li>
    <li>Memory Leak Detector based on Object Data Base(Disjoint Set Graph). Register structure type(field) in structure data base,                 register object in data base(list of structure), allocate objects - using custom xcalloc(). Algorithm traverse on graph                   for reach all object, and print information about leak memory, each which not free(custom xfree()) in application.Project used             ISO C11.</li> 
</ol>

**For build project:**
```
> cd MemoryLeakDetector
 
> mkdir build
 
> cd build   
 
> cmake ..  
 
> make  
```

