#include "sorter_server.h"

/*
	The merge function puts the data of 2 temporary arrays back into the main array
	Parameters: 	array - the main array that is to be sorted
			left - the beginning of the array 
			mid - the middle of the array
			right - the end of the array
	The function will create 2 temporary arrays called Left and Right
	Left compares its value with Right, and depending on the result of the comparison, the main array is matched with the 'lesser' value 
*/
void merge(DataRow ** array, int left, int mid, int right)
{
	//determines whether the data to compare is 'n' numeric or 's' string
	char dataType = array[0]->dataType;

	//create temp array size that split the array into two
	int tempLeftArraySize = mid - left + 1;
	int tempRightArraySize = right - mid;

	//create temporary arrays, since we do not know how big these arrays will be, put them into heap to avoid stackoverflow
	DataRow* tempLeftArray[tempLeftArraySize];
	DataRow* tempRightArray[tempRightArraySize];

	//counters for later use
	int i, j;
	
	//copy items into left array
	for(i = 0; i < tempLeftArraySize; i++)
	{	
		tempLeftArray[i] = array[left+i];
	}

	//copy items into right array
	for(j = 0; j < tempRightArraySize; j++)
	{
		tempRightArray[j] = array[mid+j+1];
	}
	
	//i is index of  leftArray, j is index of rightArray, k is index of original array
	i = 0;
	j = 0;
	int k = left;
	//comparison, compares left array with right array and the smaller value is put into original array
	while(i < tempLeftArraySize && j < tempRightArraySize)
	{
		double comparison;
		//if dataType is numeric, use a number comparison
		if(dataType == 'n')
			comparison = tempLeftArray[i]->dataCompare->numData - tempRightArray[j]->dataCompare->numData;
		//if dataType is string, use lexographical comparison aka strcmp
		else if(dataType == 's')
			comparison = strcmp(tempLeftArray[i]->dataCompare->stringData, tempRightArray[j]->dataCompare->stringData);
		
		//Determines which value to place back into main array
		if(comparison < 0){
			array[k] = tempLeftArray[i];
			i++;
		}
		else{
			array[k] = tempRightArray[j];
			j++;
		}
		k++;
	}
    	
	//copy remaining elements in left array back into the original array
	for(; i < tempLeftArraySize; i++, k++){
	    array[k] = tempLeftArray[i];
	}

    	//copy remaining elements in right array back into the original array
    	for(; j < tempRightArraySize; j++, k++){
	    array[k] = tempRightArray[j];
	}
    
}

/*
	Calls MergeSort algorithm on given array. 
	Parameters: 	array - array to be sorted
			left - beginning of the array to sort
			right - end of the array to sort
	
*/
void mergeSort(DataRow ** array, int left, int right)
{
	int mid;

	//if true, there is more than one element in the array
	if(left < right)
	{		
		mid = (left + right) / 2;
		
		//recursive call to split the array into smaller sub arrays
		mergeSort(array, left, mid);
		mergeSort(array, mid + 1, right);
		
		//once all the arrays have been split, merge the arrays back together and place into correct order
		merge(array, left, mid, right);
	}
	else{
	    return;
	}
}
