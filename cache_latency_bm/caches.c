////////////////////////////////////////////////////////////////////////////////
// Caches (cache acces latency) 
// Author: Alexander Lumsden 2019 
// orignal code from: Christopher Celio 2010 
//
// Designed to determine the latancy of the cache levels of the computer
//
// Create an array of size <input>, where each array element is a pointer to
// another element in the array (circular linked list). Arrange pointers
// randomly to minimize cache-prefetching effects, and walk the array for set
// period of elements to normalize for work/array-size. Thus, any increase in
// run-time should be localized to caching effects.
//
// The input is the size in bytes of the array, 
// This means that
// the graphing code should pay attention to the report file. 

//#define DEBUG 
//#define PRINT_ARRAY
                      

// stride in # of elements, most processors have a 64byte cache line, so 16 elements
// would stride across cache lines.
#define CACHELINE_IN_BYTES_SZ (256)
//#define CACHELINE_IN_BYTES_SZ (64)
#define CACHELINE_SZ (CACHELINE_IN_BYTES_SZ/sizeof(uint32_t))

// in # bytes
#define PAGE_SZ (4096) 

#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <cctimer.h>
#include <threadaffinity.h>
#include <cclfsr.h>
#include <math.h>

// Global Variables
uint32_t  g_num_elements;  
uint32_t  g_num_iterations;
pthread_mutex_t console_mutex;
//uint32_t  g_performed_iterations;

char *level;

double g_run_time;  // choose between stride size, or random stride 
// random access should hit cache-line addresses, to eliminate
// spatial locality and force more misses.
int g_stride = CACHELINE_SZ;

// Function Declarations
uint32_t initializeGlobalArrays(uint32_t* arr_n_ptr, uint32_t num_elements, uint32_t stride);
uint32_t threadMain(uint32_t);
void threadStart(void *);

uint32_t printArray(uint32_t iter, uint32_t *arr_ptr, uint32_t num_elements, uint32_t stride);
uint32_t verifyArray(uint32_t *arr_ptr, uint32_t num_elements, uint32_t stride);

int main(int argc, char* argv[])
{
#ifdef DEBUG  
   printf("\nBegin Test");
#endif

   if (argc != 5) 
   {
      fprintf(stderr, "argc=%d\n", argc);
      fprintf(stderr, "\n[Usage]: cache_size <Number of Array Elements (Total)> <Number of Iterations> <MinRunTime> \n\n");
      return 1;
   }
   
   level            = argv[1];
   g_num_elements   = atoi(argv[2]) / 4;
   g_num_iterations = atoi(argv[3]);
   g_run_time       = atof(argv[4]);
   if (pthread_mutex_init(&console_mutex, NULL) != 0){
         fprintf(stdout, "mutex failed\n");
   }

#ifdef DEBUG
   fprintf(stderr, "Size of the array     = %d\n",  g_num_elements);
   fprintf(stderr, "Number of Iterations  = %d\n",  g_num_iterations);
   fprintf(stderr, "min run time = %f\n\n", g_run_time);
#endif

   // make the array size a multiple of the stride; round up
   // actually stride*2 so we can insure that no consecutive accesses are adjacent, and this makes implementing this easier
   if (g_num_elements % (2*g_stride) != 0)
      g_num_elements = g_num_elements + ((2*g_stride) - (g_num_elements % (2*g_stride)));

#ifdef DEBUG
   printf(" Adjusted Num of Elements: %d\n\n", g_num_elements);
#endif
   if (strcmp(level, "L3") != 0) {
      int num_cores = sysconf(_SC_NPROCESSORS_CONF);
      cpu_thread_t** threads = setThreads(threadStart, num_cores);  

      for(uint32_t i = 0 ; i < num_cores ; i++) {
         pthread_join(*(threads[i]->thread), NULL);
      }
      for(uint32_t i = 0 ; i < num_cores ; i++) {
         freeCPUSet(threads[i]->cpu_set);
         free(threads[i]);
      } 
      fflush(stdout);
      free(threads); 
   }
   else {
      uint32_t volatile ret_val = threadMain(0);
   }
   
   // this volatile ret_val is crucial, otherwise the entire run-loop 
   // gets optimized away! :(
   //uint32_t volatile ret_val = threadMain();  

#ifdef DEBUG
  fprintf(stderr, "Done. Exiting...\n\n");
#endif

  return 0;
}

void threadStart(void *tid) {
      uint32_t id = *(uint32_t *) tid;
      uint32_t volatile ret_val = threadMain(id); 
}


uint32_t threadMain(uint32_t id)
{
   uint32_t* arr_n_ptr;
   // pad out memory to next PAGE_SZ to make initialization easier...
   uint32_t num_total_elements_per_page = PAGE_SZ/sizeof(uint32_t);
   int num_elements_allocated = g_num_elements + (num_total_elements_per_page - (g_num_elements % num_total_elements_per_page));
   
   
   arr_n_ptr   = (uint32_t *) malloc(num_elements_allocated * sizeof(uint32_t));
   
   initializeGlobalArrays( arr_n_ptr,
                           g_num_elements,
                           g_stride);

   /** CRITICAL SECTION **/

   intptr_t idx = 0;
   double volatile elapsed_time = 0;
   double volatile run_time_s;
   while (elapsed_time < g_run_time) {
      // run for g_num_iterations...
      cctime_t volatile start_time = cc_get_seconds(0); 

      for (uint32_t k = 0; k < g_num_iterations; k++)
      {
            idx = arr_n_ptr[idx];
      }
      cctime_t volatile stop_time = cc_get_seconds(0);
      run_time_s = (double) stop_time - start_time;
      elapsed_time += run_time_s;
      pthread_mutex_lock(&console_mutex);
      fprintf(stdout, "level:[%s],thread:[%d],totalTime:[%f],latency:[%f]\n", level, id, elapsed_time, (run_time_s * 1.0E9)/(double)g_num_iterations);
      pthread_mutex_unlock(&console_mutex);
   }

#ifdef DEBUG
   fprintf(stderr, "Total_Time (s)             : %g\n", run_time_s);
   fprintf(stderr, "Total_Time (ns)            : %g\n", run_time_ns);
#endif

   free(arr_n_ptr);
   // prevent compiler from removing ptr chasing...
   // although the receiver must put idx into a volatile variable as well!
   return (uint32_t) idx; 
}


//generate a random linked list of a PAGE_SZ, starting at address arr_n_ptr[page_offset]
//the calling function is in charge of stitching the page together with the other pages
//num_accesses is the number of pointers we are going to add (not the number of
//elements spanned by the page)
//returns the index to the last index (to help the calling function stitch pages together)
//interleaving_space is the size between entries (i.e., 2 means generated lines are even/odd)
//assumes num_accesses can be divided by interleaving_space
uint32_t initializePage(uint32_t* arr_n_ptr, uint32_t page_offset, uint32_t num_accesses, uint32_t stride, uint32_t interleaving_space)
{
   if (interleaving_space > 2)
      printf("ERROR: unsupported interleaving_space\n");

#ifdef DEBUG
   printf("\n--(offset = %d) Generating Page-- num_el=%d, pageSz=%lu, strd=%d\n\n",
      page_offset, num_accesses, num_accesses*stride*sizeof(int32_t), stride);
#endif

   cc_lfsr_t lfsr;
   uint32_t lfsr_init_val = 1; //TODO provide different streams different starting positions?
   //(-1) because we are going to loop through twice, once for odd and once for even entries 
   uint32_t lfsr_width = (log(num_accesses) / log(2)) - (interleaving_space - 1); //TODO EVENODD


   uint32_t max_accesses = (0x1 << lfsr_width)*interleaving_space;

//   printf("width=%d || max_accesses = %d, num_elements = %d\n", lfsr_width, max_accesses, num_accesses);

   // special case to handle non-powers of 2 num_accesses
   if (max_accesses < num_accesses)
      lfsr_width+=interleaving_space;


   //uint32_t idx;
   uint32_t curr_idx, next_idx;

   curr_idx = page_offset;

//   printf("lfsr_width = %d\n", lfsr_width);

   // special cases to handle the VERY small pages (too small to use a LFSR)
   if (lfsr_width < 2)
   {
      // lfsr width too small, forget about randomizing this page...
      for (int i=0; i < num_accesses-1; i++)
      {
         arr_n_ptr[curr_idx] = curr_idx + stride;
         curr_idx += stride; 
      }
      
      arr_n_ptr[curr_idx] = page_offset;
      return curr_idx;
   }

   
   cc_lfsr_init(&lfsr, lfsr_init_val, lfsr_width);


   // "-1" because we do the last part separately (lfsr's don't generate 0s)
   for (int i=0; i < num_accesses-interleaving_space; i+=interleaving_space)
   {
//      printf("i=%d, i < %d-%d = %d\n", i, num_accesses, interleaving_space, (num_accesses-interleaving_space));
      //TODO do both even, odd at same time...
      next_idx = lfsr.value * (interleaving_space*stride) + page_offset; //TODO EVENODD
      
      arr_n_ptr[curr_idx] = next_idx;
      if (interleaving_space == 2)
         arr_n_ptr[curr_idx+stride] = next_idx+stride;


#ifdef DEBUG
      printf("   array[%4d] = %4d,   lfsr.value = %4d, i = %4d EVEN\n", 
                  curr_idx, arr_n_ptr[curr_idx], lfsr.value, i); 
      if (interleaving_space == 2)
         printf("   array[%4d] = %4d,   lfsr.value = %4d, i = %4d ODD\n", 
                     curr_idx+stride, arr_n_ptr[curr_idx+stride], lfsr.value, i); 
#endif

      curr_idx = next_idx;
      lfsr.value = cc_lfsr_next(&lfsr);

      //handle non powers of 2 num_accesses
//      while (lfsr.value > (num_accesses-1))
      while (lfsr.value > ((num_accesses-1)/interleaving_space))
      {
         lfsr.value = cc_lfsr_next(&lfsr);
      }
   }

   //handle last index, wrap back to the start of the page
   //let calling function stitch to next pages...
   if (interleaving_space==1)
   {
      arr_n_ptr[curr_idx] = page_offset;
   }
   else
   {
      arr_n_ptr[curr_idx] = page_offset+stride;
      arr_n_ptr[curr_idx+stride] = page_offset;
#ifdef DEBUG
      printf("  *array[%4d] = %4d,   lfsr.value = %4d, \n", 
                  curr_idx, arr_n_ptr[curr_idx], lfsr.value); 
      printf("  *array[%4d] = %4d,   lfsr.value = %4d, \n", 
                  curr_idx+stride, arr_n_ptr[curr_idx+stride], lfsr.value); 
#endif
      curr_idx += stride;
   }


   return curr_idx;
}


// this is the future initializeGlobalArrays, but it has one or two bugs so it's commented out for now
// it will more accurately provide the L3 access times on some processors
uint32_t initializeGlobalArrays(uint32_t* arr_n_ptr, uint32_t num_elements, uint32_t stride)
{
   
      //randomize array on cacheline strided boundaries
#ifdef DEBUG
      printf("-==== Begin two-level randomization of the chase array ====-\n");
#endif
 
      // check to see if the array is 1-element long...
      if (num_elements == stride)
      {
#ifdef DEBUG
         printf("\nArray is 1 element long, returning....\n");
#endif
         // points back on itself
         arr_n_ptr[0] = 0;
         return 0;
      }
      
      uint32_t num_elements_per_page = PAGE_SZ/sizeof(uint32_t);

#ifdef DEBUG
      printf("\nnum_elements_per_page = %lu\n\n", PAGE_SZ/sizeof(uint32_t));
#endif
                    
      
      uint32_t last_idx;

      // two-level randomization (this is the outer level for-loop)
      // for each page...
      for(int i=0; i < num_elements; i+=num_elements_per_page)
      {
#ifdef DEBUG
         printf("\nStarting New Page: i=%d, page offset = %d\n", i, i); 
#endif
         
         uint32_t page_offset = i;


         //test for partial page (which is always the last page)...
         if ((int32_t) i >= (int32_t) (num_elements - num_elements_per_page)  //is last page?
            && ((num_elements - i) % num_elements_per_page != 0))  //and last page is partial
         {
            num_elements_per_page = num_elements - i; //num_elements % num_elements_per_page;
         }



         last_idx = initializePage(arr_n_ptr, page_offset, num_elements_per_page/stride, stride, 1);
         arr_n_ptr[last_idx] = (i+num_elements_per_page);
      }


      //handle the last page ...
      //wrap the array back to the start
      arr_n_ptr[last_idx] = 0;



    //end of randomized array creation

#ifdef DEBUG
   printf("\n\n");
#endif

#ifdef PRINT_ARRAY
   printArray(0, arr_n_ptr, num_elements, stride);
#endif

   verifyArray(arr_n_ptr, num_elements, stride);
  
   return 0;
}


uint32_t printArray(uint32_t iter, uint32_t *arr_ptr, uint32_t num_elements, uint32_t stride )
{
   fprintf(stderr, "Chasing through Array (run thru for 2x*num_el/stride) after iteration: %d\n", iter);
   uint32_t idx = 0;
   for (uint32_t i = 0; i < 2*num_elements/stride; i++)
   {
      fprintf(stderr, "%3d, ", arr_ptr[idx]);
      idx = arr_ptr[idx];
   }
   fprintf(stderr, "\n");
   
   fprintf(stderr, "arr_ptr: ");
   for (uint32_t i = 0; i < num_elements; i++)
   {
      if (i % stride == 0) fprintf(stderr, "\n");
      fprintf(stderr, "%3d, ", arr_ptr[i]);
   }
   fprintf(stderr, "\n\n\n");

   return 0;
}


uint32_t verifyArray(uint32_t *arr_ptr, uint32_t num_elements, uint32_t stride)
{
   uint32_t idx = 0;
   uint32_t counter = 0;

   uint32_t first_idx = 0;
   uint32_t finished = 0;

   uint32_t* verify_array;
   verify_array = (uint32_t *) calloc(g_num_elements, sizeof(uint32_t));
   
   while (!finished)
   {
      verify_array[idx] = verify_array[idx] + 1;
      counter++;
      idx = arr_ptr[idx];

      if(counter >= num_elements || idx == first_idx)
         finished = 1;
   }

   uint32_t error = 0;

   for (uint32_t i = 0; i < num_elements; i+=stride)
   {
      if(verify_array[i] != 1)  
      {
#ifdef DEBUG
         fprintf(stderr, "Error at Element [%d], accessed %d times\n", 
            i,
            verify_array[i]
            );
#endif
         error = 1;
      }
   }

   if (counter==(num_elements/stride) && !error) {
#ifdef DEBUG
      fprintf(stderr,"Array verified\n\n");
#else
      int x=0; //here for compiler reasons
#endif
   } 
   else 
   {
      fprintf(stderr,"Error: Array size:%d, Stride size:%d, Loops in:%d (wants to loop in %d)\n\n",
         num_elements,
         stride,
         counter,
         num_elements/stride);
   }

   return error | (counter==(num_elements/stride));
}

