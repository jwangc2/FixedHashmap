/******************************
 * Author: Joshua Wang
 * Date: 10/16/2015
 * Contact: jwangc2@gmail.com
 */

#include <iostream>
#include <fstream>
#include <string>

// A generic type for our hashtable
template<typename T>
struct BucketNode {
    BucketNode *next = NULL;
    std::string key;
    T *value = NULL;
};

template<typename T>
class FixedHashmap
{
    public:
        // Constructors
        FixedHashmap(int size)
        {
            mSize = size;

            // Init capacity to be roughly the size of the alphabet (given the hash function)
            mCapacity = 20;

            // If capacity is too large, tone it down
            if (mCapacity > mSize / 2)
                mCapacity = (mSize / 2);

            // ...and if too small, ensure a legal capacity
            if (mCapacity <= 0)
                mCapacity = 1;

            // Dynamic alloc of the underlying data structures
            hashAnchor = new BucketNode<T> *[mCapacity];
            hashTable = new BucketNode<T>[mSize];

            numItems = 0;

            // Initialize the anchor table to have each anchor point to null (empty bucket)
            for (int i = 0; i < mCapacity; i ++)
                hashAnchor[i] = NULL;
            
            // Initialize the actual table to be a giant free list (top-down)
            for (int i = 1; i < mSize; i ++)
                hashTable[i - 1].next = &hashTable[i];
            
            hashTable[mSize - 1].next = NULL;               // Really make sure that the free list DOES terminate
            freePtr = &hashTable[0];                        // Head points to the first element of the table
        }

        ~FixedHashmap()
        {
            // Clean up your mess
            delete[] hashTable;
            delete[] hashAnchor;
        }

        // Public Member Functions
        bool set(const std::string &key, T *value)
        {
            // Find the head that this bucket points to
            int bucketID = hashit(key);

            BucketNode<T> *currNode = hashAnchor[bucketID];     // Iterator node
            BucketNode<T> *lastNode = hashAnchor[bucketID];     // Keeps track of the last valid node (non-NULL)

            // Go down the rabbit hole and try to find an existing node for this key
            while (currNode != NULL)
            {
                // Check for existing entries, then edit it
                if ((currNode->key).compare(key) == 0)
                {
                    currNode->value = value;
                    return true;
                }

                // Look for the next one
                lastNode = currNode;
                currNode = currNode->next;
            }

            // No match? Use a free node then...
            if (freePtr != NULL)
            {
                // Take the nearest node (at free pointer), and move the free pointer to the next node in the free list (pop off)
                currNode = freePtr;
                freePtr = freePtr->next;

                // Now fill the taken node with value
                currNode->key = key;
                currNode->value = value;
                currNode->next = NULL;

                // ...not forgetting to point the previous item in the bucket to this node
                if (lastNode == NULL)
                    hashAnchor[bucketID] = currNode;
                else
                    lastNode->next = currNode;

                numItems++;
            
                return true;
            }
            else
            {
                // Well dadgum!! You managed to use up the WHOLE hashtable?!
                return false;
            }
        }

        T * get(const std::string &key)
        {
            // Find the head that this bucket points to
            int bucketID = hashit(key);
            BucketNode<T> *currNode = hashAnchor[bucketID];

            // Go down the rabbit hole and try to find an existing node for this key
            while (currNode != NULL)
            {
                // Found it!
                if ((currNode->key).compare(key) == 0)
                    return currNode->value;

                // Look for the next one
                currNode = currNode->next;
            }

            // It's not even there tho
            return NULL;
        }

        T * del(const std::string &key)
        {
            // Find the head that this bucket points to
            int bucketID = hashit(key);
            BucketNode<T> *currNode = hashAnchor[bucketID];
            BucketNode<T> *prevNode = NULL;

            // Go down the rabbit hole and try to find an existing node for this key
            while (currNode != NULL)
            {
                // Found it!
                if ((currNode->key).compare(key) == 0)
                {
                    // Skip over the current node (aka, removing it from the bucket)
                    if (prevNode != NULL)
                        prevNode->next = currNode->next;
                    else
                        hashAnchor[bucketID] = currNode->next;

                    // Insert before head of free list for recycling (push on)!
                    currNode->next = freePtr;
                    freePtr = currNode;

                    numItems--;
                    
                    // Still helps to know what the value was (kinda like a stack pop)
                    return currNode->value;
                }

                // Look for the next one
                prevNode = currNode;
                currNode = currNode->next;
            }

            // It's not even there tho
            return NULL;
        }

        float load()
        {
            // Load is simply a ratio of how full the hashmap is
            return (float)numItems / mSize;
        }    

        void print(const char *fname)
        {
            // Open file for writing
            std::ofstream of(fname);

            // Header value
            of << "HTS_CAPACITY: " << mCapacity << std::endl;
            of << "HTS_SIZE: " << mSize << std::endl;
            of << "LOAD: " << load() << std::endl;

            of << "Free Pointer -> ";
            if (freePtr != NULL)
                of << "HT[" << (freePtr - hashTable) << "]"; // Pointer sub gives index due to HT being an array
            else
                of << "NULL";
            of << std::endl << std::endl;
            
            // Hash Anchor Table
            for (int i = 0; i < mCapacity; i ++)
            {
                of << "BUCKET[" << i << "] -> ";
                if (hashAnchor[i] != NULL)
                    of << "HT[" << (hashAnchor[i] - hashTable) << "]"; // Pointer sub gives index due to HT being an array
                else
                    of << "NULL";

                of << std::endl;
            }
            of << std::endl;

            // The actual Hash Table
            T *valuePtr;
            for (int i = 0; i < mSize; i ++)
            {
                // Current entry's position
                of << "HT[" << i << "]:" << std::endl;
                of << "    Key: " << hashTable[i].key << std::endl;

                // Current entry's value (account for NULL)
                of << "    Data: ";
                valuePtr = hashTable[i].value;
                if (valuePtr != NULL)
                    of << *valuePtr;
                else
                    of << "NO DATA";
                of << std::endl;

                // Current entry's next node (account for NULL)
                of << "    Next: ";
                if (hashTable[i].next != NULL)
                    of << "HT[" << (hashTable[i].next - hashTable) << "]"; // Pointer sub gives index due to HT being an array
                else
                    of << "NULL";

                // Leave a line inbetween entries
                of << std::endl << std::endl;
            }
            
            of.close();
        }

    private:
        // Private Member Functions
        int hashit(const std::string &key)
        {
            // Take the ascii value of the char and normalize it with a modulo
            return (int)key[0] % mCapacity;
        }
        
        // Private Data Members
        int mSize;                                  // Number of entries pre-allocated (and unchangeable at that)
        int mCapacity;                              // Number of buckets
        int numItems;                               // Number of items currently in the hashmap
        BucketNode<T> **hashAnchor;                 // Pointer to hash anchor: every entry points to the "head" of each bucket
        BucketNode<T> *hashTable;                   // Pointer to a fixed-size BucketNode table - which is really just an array
        BucketNode<T> *freePtr;                     // Pointer to the head of the free list
};
