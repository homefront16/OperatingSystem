#include <strings.h>
#include <stdio.h>

//Virtual memory size (1M bytes)
#define VM_SIZE   0xFFFFF

//512 Kb physical memory size
#define MEM_SIZE  0x7FFFF

static unsigned short PAGE_SIZE = 4095;

//page table for both sizes
static unsigned char page_table[VM_SIZE / 4096];  //supports both 4095 and 8091 page sizes
static unsigned char phyRam[MEM_SIZE * 8191L];  //physical memory

static void printBinary(unsigned short val){
  int i;
  for(i=0; i < 16; i++){
    if(val & 0x4f){ //take the 15th bit
      putchar('1');
    }else{
      putchar('0');
    }
    val <<= 1;  //drop rightmost bit
  }
  printf("\n");
}

static void fill_page_table(){

  int N = VM_SIZE / PAGE_SIZE;  //number of pages
  int pageNumber;
  int offsetInPage;

  for (int i = 0; i < N; i++)
  {
      if ((i % PAGE_SIZE) == 0)
      {
          
          offsetInPage = 0; // Will reset to zero on every PAGE_SIZE interval

          if (i == 0)
          {
              pageNumber = 0;
          }
          else
          {
              pageNumber += (PAGE_SIZE == 4095) ? 0x1000 : 0x2000;
          }
      }
      page_table[i] = pageNumber + offsetInPage;
      ++offsetInPage;
  }
}

int main(){
  int vaddr;
  int numTimes;

  printf("Enter page size : ");
  scanf("%hu", &PAGE_SIZE);

  // get page size
  if((PAGE_SIZE != 4095) && (PAGE_SIZE != 8191)){
    fprintf(stderr, "Error: PAGE_SIZE must be 4095 or 8191\n");
    return 1;
  }

  // display it
  printf("PAGE_SIZE = %d(10) = 0x%hx(16) = ", PAGE_SIZE, PAGE_SIZE);
  printBinary(PAGE_SIZE);

  
  fill_page_table();

  for(numTimes = 0; numTimes < 10; ++numTimes){
    // Prompt the user for a hexadecimal virtual memory address
    printf("Enter virtual address (0x0 to 0xFFFFF) : ");
    scanf("%x", &vaddr);
    printf("virtual address = 0x%x\n", vaddr);
    // check if its in range ?
    if((vaddr < 0) || (vaddr > MEM_SIZE)){
      fprintf(stderr, "Error: Virtual address out of range\n");
      return 1;
    }

    //e. Convert the virtual memory address to a physical memory address

    const int offsetBits = (PAGE_SIZE == 4091) ? 12 : 13;
    const int offset = vaddr & ((PAGE_SIZE == 4091) ? 0xfff : 0x1fff);  //page offset
    const int paddr = vaddr >> offsetBits;  //drop offset bits to get page index

    printf("physical address : 0x%x\n", paddr);
    printf("page offset : %d\n", offset);

    if(paddr > MEM_SIZE){
      printf("Currently On Disk.\n");
    }else{
      int i;

      //copy virtual to physical
      int p = vaddr / PAGE_SIZE;  //page number
      int o = vaddr % PAGE_SIZE;  //page offset

      for(i=0; i < PAGE_SIZE - o; i++){
        //copy the byte from page table to physical frame
        phyRam[paddr*PAGE_SIZE + o + i] = page_table[p*PAGE_SIZE + o + i];
      }
      printf("Copied %d bytes from page %d to frame %d\n", PAGE_SIZE - o, p, paddr);
    }
  }

  return 0;
}