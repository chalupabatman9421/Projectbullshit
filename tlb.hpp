//
//  tlb.hpp
//  COMP3500_Project5_TLB
//
//  Justin Hahn
//

#ifndef tlb_hpp
#define tlb_hpp

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <vector>

using namespace std;

/* offset is used to address the location within a page */
#define OFFSET_BITS 8
/* mask is used to extract the offset from a logical address */
#define OFFSET_MASK 0xFF

#define EMPTY_PAGE 3435973836
#define NUM_PAGES 256
#define NUM_FRAMES 256
#define PAGE_SIZE 256
#define FRAME_SIZE 256
#define TLB_SIZE 16
#define LOGIC_ADDR_LIST_SIZE 1024

/* There is no byte data type in C */
typedef unsigned char byte;

/* new data type of pages, frames, and offset */
typedef unsigned int page_on_table;
typedef unsigned int physicaladdressholder;
typedef vector<physicaladdressholder> physAddressList_t;
typedef char physicalMemArray[NUM_FRAMES][PAGE_SIZE];
typedef vector<logicaddresholder> logicAddressList_t;
typedef char value_A;
typedef unsigned int logicaddresholder;
typedef vector<char> valueList_t;
typedef unsigned int value_on_table;
typedef unsigned int amountToBeOffSet;


typedef struct {
    page_on_table pageNum;
    value_on_table frameNum;
    bool valid;
    int age;
} tlbEntry;


typedef struct {
    tlbEntry tlb_entry[TLB_SIZE];
    unsigned int next_tlb_ptr;
} tlb;


typedef struct {
    char page[PAGE_SIZE];
    bool valid;
} frame;

//typedef frame physicalMemArray[NUM_FRAMES];
typedef value_on_table pageTable_t[NUM_PAGES];

int logicAdrrLoader(string fileName, vector<logicaddresholder> * logicAddrList);
int extractLogicAddr(logicaddresholder address, page_on_table * pageNum, amountToBeOffSet * offset);
int initPageTable(pageTable_t pageTable);
int TLB_init(tlb *tlb);
int PhsyMemInit(frame physical_memory[NUM_FRAMES]);
int searchTLB(page_on_table * pageNum, bool * isTlbHit, value_on_table * frameNum, tlb * tlbSearch);
int searchPageTable(page_on_table pageNum, bool * isPageFault, value_on_table * frameNum, pageTable_t page_on_tableable);
int handlePageFault(page_on_table p_num, value_on_table *frame_num, frame physical_memory[NUM_FRAMES], pageTable_t p_table, tlb tlb);
int load_value_on_tableo_physical_memory(page_on_table pageNum, const char *backingStoreFileName, frame physical_memory[NUM_FRAMES], value_on_table *frameNum);
int createPhysicalAddress(value_on_table f_num, amountToBeOffSet off, physicaladdressholder *physical_addr);
int TLB_replacement_FIFO(page_on_table pageNum, value_on_table frameNum, tlb *tlb);
int TLB_replacement_LRU(page_on_table pageNum, value_on_table frameNum, tlb *tlb);
int readPhysicalMemory (physicaladdressholder p_addr, frame physical_memory[NUM_FRAMES], value_A *value);
int update_all_lists(physicaladdressholder physAddress, value_A value, physAddressList_t *physAddressList, valueList_t *valueList);
int output_all_lists(logicAddressList_t logicAddrList, physAddressList_t physAddrList, valueList_t valueList, int count);
int displayAddresses(bool displayAddress, int count, logicAddressList_t logicAddrList, physAddressList_t physAddrList, valueList_t valueList);

#endif /* tlb_hpp */
