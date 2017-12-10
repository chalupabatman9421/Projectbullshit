//
//
//
//Justin Hahn
//
//

#include "tlb.hpp"

// Outputs the display addresses 
int displayAddresses(bool choiceForDisplay, int count, logicAddressList_t logicAddrList, physAddressList_t physAddrList, valueList_t valueList) {
    if (choiceForDisplay == true) {
        for (int i = 0; i < count; i++) {
            cout << "Virtual Address: " << logicAddrList[i] << "; Physical Address: " << physAddrList[i] << "; Value: " << (int) valueList[i] << endl;
        }
    }

    return 0;
}

// look at physical mem to read 
int physMemRead (physicalAddress_t p_addr, frame physical_memory[NUM_FRAMES], value_t *value) {
    int offset = p_addr % FRAME_SIZE;
    int row = p_addr / FRAME_SIZE;

    const char x = physical_memory[row].page[offset];
    *value = x;

    return 0;
}

// Inserts the physical addresses 
int updateLists(physicalAddress_t physAddress, value_t value, physAddressList_t *physAddressList, valueList_t *valueList) {

    physAddressList->push_back(physAddress);
    valueList->push_back(value);

    return 0;
}




int writeToOutput(logicAddressList_t logicAddrList, physAddressList_t physAddrList, valueList_t valueList, int count) {
    ofstream file;
	
    file.open("vm_sim_output.txt");
    for (int i = 0; i < count; i++) {
        file << "Virtual Address: " << logicAddrList[i] << "; Physical Address: " << physAddrList[i] << "; Value: " << (int) valueList[i] << endl;
    }
    file.close();

    return 0;
}



int lgcAddressMaker(string fileName, vector<literalAdd_t> * logicAddrList) {
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



int grabLgcAddresses(literalAdd_t address, tlbPageEntry_t * pageNum, offsetAmount_t * offset) {
    *pageNum = address >> OFFSET_BITS;
	
    *offset = address & OFFSET_MASK;
    return 0;
}

// Initialize pg tbl
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
int initPhysicalMem(frame physical_memory[NUM_FRAMES]){
    for (int i = 0; i < NUM_PAGES; i++) {
        physical_memory[i].valid = false;
    }
    return 0;


}




int searchTLB(tlbPageEntry_t * pageNum, bool * isTlbHit, frame_t * frameNum, tlb * tlbSearch) {
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



int searchPageTable(tlbPageEntry_t pageNum, bool * isPageFault, frame_t * frameNum, pageTable_t tlbPageEntry_table) {
    if (tlbPageEntry_table[pageNum] == EMPTY_PAGE) {
        *isPageFault = true;
    }
    else {
        *frameNum = tlbPageEntry_table[pageNum];
    }
    return 0;
}




int TLB_display(tlb * tlb) {
    for (int i = 0; i < TLB_SIZE; i++) {
        cout << "TLB entry " << i << ", page num: " << tlb->tlb_entry[i].pageNum
        << ", frame num: " << tlb->tlb_entry[i].frameNum;
        if (tlb->tlb_entry[i].valid == false)             printf("Invalid\n");
        else printf("Valide\n");
    }


    return 0;
}




int load_frame_to_physical_memory(tlbPageEntry_t pageNum, const char *backingStoreFileName, frame physical_memory[NUM_FRAMES], frame_t *frameNum) {
    FILE *file = fopen(backingStoreFileName, "r");
    fpos_t pos;
    char one_byte;


    if (file == 0) {
        printf("Could not open file: %s.\n", backingStoreFileName);
    }


    else {
        fseek(file, pageNum * FRAME_SIZE, SEEK_SET);
        //fgetpos(file, &pos); didn't work come back and look
        frame_t frame;
        for (frame = 0; frame < 256; frame++) {
            
            if (!physical_memory[frame].valid) 
			{
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




int createPhysicalAddress(frame_t f_num, offsetAmount_t off, physicalAddress_t *physical_addr) {
    *physical_addr = f_num * FRAME_SIZE + off;
    return 0;
}




int TLB_replacement_FIFO(tlbPageEntry_t pageNum, frame_t frameNum, tlb *tlb) {
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



int TLB_replacement_LRU(tlbPageEntry_t pageNum, frame_t frameNum, tlb *tlb) {
    for (int i = 0; i < TLB_SIZE; i++) {
        // If the tlb isn't full yet
        if (tlb->tlb_entry[i].valid == false) {
            tlb->tlb_entry[i].pageNum = pageNum;
            tlb->tlb_entry[i].frameNum = frameNum;
            tlb->tlb_entry[i].valid = true;
            return 0;
        }
    }
    // Find the oldest
    int oldAge = 0;
    int indexOfOldest = 0;
    for (int i = 0; i < TLB_SIZE; i++) {
        if (oldAge < tlb->tlb_entry[i].age) {
            oldAge = tlb->tlb_entry[i].age;
            indexOfOldest = i;
        }
    }
    // Replace the oldest
    tlb->tlb_entry[indexOfOldest].pageNum = pageNum;
    tlb->tlb_entry[indexOfOldest].frameNum = frameNum;
    tlb->tlb_entry[indexOfOldest].age = 0;
    tlb->tlb_entry[indexOfOldest].valid = true;


    return 0;
}

// Handle a page fault
int handlePageFault(tlbPageEntry_t p_num, frame_t *frame_num, frame physical_memory[NUM_FRAMES], pageTable_t p_table, tlb tlb) {
    load_frame_to_physical_memory(p_num, "BACKING_STORE", physical_memory, frame_num);
    p_table[p_num] = *frame_num;


    return 0;
}
