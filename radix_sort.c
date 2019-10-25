#include <stdio.h>
#include <stdlib.h>
#include "GeneralHeader.h"

/**
 * Copies a part of the source relation to the target relation with the use of 
 * the cumulative histogram (psum)
 * @param relation* The source relation
 * @param relation* The target relation
 * @param uint64_t The starting index of the relation
 * @param uint64_t The ending index of the relation (it is not included in 
 *                 the transfer)
 * @param uint64_t* The cumulative histogram (psum)
 * @param unsigned Which byte is used to search in the cumulative histogram
 * @return 
 */
int copy_relation_with_psum(relation* source, relation* target,uint64_t index_start,uint64_t index_end,uint64_t* psum,unsigned short nbyte)
{
    //Check the values given //Can be removed
    if(source==NULL||source->tuples==NULL||target==NULL||target->tuples==NULL||
            index_start>=index_end||index_end>source->num_tuples||
            source->num_tuples!=target->num_tuples)
    {
        fprintf(stderr, "%s", "copy_relation: wrong parameters\n");
        return 1;
    }
    //Start the copy
    uint64_t target_index;
    short histogram_index;
    for(uint64_t i=index_start;i<index_end;i++)
    {
        //Find in which place the next tuple will be copied to from the psum
        histogram_index = ((source->tuples[i].key) >> ((8-nbyte) << 7)) & 0xff;
        printf("i: %ld histogram %hd\n",i,histogram_index);
        target_index=psum[histogram_index]+index_start;
        printf("%ld\n",target_index); //will be removed 
        //Increase the psum index for the next tuple with the same byte
        psum[histogram_index]++;
        //Copy the tuple
        target->tuples[target_index].key=source->tuples[i].key;
        target->tuples[target_index].row_id=source->tuples[i].row_id;
    }
    return 0;
}

void radix_sort(unsigned short byte, relation *source, relation *result, uint64_t start_index, uint64_t end_index)
{
	//Check if bucket is small enough
	if((end_index - start_index)*sizeof(tuple) < 64*1024)
	{
		//Choose whether to place result in source or result array
		if(byte % 2 == 0)
		{
			quicksort(source->tuples, start_index, end_index-1, result->tuples);
			//copy_relation(source, result, start_index, end_index);
		}
		else
		{
			quicksort(source->tuples, start_index, end_index-1, NULL);
		}
	}
	else
	{
		uint64_t *hist = malloc(HIST_SIZE*sizeof(uint64_t));
		create_histogram(source, start_index, end_index, hist, byte);
		transform_to_psum(hist);
		copy_relation_with_psum(source, result, start_index, end_index, hist, byte);
		
		//Recursively sort every part of the array
		uint64_t start = start_index;
		for(uint64_t i=0; i<HIST_SIZE; i++)
		{
			radix_sort(byte+1, result, source, start, hist[i]);
			start = hist[i];
		}
	}
}