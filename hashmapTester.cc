/******************************
 * Author: Joshua Wang
 * Date: 10/16/2015
 * Contact: jwangc2@gmail.com
 */

#include <iostream>
#include <vector>
#include <cstdlib>
#include "FixedHashmap.h"

// Global Prototypes
void printHelp();
std::vector<std::string> parseString(std::string st, std::string delimSet, int maxCount);

// ENTRY POINT
int main()
{
    // Initialize HT
    std::vector<std::string> keys;

    int uSize = 32;
    std::cout << "Size of the HT? ";
    std::cin >> uSize;

    FixedHashmap<int> mhts(uSize);
    std::cout << "Hashtable is a go!" << std::endl;

    printHelp();
    std::cout << std::endl;

    // Variables to be used in the loop
    bool isRunning = true;              // Loop control

    std::string key = "";               // A key is ALWAYS needed when accessing the HM
    int *valPtr = NULL;                 // The pointer to data is important too

    std::string input;                  // The current input
    std::vector<std::string> results;   // The current input, parsed into chunks
    
    // Input loop
    do
    {
        // Get the current command and parse it
        getline(std::cin, input);
        results = parseString(input, " ", 3);

        if (results.size() > 0)
        {
            bool error = false; // Assume the best
            if (results[0].compare("SET") == 0)
            {
                // SET [KEY] [VALUE]
                if (results.size() >= 3)
                {
                    key = results[1];
                    valPtr = new int(atoi(results[2].c_str())); // dynamic alloc required for user input (runtime / on the fly)

                    // Set on the hashmap and print the result
                    if (mhts.set(key, valPtr))
                    {
                        keys.push_back(key);
                        std::cout << "Your value [" << *valPtr << "] has been paired with the key [" << key << "]." << std::endl;
                    }
                    else
                        std::cout << "Failed to pair the value [" << *valPtr << "] with the key [" << key << "]." << std::endl;
                }
                else
                { error = true; }
            }
            else if (results[0].compare("GET") == 0)
            {
                // GET [KEY]
                if (results.size() >= 2)
                {
                    key = results[1];
                    valPtr = mhts.get(key);
                    if (valPtr != NULL)
                        std::cout << "Found a value of [" << *valPtr << "] for the key [" << key << "]." << std::endl;
                    else
                        std::cout << "No value found for the key [" << key << "]." << std::endl;
                }
                else
                { error = true; }
            }
            else if (results[0].compare("DEL") == 0)
            {
                // DEL [KEY]
                if (results.size() >= 2)
                {
                    key = results[1];
                    valPtr = mhts.del(key);
                    
                    if (valPtr != NULL)
                    { 
                        // Remember to dealloc!
                        std::cout << "Successfully removed the key [" << key << "] with a value of [" << *valPtr;
                        delete valPtr;
                    }
                    else
                        std::cout << "Failed to remove the key [" << key;

                    std::cout << "]." << std::endl;
                }
                else
                { error = true; }
            }
            else if (results[0].compare("PRINT") == 0)
            {
                // PRINT [FILENAME]
                if (results.size() >= 2)
                {   
                    mhts.print(results[1].c_str());
                    std::cout << "Printed the Hashtable to the file [" << results[1] << "]." << std::endl;
                }
                else
                { error = true; }
                
            }
            else if (results[0].compare("HELP") == 0)
            { printHelp(); }
            else if (results[0].compare("END") == 0)
            { isRunning = false; }
            else
            { error = true; }

            // Print out a generic error message
            if (error)
                std::cout << "Didn't understand your command there." << std::endl;
            
            std::cout << std::endl;
        }
    } while (isRunning);


    // Time to clean up the leftover references
    std::cout << "Cleaning up hashtable..." << std::endl;
    for (int i = 0; i < keys.size(); i ++)
    {
        valPtr = mhts.del(keys[i]);
        if (valPtr != NULL)
        {
            std::cout << "Cleaning an entry for [" << keys[i] << "] with a value of [" << *valPtr << "]." << std::endl;
            delete valPtr;
        }
    }

    return 0;
}

// Prints the help content to the console
void printHelp()
{
    std::cout << "Use SET [KEY] [VALUE] to set a value in the hashmap, where [VALUE] is an <integer>." << std::endl;
    std::cout << "Use GET [KEY] to get a value in the hashmap." << std::endl;
    std::cout << "Use DEL [KEY] to delete a value in the hashmap." << std::endl;
    std::cout << "Use PRINT [FILENAME] to write the content of the hashmap to a file." << std::endl;
    std::cout << "Use HELP to review these instructions." << std::endl;
    std::cout << "Use END to end the program." << std::endl;
}

// Returns all the substrings delimited by [delimSet] in the given string [st], up to [maxCount]
std::vector<std::string> parseString(std::string st, std::string delimSet, int maxCount)
{
    // Init variables
    std::vector<std::string> substr;
    std::string::size_type i = 0;
    int count = 0;

    if (st.empty()) return substr; // No operands
    while( (i< st.length()) && (count < maxCount) )
    {
        // Look for the next occurence of the delim
        std::string::size_type j = st.find_first_of(delimSet, i);
        if (j == std::string::npos)
        {
            // Not found, take whatever is left (the while loop will terminate)
            substr.push_back(st.substr(i, j));
            count++;
            i = j;
        }
        else
        {
            // Great success, a new day a new substring
            substr.push_back(st.substr(i, j-i));
            count++;
            i = j + 1;
        }
    }
    return substr;
}
