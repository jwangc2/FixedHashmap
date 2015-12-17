Author:       Joshua Wang
Last Updated: 10/16/2015
Contact:      jwangc2@gmail.com

----------------------------
         Content
----------------------------
  What is it?              ;
  How's it work?           ;
  How do I know it works?  ;
  Decisions & Rationale    ;

----------------------------
       What is it?
----------------------------
A fixed-size hashmap that uses only primitive types, associating string keys with arbitrary data object references.
Furthermore, many considerations (Decisions & Rationale) have been made to optimize algorithmic runtime and memory usage.

Ideal hashmaps function at a runtime of O(1), given absolutely no collisions. Due to the possibly, and more importantly
often, large ratio of key space to data space, collisions are bound to happen, lest the hash function be perfectly tailored 
to the two spaces (given this function exists in the first place). Collisions, where multiple keys map to the same entry in a 
hashmap, must be resolved. The common and accepted approach is to thus replace these entries with a bucket, a collection of
entries that all share the same mapping. Thus one must first map to a bucket, then search the bucket for the key. As expected,
this resolution of collisions thus increase the runtime of the hashmap, which is directly related to the collision count: the 
longer the average length of a bucket, the longer the access time.

----------------------------
      How's it work?
----------------------------
The hashmap contains a hashtable, which is the fixed-size data space (size determined on construction), implemented as an 
array. Due to the fixed-size property, a freelist is required to keep track of the "free" array entries that can be placed
into a bucket. This freelist is implemented as a stack (LIFO) and a singly-linked list, where only the head of this list is
modified (popped or pushed).

On the front-end, each key is hashed via the hash function to an index on the hashtable anchor, which is an array of pointers 
that point to the head of each bucket. Each bucket is subsequently implemented as a singly-linked list that is in turn very 
flexible, and specifically, not necessarily contiguous. This property makes the bucket well suited to the limited data space.

For more on why the hashmap has been implemented in these many different ways, please take a look at the next section.

----------------------------
  How do I know it works?
----------------------------
The only way you know how: via testing and confirming. And for you, there is also a program that helps to simplify the testing. 
For corroboration, the source code of both the testing program AND the hashmap itself are provided. Additionally, input files 
which pre-selected commands are provided. Again if you don't trust these, you are encouraged to personally write the test cases.

The code was nurtured in Ubuntu 14.04 LTS, using GCC 4.8.2, so the following commands are in this context:
[1] Compiling the test program
> make clean
> make

[2a] Running the test program (user input)
> ./testProgram

[2b] Running the test program (automated input, recorded output)
> ./testProgram < inputFile.txt > outputFile.txt

To write a test case input file:
1) Each line represents an input on the command line
2) The first line is always the desired size of the hashmap (max number of entries).
3) Follow this format:
  Use SET [KEY] [VALUE] to set a value in the hashmap, where [VALUE] is an <integer>.
  Use GET [KEY] to get a value in the hashmap.
  Use DEL [KEY] to delete a value in the hashmap.
  Use PRINT [FILENAME] to write the content of the hashmap to a file.
  Use HELP to review these instructions.
  Use END to end the program.
Take a look at any of the inputFile*.txt files for reference.

Should Linux not be your OS of choice, the source code is all provided, so just port it over to your favorite C++ IDE and
compile it from there.

----------------------------
 [D]ecisions & [R]ationale:
----------------------------
D: IMPLEMENTATION VIA C++.
R: I am currently in the process of learning C++ (8 weeks in), so applying it towards projects that I am interested in
   (such as this) further helps bolster my understanding of the language. C++ is also generally a good choice.

D: USAGE OF HASH ANCHOR TABLE.
R: Gives the option of multiple buckets (rather than one), which alleviates and distributes the impact of hash collisions.

D: IMPLEMENTATION OF BUCKETS VIA SINGLY-LINKED LISTS.
R: Linked lists are bigger than arrays (due to storing a pointer to the next link), but much more flexible because the 
   entries do not have to be contiguous or ordered. Additionally, insertion and removal of any of the links is trivial;
   this property also allows the size of the linked list to change freely. As for the singly-linked property, the hashmap
   only requires a forward search on the list, so a doubly-linked list would be larger and take up more memory.

D: IMPLEMENTATION OF LINKED LIST VIA NODE STRUCT (AS OPPOSED TO A CONTAINER CLASS).
R: This is in part again due to a desire to apply learned concepts (see above D: IMPLEMENTATION VIA C++), but also done for 
   control over the exact members (and thus size) of the struct. Keep it simple stupid: structs are the basic building blocks 
   of data structures in the C family.

D: IMPLEMENTATION OF HASH FUNCTION VIA NAIVE ALGORITHM: [BUCKET INDEX] = [FIRST LETTER OF KEY] mod [CAPACITY].
R: A fairly simple and intuitive hash function. Works semi-decently especially when the usage of pre-fixes are avoided;
   particularly bad in the context of how programmers tend to name their variables (which is often reflected in the keys).
   While admittedly NOT a component that has been fully optimized for runtime, the function is still very very fast.
   
D: IMPLEMENTATION OF CAPACITY (INTIALLY 20, SET TO HALF OF SIZE ... IF 20 > HALF, DEFINITELY MAINTAIN POSITIVE > 0 VALUE).
R: The other half of the naive implementation of the hash function. The initial number 20 was chosen based on the size of the 
   alphabet (26), assuming that a) the first letter is probably a letter and b) roughly 6 of the 26 letters are uncommon at
   the start. The half-size cap is in place to ensure that there are not too many unused buckets. Finally, it is obvious that
   the number of buckets must be greater than 0. From a few test runs, it seems to acceptably reduce collision count.

D: INITIALIZATION OF FREELIST AS ENTIRE HASHMAP, FROM TOP TO BOTTOM.
R: A freelist is absolutely required for the fixed-size hashmap, since the data is stored in a specifically allocated space. 
   This freelist keeps track of all the unused (but allocated) blocks available for overwriting, much like a heap. As for 
   the top to bottom nature of the freelist, it is simply for a) ease of implementation via a for loop and b) ease of
   debugging the hashmap.

D: KEEPING KEYS AS A <std::string> TYPE (AS OPPOSED TO <char *> OR <char[]>).
R: The string class comes in particularly handy with methods that allow for conversion to integer (hash function), as well 
   as the compare function (used in the main set(), get(), del() methods).

D: KEEPING DATA AS REFERENCES / POINTERS (AS OPPOSED TO COPIES).
R: Partly due to design constraint, but also a good practice because the data is implemented as a generic, which means that
   a copy of that class could be astronomically large.

D: IMPLEMENTATION OF "FIXED-SIZE" CONTAINER VIA AN ARRAY.
R: An array is simply a contiguously allocated data structure, allowing for a simple pointer type and simpler memory.
   Additionally, the contiguous property of the array allows one to calculate the index of an entry via pointer
   subtraction, which additionally helps with debugging (such as in the print() method of the class).

D: DYNAMIC ALLOCATION OF THE "FIXED-SIZE" CONTAINER FOR THE HASHMAP.
R: This particular decision is implemented for flexibility: it works when the desired size of the hashmap is not known 
   at compile time, and it definitely works even when the desired size IS known at compile time.

D: MANAGEMENT OF DYNAMIC MEM VIA MANUAL new AND delete CALLS (AS OPPOSED TO A GARBAGE COLLECTION FRAMEWORK).
R: The hashmap contains only two dynamically allocated arrays, whereas the dynamic allocation of data in the test
   program also allows for further testing of the del() and/or get() methods of the hashmap.

D: NON-NULLING OF DATA POINTER UPON DELETION.
R: Nulling the data pointer inside the hashmap entry is unnecessary, for the entry itself is thrown on the freelist. 
   While the pointer is still there, it is considered recycled "garbage", and thus should be treated as such - unknown
   and unusable data. Why clean what nobody is supposed to look at? This brings up a point about the print() method, 
   where it is shown that a pointer may actually point to random places in memory once the actual data object is deleted.

D: FUNCTIONS get() AND del() RETURN THE POINTER (AS OPPOSED TO THE VALUE).
R: The set() method takes a reference to the data as an argument, so the return of the get() and del() functions simply 
   keep a consistent implementation by also returning the reference. Additionally, returning a reference is much faster 
   and less memory-intensive than a copy.

D: ON del(), INSERTING THE "DELETED" ENTRY BEFORE THE HEAD OF THE FREELIST (AS OPPOSED TO ELSEWHERE, LIKE THE TAIL).
R: Simply put, the hashmap keeps a reference to the head of the freelist at all times, so simply inserting the deleted 
   entry at the front (making it the new head of the freelist) is the simplest and the fastest.

D: DEFINITION OF HASHMAP CLASS ENTIRELY WITHIN THE HEADER (.h) FILE.
R: Because the hashmap utilizes generics via template definitions, if other classes wish to have access to the implementation, 
   they must see the full implementation of these template definitions. A few other options are available, but this seems the 
   most intuitive and generally accepted solution.

D: ADDITION OF PRIVATE DATA MEMBER [numItems].
R: While it takes up a little more memory to keep track of the number of items currently in the hashmap, it significantly reduces 
   the calculation that would otherwise be required of the load() method: a linear search through the entire hashtable.

D: DECLARATION OF THE PRIVATE DATA MEMBER [mSize] IS NON-CONSTANT (DESPITE THE PHYSICAL SIZE BEING "FIXED").
R: Due to the dynamic nature of this hashmap's implementation, the size is not defined until instantiation. Rest assured, that is 
   the only time that the [size] data member will change during the lifetime of the hashmap.
