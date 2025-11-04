#include "merge_lists.h"

#include "catch2.hpp"

#include <iostream>

namespace lab04
{
	NodeT* create_node(int value) 
	{
		auto node = new NodeT;
		node->value = value;
		node->next = nullptr;
		return node;
	}

	ListT* create_list(int* values, int n) 
	{
		const auto list = new ListT;
		list->first = nullptr;
		list->last = nullptr;

		if (n == 0) {
			return list; // return empty list
		}


		list->first = create_node(values[0]);
		list->last = list->first;

		for (int i = 1; i < n; i++) {
			const auto tmp = create_node(values[i]);
			tmp->next = list->first;
			list->first = tmp;
		}

		return list;
	}

	NodeT* remove_first(ListT* list) 
	{
		if (!list) {
			return nullptr;
		}

		const auto second = list->first->next;
		delete list->first;
		list->first = second;

		if (list->first == nullptr) {
			list->last = nullptr;
		}

		return list->first;
	}

	void insert_last(ListT* list, NodeT* node) {
		if (!list || !list->first) return;
		list->last->next = node;
		list->last = node;
	}

	void print_list(ListT* list)
	{
		if (!list) {
			printf("The list is empty!\n");
			return;
		}

		NodeT* tmp = list->first;
		while (tmp) {
			printf("%d ", tmp->value);
			tmp = tmp->next;
		}
	}

	bool is_empty(const ListT * const list)
	{
		if (list && list->first) {
			return false;
		}
		return true;
	}

	void destroy_list(ListT** list)
	{
		if (!list || !(*list)) {
			return;
		}

		ListT* rlist = *list;
		NodeT* current = rlist->first;
		while (current) {
			NodeT* next = current->next;
			delete current;
			current = next;
		}

		*list = nullptr;
		delete rlist;
	}


	//GENERATION AND MERGING ALGORITHMS
	ListT** generate_k_sorted_lists(int n, int k)
	{
		// TODO: Generate k sorted lists of equal size that have the sum of elements equal to n. The lists should be stored in an array.
		return nullptr;
	}

	ListT* merge_2_lists(ListT* list_A, ListT* list_B)
	{
		// TODO: Merge two sorted lists.
		return nullptr;
	}

	void min_heapify(ListT* lists[], int size, int i, Operation *op)
	{
		// TODO: Apply min-heapify considering the heads of the lists (you should swap the lists pointers based on the heads' values)
	}

	void build_heap_buttomup(ListT* lists[], int size, Operation* op)
	{
		// TODO: Build the heap of lists in a bottom-up manner using min-heapify.
	}

	ListT* merge_k_lists(ListT* lists[], int size, Operation* op)
	{
		// TODO: Merge the k sorted lists.
		return nullptr;
	}

    void demonstrate(int n, int k)
    {
        // TODO: Write a small demo for the list operations, list generation and merging functions.
    }

    TEST_CASE("Merge lists")
    {
        // TODO: Write your unit tests here
        // Use REQUIRE( [condition] ); to perform checks
        // You can use the generate_k_sorted_lists to generate input data
        // and create a IsListSorted method to test the result of your sorting function on the lists.
        // For example:
        //   ListT* lists[100] = generate_k_sorted_lists(1000, 100)
		//	 ListT* sorted_list = merge_k_lists(lists, 100)
		//   for(int i=0; i<100; i++)
        //   	REQUIRE( IsListSorted(lists[i]) );
    }

    void performance(Profiler& profiler, ListsCase whichCase)
    {
        // TODO: Write your performance analysis here, as explained in the readme.
        // You should use profiler.createOperation to pass the counter to each sorting function
    }

} // namespace lab04
