#include <stdio.h>
#include <stdlib.h>
#include "list_array.h"

/**
 * Creates a list_array (with middle_lists initialized with create_middle_list)
 * @param unsigned int size - the number of lists in the array
 * @return the list_array, NULL for error
 */
list_array *create_list_array(unsigned int size)
{
	list_array *la = malloc(sizeof(list_array));
	if(la == NULL)
	{
		perror("create_list_array: malloc error");
		return NULL;
	}
	la->num_lists = size;
	la->lists = malloc(size*sizeof(middle_list *));
	if(la->lists == NULL)
	{
		perror("create_list_array: malloc error");
		return NULL;
	}
	for(int i=0; i<size; i++)
	{
		la->lists[i] = malloc(2*sizeof(middle_list));
		if(la->lists[i] == NULL)
		{
			perror("create_list_array: malloc error");
			return NULL;
		}
		la->lists[i][0] = create_middle_list();
		la->lists[i][1] = create_middle_list();
	}

	return la;
}

/**
 * Appends the second middle list given, to the end of the first middle list given (possibly leaving gaps in the last bucket)
 * @param middle_list *main_list - the list to append to
 * @param middle_list *list - the list to append to the main_list
 */
void append_middle_list(middle_list *main_list, middle_list *list)
{
	if(main_list == NULL || list == NULL)
	{
		fprintf(stderr, "append_middle_list: NULL parameters\n");
		return;
	}

	//Second list is empty, nothing to append
	if(list->head == NULL)
	{
		return;
	}

	//First list is empty, replace it with second
	if(main_list->head == NULL)
	{
		main_list->head = list->head;
	}
	else
	{
		//Both lists are full, add head of second list to the end of first
		main_list->tail->next = list->head;
                //Update the gaps counters
                middle_list_node* temp=main_list->tail;
                uint64_t gaps_before=0;
                while(temp->next!=NULL)
                {
                    temp->next->bucket.gaps_before=temp->bucket.gaps_before+(middle_LIST_BUCKET_SIZE-temp->bucket.index_to_add_next);
                    temp=temp->next;
                }
	}
	main_list->tail = list->tail;
	main_list->number_of_nodes += list->number_of_nodes;
	main_list->number_of_records += list->number_of_records;
}

/**
 * Appends the second middle list given, to the end of the first middle list given (leaving no gaps in the buckets)
 * @param middle_list *main_list - the list to append to
 * @param middle_list *list - the list to append to the main_list
 */
void append_middle_list_no_gaps(middle_list *main_list, middle_list *list)
{
	if(main_list == NULL || list == NULL)
	{
		fprintf(stderr, "append_middle_list: NULL parameters\n");
		return;
	}

	//Second list is empty, nothing to append
	if(list->head == NULL)
	{
		return;
	}

	//First list is empty, replace it with second
	if(main_list->head == NULL)
	{
		main_list->head = list->head;
		main_list->tail = list->tail;
		main_list->number_of_nodes += list->number_of_nodes;
		return;
	}
	
	//Go through the second list and append its contents to the main_list
	middle_list_node*temp=list->head;
	while(temp!=NULL)//Visit all the nodes and print them
	{
		for(unsigned int i=0; i<temp->bucket.index_to_add_next; i++)
		{
		    append_to_middle_list(main_list, temp->bucket.row_ids[i]);
		}

	    temp=temp->next;
	}
	
}

/**
 * Merges all the lists of the list_array into two lists (one for R [0], one for S [1])
 * @param list_array *la - the list_array
 * @param middle_list *final_r - list to place the merged [0] lists (must be initialized with create_middle_list)
 * @param middle_list *final_s - list to place the merged [1] lists (must be initialized with create_middle_list)
 */
void merge_middle_lists(list_array *la, middle_list *final_r, middle_list *final_s)
{
	for(int i=0; i<la->num_lists; i++)
	{
//		append_middle_list_no_gaps(final_r, la->lists[i][0]);
		append_middle_list(final_r, la->lists[i][0]);
//		append_middle_list_no_gaps(final_s, la->lists[i][1]);
		append_middle_list(final_s, la->lists[i][1]);
	}
}

/**
 * Frees a list_array
 * @param list_array *la - the list_array
 */
void delete_list_array(list_array *la)
{
	for(int i=0; i<la->num_lists; i++)
	{
		delete_middle_list(la->lists[i][0]);
		delete_middle_list(la->lists[i][1]);
		free(la->lists[i]);
	}

	free(la->lists);
	free(la);
}
