//
//
//
//
//
//

#include "tlb.hpp"

// Outputs the display addresses with their virtual/physical address and value
int displayAddresses(bool displayAddrChoice, int count, logicAddressList_t logicAddrList, physAddressList_t physAddrList, valueList_t valueList) {
    if (displayAddrChoice == true) {
        for (int i = 0; i < count; i++) {
            cout << "Virtual Address: " << logicAddrList[i] << "; Physical Address: " << physAddrList[i] << "; Value: " << (int) valueList[i] << endl;
        }
    }

    return 0;
}

// Reads out the value from the physical memory location
int readPhysicalMemory (physicaladdressholder p_addr, frame physical_memory[NUM_FRAMES], value_A *value) {
    int offset = p_addr % FRAME_SIZE;
    int row = p_addr / FRAME_SIZE;

    const char x = physical_memory[row].page[offset];
    *value = x;

    return 0;
}

// Inserts the physical addresses and corresponding values into the arrays for later use
int update_all_lists(physicaladdressholder physAddress, value_A value, physAddressList_t *physAddressList, valueList_t *valueList) {

    physAddressList->push_back(physAddress);
    valueList->push_back(value);

    return 0;
}

// Outputs all lists to the output file 'vm_sim_output.txt'
int output_all_lists(logicAddressList_t logicAddrList, physAddressList_t physAddrList, valueList_t valueList, int count) {
    ofstream file;
    file.open("vm_sim_output.txt");
    for (int i = 0; i < count; i++) {
        file << "Virtual Address: " << logicAddrList[i] << "; Physical Address: " << physAddrList[i] << "; Value: " << (int) valueList[i] << endl;
    }
    file.close();

    return 0;
}

// Opens the input file and pushes each of the logical addresses into an array
int logicAdrrLoader(string fileName, vector<logicaddresholder> * logicAddrList) {
    int count = 0;
    ifstream instream(fileName);
    if (instream.fail()) {
        cout << "File failed to open.";
        exit(1);
    }

    unsigned int nextLogicAddr;
    while (!instream.eof()) {
        instream >> nextLogicAddr;
        logicAddrList->push_back(nextLogicAddr);
        count++;
    }
    instream.close();
    return count;
}

// Converts the logical address into a page number and offset
int extractLogicAddr(logicaddresholder address, page_on_table * pageNum, amountToBeOffSet * offset) {
    *pageNum = address >> OFFSET_BITS;
    *offset = address & OFFSET_MASK;
    return 0;
}

// Initializes the page table
int initPageTable(pageTable_t pageTable) {
    for (int i = 0; i < NUM_PAGES; i++) {
        pageTable[i] = EMPTY_PAGE;
    }
    return 0;
}

// Initializes the TLB
int TLB_init(tlb *tlb) {
    unsigned int i;
    tlb->next_tlb_ptr = 0;
    for (i = 0; i < TLB_SIZE; i++)
        tlb->tlb_entry[i].valid = false;
    tlb->tlb_entry[i].age = 0;
    return 0;
}

// Initializes the phyiscal memory
int PhsyMemInit(frame physical_memory[NUM_FRAMES]){
    for (int i = 0; i < NUM_PAGES; i++) {
        physical_memory[i].valid = false;
    }
    return 0;


}

// Searches through the TLB for the corresponding page number to get the page frame and determine a tlb hit
int searchTLB(page_on_table * pageNum, bool * isTlbHit, value_on_table * frameNum, tlb * tlbSearch) {
    for (int i = 0; i < TLB_SIZE; i++) {
        if (tlbSearch->tlb_entry[i].valid && tlbSearch->tlb_entry[i].pageNum == *pageNum) {
            *isTlbHit = true;
            *frameNum = tlbSearch->tlb_entry[i].frameNum;
            return 0;
        }
    }
    *isTlbHit = false;
    return 0;
}

// Searcg the page table for the corresponding page number and get the frame number
int searchPageTable(page_on_table pageNum, bool * isPageFault, value_on_table * frameNum, pageTable_t page_on_tableable) {
    if (page_on_tableable[pageNum] == EMPTY_PAGE) {
        *isPageFault = true;
    }
    else {
        *frameNum = page_on_tableable[pageNum];
    }
    return 0;
}

// Display the contents of the TLB
int TLB_display(tlb * tlb) {
    for (int i = 0; i < TLB_SIZE; i++) {
        cout << "TLB entry " << i << ", page num: " << tlb->tlb_entry[i].pageNum
        << ", frame num: " << tlb->tlb_entry[i].frameNum;
        if (tlb->tlb_entry[i].valid == false)             printf("Invalid\n");
        else printf("Valide\n");
    }


    return 0;
}

// Loading from the BACKING_STORE into physical memory when necessary
int load_value_on_tableo_physical_memory(page_on_table pageNum, const char *backingStoreFileName, frame physical_memory[NUM_FRAMES], value_on_table *frameNum) {
    FILE *file = fopen(backingStoreFileName, "r");
    fpos_t pos;
    char one_byte;


    if (file == 0) {
        printf("Could not open file: %s.\n", backingStoreFileName);
    }


    else {
        fseek(file, pageNum * FRAME_SIZE, SEEK_SET);
        //fgetpos(file, &pos);
        value_on_table frame;
        for (frame = 0; frame < 256; frame++) {
            //If the frame is empty
            if (!physical_memory[frame].valid) {
                physical_memory[frame].valid = true;
                break;
            }
        }


        for (int i = 0; i < 256; i++) {
            fread(&one_byte, 1, 1, file);
            physical_memory[frame].page[i] = one_byte;
        }


        fclose(file);
        *frameNum = frame;
    }
    return 0;
}

// Creates the correct physical address based upon the frame number and offset
int createPhysicalAddress(value_on_table f_num, amountToBeOffSet off, physicaladdressholder *physical_addr) {
    *physical_addr = f_num * FRAME_SIZE + off;
    return 0;
}

// Implementation of the TLB FIFO replacement algorithm
int TLB_replacement_FIFO(page_on_table pageNum, value_on_table frameNum, tlb *tlb) {
    for (int i = 0; i < TLB_SIZE; i++) {
        // If the tlb isn't full yet
        if (tlb->tlb_entry[i].valid == false) {
            tlb->tlb_entry[i].pageNum = pageNum;
            tlb->tlb_entry[i].frameNum = frameNum;
            tlb->tlb_entry[i].valid = true;
            return 0;
        }
    }


    // If the tlb is full
    tlb->tlb_entry[tlb->next_tlb_ptr].pageNum = pageNum;
    tlb->tlb_entry[tlb->next_tlb_ptr].frameNum = frameNum;
    tlb->tlb_entry[tlb->next_tlb_ptr].valid = true;
    if (tlb->next_tlb_ptr == 15) {
        tlb->next_tlb_ptr = 0;
    }
    else {
        tlb->next_tlb_ptr++;
    }
    return 0;
}

// Implementation of the TLB LRU replacement algorithm
int TLB_replacement_LRU(page_on_table pageNum, value_on_table frameNum, tlb *tlb) {
    for (int i = 0; i < TLB_SIZE; i++) {
        // If the tlb isn't full yet
        if (tlb->tlb_entry[i].valid == false) {
            tlb->tlb_entry[i].pageNum = pageNum;
            tlb->tlb_entry[i].frameNum = frameNum;
            tlb->tlb_entry[i].valid = true;
            return 0;
        }
    }
    // Find the oldest tlb
    int oldestAge = 0;
    int oldestIndex = 0;
    for (int i = 0; i < TLB_SIZE; i++) {
        if (oldestAge < tlb->tlb_entry[i].age) {
            oldestAge = tlb->tlb_entry[i].age;
            oldestIndex = i;
        }
    }
    // Replace the oldest tlb
    tlb->tlb_entry[oldestIndex].pageNum = pageNum;
    tlb->tlb_entry[oldestIndex].frameNum = frameNum;
    tlb->tlb_entry[oldestIndex].age = 0;
    tlb->tlb_entry[oldestIndex].valid = true;


    return 0;
}

// Handle a page fault
int handlePageFault(page_on_table p_num, value_on_table *frame_num, frame physical_memory[NUM_FRAMES], pageTable_t p_table, tlb tlb) {
    load_value_on_tableo_physical_memory(p_num, "BACKING_STORE", physical_memory, frame_num);
    p_table[p_num] = *frame_num;


    return 0;
}
