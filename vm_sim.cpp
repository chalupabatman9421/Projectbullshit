/*
* VirtualMemoryManager.cpp : Defines the entry point for the console application.
* Source https://www.tutorialspoint.com/operating_system/os_virtual_memory.htm
* http://www.geeksforgeeks.org/virtual-memory-operating-systems/
* http://www.aqualab.cs.northwestern.edu/component/attachments/download/629
* https://www2.cs.uic.edu/~jbell/CourseNotes/OperatingSystems/9_VirtualMemory.html
*
*
*/

#include "tlb.hpp"

/*
 * In C language, there is no binary format in printf
 * You must implement the following functions to print binary format
 * itob16() and itob8() are modified from itob() by Xiao Qin.
 */
char *itob(int x);
char *itob16(int x);
char *itob8(int x);

// Flags used to determine user's choice
bool addressestoDisplay;
bool algoToUse;

//The start of displaying 
int main() {
    cout << "Welcome to Group Justin's VM Simulator Version 1.0\n\n"
    << "Number of logical pages: " << NUM_PAGES << endl
    << "Page size: " << PAGE_SIZE << " bytes" << endl
    << "Page table size: " << NUM_FRAMES << endl
    << "TLB size: " << TLB_SIZE << endl
    << "Number of physical frames: " << NUM_FRAMES << endl
    << "Physical memory size: 65,536 bytes\n\n";


    string choiceForDisplay;


    cout << "Display physical addresses? [y or n] ";
    cin >> choiceForDisplay;
    if (choiceForDisplay == "y" || choiceForDisplay == "Y") {
        addressestoDisplay = true;
    }
    else {
        addressestoDisplay = false;
    }


    int choiceForStrat;


    cout << "Choose TLB Replacement Strategy [1: FIFO, 2: LRU] ";
    cin >> choiceForStrat;
    if (choiceForStrat == 1) {
        algoToUse = true;
    }
    else {
        algoToUse = false;
    }


    tlbPageEntry_t pageNum;
    frame_t frameNum;
    offsetAmount_t offset;


    // Addresses
    literalAdd_t logicAddress;
    physicalAddress_t physicalAddress;
    value_t value;


    // The TLB 
    tlb tlb;
    pageTable_t pageTable;


    // Flags to keep track of TLB and page faults
    bool tlbHit;
    bool pageFault;


    // Simulated main memory
    frame physical_memory[NUM_FRAMES];
 


    // Address Lists
    physAddressList_t physAddressList;
    logicAddressList_t logicAddressList;
    valueList_t valueList;


    // File Names
    const char input_file[] = "InputFile.txt";
    const char backing_store_file_name[] = "BACKING_STORE";


 
    TLB_init(&tlb);
    initPageTable(pageTable);
    PhsyMemInit(physical_memory);

    
    double numOfPageFlts = 0;
    double smackDaTLB = 0;

    
    int count = logicAdrrLoader(input_file, &logicAddressList);


    for (int i = 0; i < count; i++) {
      
        extractLogicAddr(logicAddressList[i], &pageNum, &offset);


        // look at the TLB
        searchTLB(&pageNum, &tlbHit, &frameNum, &tlb);


        if (tlbHit == true) {
            smackDaTLB++;
            createPhysicalAddress(frameNum, offset, &physicalAddress);
        }


        // TLB miss
        else {
            searchPageTable(pageNum, &pageFault, &frameNum, pageTable);


            // page hit
            if (pageFault == false) {
                createPhysicalAddress(frameNum, offset, &physicalAddress);


                // TLB replacement methods
                if (algoToUse == true) {
                    TLB_replacement_FIFO(pageNum, frameNum, &tlb);
                }
                else {
                    TLB_replacement_LRU(pageNum, frameNum, &tlb);
                }
            }


            // Page Fault
            else {
                numOfPageFlts++;
                handlePageFault(pageNum, &frameNum, physical_memory, pageTable, tlb);


                // determine the replacement methods
                if (algoToUse == true) {
                    TLB_replacement_FIFO(pageNum, frameNum, &tlb);
                }
                else {
                    TLB_replacement_LRU(pageNum, frameNum, &tlb);
                }


                createPhysicalAddress(frameNum, offset, &physicalAddress);
            }
        } 


        
        readPhysicalMemory(physicalAddress, physical_memory, &value);

       
        update_all_lists(physicalAddress, value, &physAddressList, &valueList);
    } 


    // Sends everything to the output file
    output_all_lists(logicAddressList, physAddressList, valueList, count);
    displayAddresses(addressestoDisplay, count, logicAddressList, physAddressList, valueList);
    double pageFaultPercent = (numOfPageFlts / count) * 100;
    double tlbHitPercent = (smackDaTLB / count) * 100;

    cout << "\nPage Fault Rate: " << pageFaultPercent << "%"
    << "\nTLB Hit Rate: " << tlbHitPercent << "%\n\n"
    << "Check the results in the outputfile: vm_sim_output.txt\n\n";

    return 0;
}
