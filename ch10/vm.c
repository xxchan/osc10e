#include <inttypes.h>
#include <stdlib.h>
#include <stdio.h>

#define GET_PAGE(addr) (((addr)&0xFF00) >> 8)
#define GET_OFFSET(addr) ((addr)&0xFF)

#define MEM_SIZE 65536
#define TLB_SIZE 16
#define NUM_OF_PAGES 256
#define NUM_OF_FRAMES 128
#define PAGE_SIZE 256

typedef struct TLB_entry
{
    uint8_t page;
    uint8_t frame;
    int valid;
} TLB_entry_t;
TLB_entry_t TLB[TLB_SIZE];

typedef struct page_table_entry
{
    uint8_t frame;
    int valid;
} PTE_t;
PTE_t PTB[NUM_OF_PAGES];

int8_t memory[MEM_SIZE];
FILE *backing_store;
int page_faults = 0;
int TLB_hits = 0;
int next_frame = 0;
int next_TLB_entry = 0;

uint8_t page2frame(uint8_t page);

void init()
{
    next_frame = next_TLB_entry = page_faults = TLB_hits = 0;
    backing_store = fopen("BACKING_STORE.bin", "rb");
    for (int i = 0; i < TLB_SIZE; ++i)
        TLB[i].valid = 0;
    for (int i = 0; i < NUM_OF_PAGES; ++i)
        PTB[i].valid = 0;
}

void cleanup()
{
    fclose(backing_store);
}

int main(int argc, char *argv[])
{
    FILE *fd;
    uint32_t addr;
    uint8_t page;
    uint8_t offset;
    uint8_t frame;
    int cnt = 0;

    if (argc < 2)
    {
        printf("error: no input file\n");
        return -1;
    }
    fd = fopen(argv[1], "r");

    init();

    while (fscanf(fd, "%d", &addr) != EOF)
    {
        addr = addr & 0xFFFF;
        printf("Virtual address: %d ", addr);
        page = GET_PAGE(addr);
        offset = GET_OFFSET(addr);
        frame = page2frame(page);
        addr = (frame << 8) + offset;
        printf("Physical address: %d ", addr);
        printf("Value: %d\n", memory[addr]);
        cnt++;
    }
    fclose(fd);
    printf("Statistics:\n");
    printf("Page-fault rate: %.2f\n", (float)page_faults / cnt);
    printf("TLB hit rate: %.2f\n", (float)TLB_hits / cnt);

    cleanup();
    return 0;
}

int TLB_lookup(uint8_t page)
{
    for (int i = 0; i < TLB_SIZE && TLB[i].valid; ++i)
    {
        if (TLB[i].page == page)
            return TLB[i].frame;
    }
    return -1;
}

void TLB_update(uint8_t page, uint8_t frame)
{
    int i = (next_TLB_entry++) % TLB_SIZE;
    TLB[i].page = page;
    TLB[i].frame = frame;
    TLB[i].valid = 1;
}

/* returns the new frame number */
uint8_t handle_page_fault(uint8_t page)
{
    uint8_t frame = next_frame % NUM_OF_FRAMES;
    if (next_frame >= NUM_OF_FRAMES)
    {
        /* eviction happens, we should set the PTE (old_page, frame) invalid */
        /* for simplicity, here is stupid linear search */
        for (uint8_t old_page = 0; old_page < NUM_OF_PAGES; ++old_page)
        {
            if (PTB[old_page].frame == frame)
            {
                PTB[old_page].valid = 0;
                break;
            }
        }
    }
    PTB[page].frame = frame;
    PTB[page].valid = 1;
    next_frame++;
    page_faults++;
    fseek(backing_store, page * PAGE_SIZE, SEEK_SET);
    fread(memory + frame * PAGE_SIZE, PAGE_SIZE, 1, backing_store);
    return frame;
}

uint8_t page2frame(uint8_t page)
{
    int frame;

    frame = TLB_lookup(page);
    /* TLB miss */
    if (frame == -1)
    {
        if (PTB[page].valid == 0)
            frame = handle_page_fault(page);
        else
        {
            frame = PTB[page].frame;
        }
        TLB_update(page, frame);
    }
    else
    {
        TLB_hits++;
    }
    return (uint8_t)frame;
}