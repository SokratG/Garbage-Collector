# Garbage-Collector

Project Garbage Collector and Memory Leak Detector(MLD) Data Structure.

<ol>
    <li>Garbage Collector based on counting reference(RAII) pointer. Implementation avoid cycle reference and collect data in                     background thread. Used ISO C++17.</li>
    <li>Memory Leak Detector based on Object Data Base(Disjoint Set Graph). Register structure type(field) in structure data base,                 register object in data base(list of structure), allocate objects - using custom xcalloc(). Algorithm traverse on graph                   for reach all object, and print information about leak memory, each which not free(custom xfree()) in application. Used                   ISO C11.</li> 
</ol>
# For build project:<br> 
<p>cd MemoryLeakDetector</p>
<p>mkdir build</p>
<p>cd build</p>
<p>cmake ..</p>
<p>make</p>
