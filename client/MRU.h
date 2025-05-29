
//
// MRU maintains a list of 'Most Recently Used' strings in the registry
// 

#ifndef MRU_H__
#define MRU_H__

#pragma once

#include <windows.h>
#include <tchar.h>

class MRU  
{
public:

    // Create an MRU and initialise it from the key.
    // Key created if not existant.

    MRU(LPTSTR keyname, unsigned int maxnum = 8);

    // Add the item specified at the front of the list
    // Move it there if not already.  If this makes the
    // list longer than the maximum, older ones are deleted.
    void AddItem(LPTSTR txt);

    // How many items are on the list?
    int NumItems();

    // Return them in order. 0 is the newest.
    // NumItems()-1 is the oldest.
    int GetItem(int index, LPTSTR buf, int buflen);

    // Remove the specified item if it exists.
    void RemoveItem(LPTSTR txt);

    // Remove the item with the given index.
    // If this is greater than NumItems()-1 it will be ignored.
    void RemoveItem(int index);

	virtual ~MRU();

private:
	void ReadIndex();

    TCHAR m_index[28];  // allow a bit of workspace beyond maximum of 26
    HKEY m_hRegKey;
    unsigned int m_maxnum;

    void Tidy();
protected:
	void WriteIndex();
};

#endif // MRU_H__

