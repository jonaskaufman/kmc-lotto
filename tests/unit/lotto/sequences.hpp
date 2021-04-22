#ifndef SEQUENCES_H
#define SEQUENCES_H

#include <set>
#include <vector>

// Dumb hash function to produce nonconsecutive values
int hash_int(int input) { return input * 7; }

// Generate an integer sequence of a given length
std::vector<int> hashed_sequence(int length)
{
    std::set<int> sequence;
    int i = 0;
    while (sequence.size() < length)
    {
        sequence.insert(hash_int(i));
        ++i;
    }
    return std::vector<int>(sequence.begin(), sequence.end());
}

#endif
