#include "merge_lists.h"

#include "catch2.hpp"

#include <iostream>

namespace lab04
{
	constexpr int left(const int i) {
		return 2 * i + 1;
	}

	constexpr int right(const int i) {
		return 2 * i + 2;
	}

	bool is_empty(const ListT* list)
	{
		if (list && list->first) {
			return false;
		}
		return true;
	}

	NodeT* create_node(int value) 
	{
		// NOLINTNEXTLINE
		auto node = new NodeT;
		node->value = value;
		node->next = nullptr;
		return node;
	}

	ListT* create_list(const int* values, const int n, Operation* op)
	{
		// NOLINTNEXTLINE
		auto* list = new ListT;
		list->first = nullptr;
		list->last = nullptr;

		if (values == nullptr || n == 0) {
			return list; // return empty list
		}

		list->first = create_node(values[0]);
		list->last = list->first;

		for (int i = 1; i < n; i++) {
			insert_last(list, values[i]);
		}

		return list;
	}

	void insert_first(ListT* list, NodeT* node) {
		if (!list || !node) {
			return;
		}

		if (!list->first) {
			list->first = node;
			list->last = node;
			return;
		}

		node->next = list->first; // now second
		list->first = node;
	}

	NodeT* remove_first(ListT* list)
	{
		if (is_empty(list)) {
			return nullptr;
		}

		NodeT* retval = list->first;

		const auto second = list->first->next;
		list->first = second;

		if (list->first == nullptr) {
			list->last = nullptr;
		}

		return retval;
	}

	void insert_last(ListT* list, const int value, Operation* op) {
		if (!list) return;

		if (op) op->count(3);
		NodeT* to_insert = create_node(value); // 3 operations: allocates memory and 2 assignments

		if (op) op->count();
		if (!list->first) {
			list->first = to_insert;
			list->last = to_insert;
			if (op) op->count(2);
			return;
		}

		if (op) op->count(2);
		// NOLINTNEXTLINE
		list->last->next = to_insert;
		list->last = to_insert;
	}

	void insert_last(ListT* list, const NodeT* node, Operation* op) {
		insert_last(list, node->value, op);
	}

	void print_list(const ListT* list)
	{
		if (is_empty(list)) {
			printf("The list is empty!\n");
			return;
		}

		NodeT* tmp = list->first;
		while (tmp) {
			printf("%d ", tmp->value);
			tmp = tmp->next;
		}
		putchar('\n');
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
	ListT** generate_k_sorted_lists(int n, int k, int range_min, int range_max)
	{
		// TODO: Generate k sorted lists of equal size that have the sum of elements equal to n. The lists should be stored in an array.
		const int ratio = n / k;
		int* arr = new int[ratio + n % k];
		// NOLINTNEXTLINE
		ListT** lists = new ListT*[k];
		for (int i = 0; i < k - 1; i++) {
			FillRandomArray(arr, ratio, range_min, range_max, false, ASCENDING);
			// NOLINTNEXTLINE
			lists[i] = create_list(arr, ratio);
		}

		FillRandomArray(arr, ratio + n % k, range_min, range_max, false, ASCENDING);
		// NOLINTNEXTLINE
		lists[k - 1] = create_list(arr, ratio + n % k);
		delete[] arr;
		return lists;
	}

	ListT* merge_2_lists(const ListT* list_A, const ListT* list_B)
	{
		ListT* res = create_list(nullptr);
		NodeT* n1 = list_A->first, *n2 = list_B->first;
		while (n1 != nullptr && n2 != nullptr) {
			if (n1->value < n2->value) {
				insert_last(res, n1);
				n1 = n1->next;
			}
			else {
				insert_last(res, n2);
				n2 = n2->next;
			}
		}

		// one of the lists may be longer
		while (n1 != nullptr) {
			insert_last(res, n1);
			n1 = n1->next;
		}

		while (n2 != nullptr) {
			insert_last(res, n2);
			n2 = n2->next;
		}

		return res;
	}

	void min_heapify(ListT* lists[], int size, int i, Operation *op)
	{
		if (i >= size) {
			return; // if index is out of bounds
		}

		const int lc_idx = left(i);
		const int rc_idx = right(i);


		int min_idx = i;
		if (op && lc_idx < size) op->count(); // count the comparison between values in the next line
		if (lc_idx < size && lists[lc_idx]->first->value < lists[min_idx]->first->value) {
			min_idx = lc_idx;
		}

		if (op && rc_idx < size) op->count();
		if (rc_idx < size && lists[rc_idx]->first->value < lists[min_idx]->first->value) {
			min_idx = rc_idx;
		}

		if (min_idx != i) { // if children are smaller, swap
			std::swap(lists[min_idx], lists[i]);
			if (op) op->count(3); // 3 assignments for swap
			min_heapify(lists, size, min_idx, op);
		}
	}

	void build_heap_bottom_up(ListT* lists[], int size, Operation* op)
	{
		for (int i = size / 2 - 1; i >=0; i--) {
			min_heapify(lists, size, i, op);
		}
	}

	int extract_min(ListT* lists[], int &size, Operation* op) {
		const NodeT* min_node = remove_first(lists[0]);
		if (op) op->count();
		const int min = min_node->value;
		delete min_node; // never leak memory

		if (op) op->count();
		if (lists[0]->first == nullptr) {
			std::swap(lists[0], lists[size - 1]);
			if (op) op->count(3);
			// if we removed the last one from that list, decrease size
			size--;
		}

		if (op) op->count();
		if (size > 0) {
			min_heapify(lists, size, 0, op);
		}

		return min;
	}

	ListT* merge_k_lists(ListT* lists[], int size, Operation* op)
	{
		build_heap_bottom_up(lists, size, op);
		ListT* res = create_list(nullptr, 0, op);
		while (size != 0) {
			if (op) op->count();
			insert_last(res, extract_min(lists, size, op), op);
		}

		if (op) op->count();

		return res;
	}

    void demonstrate(int n, int k)
    {
		printf("Generated %d lists with total number of elements %d.\n", k, n);
        ListT** lists = generate_k_sorted_lists(n, k, 1, 20);
		for (int i = 0; i < k; i++) {
			print_list(lists[i]);
		}

		if (k >= 2) {
			printf("Merge of the first two lists: ");
			ListT* merged = merge_2_lists(lists[0], lists[1]);
			print_list(merged);
			destroy_list(&merged);
		}

		printf("Merge of all lists: ");
		ListT* res = merge_k_lists(lists, k, nullptr);
		print_list(res);
		destroy_list(&res);

		for (int i = 0; i < k; i++) {
			destroy_list(lists + i);
		}
    }

	bool IsListSorted(const ListT* list) {
		if (is_empty(list)) {
			return true;
		}
		NodeT* tmp = list->first;
		while (tmp != nullptr && tmp->next != nullptr) {
			if (tmp->value > tmp->next->value) {
				return false;
			}
			tmp = tmp->next;
		}

		return true;
	}

    TEST_CASE("Merge lists")
    {
		int n = 1000, k = 101;
        ListT** lists = generate_k_sorted_lists(n, k);
		for (int i = 0; i < k; i++) {
			REQUIRE( IsListSorted(lists[i]) );
		}

		ListT* sorted_list = merge_k_lists(lists, k);
		REQUIRE( IsListSorted(sorted_list) );

		int count = 0;
		NodeT* tmp = sorted_list->first;
		while (tmp != nullptr) {
			count++;
			tmp = tmp->next;
		}

		REQUIRE(count == n);

		destroy_list(&sorted_list);
		for (int i = 0; i < k; i++) {
			destroy_list(lists + i);
		}
		delete[] lists;
    }

    void performance(Profiler& profiler, ListsCase whichCase)
    {
        switch (whichCase) {
        case FIXED_K:
	        {
		        std::string names[] = {"merge5Op", "merge10Op", "merge100Op"};
		        for (int k = 0; k < 3; k++) {
		        	for (int times = 0; times < 5; times++) {
		        		for (int n = 100; n <= 10000; n += 100) {
					        int k_vals[] = {5, 10, 100};
					        ListT** lists = generate_k_sorted_lists(n, k_vals[k]);
		        			Operation op = profiler.createOperation(names[k].c_str(), n);

		        			ListT* res = merge_k_lists(lists, k_vals[k], &op);

		        			// cleanup
		        			for (int x = 0; x < k_vals[k]; x++) {
		        				destroy_list(lists + x);
		        			}
							destroy_list(&res);
		        			delete[] lists;
		        		}
		        	}
		        }
        		profiler.divideValues(names[0].c_str(), 5);
        		profiler.divideValues(names[1].c_str(), 5);
        		profiler.divideValues(names[2].c_str(), 5);
        		profiler.createGroup("FIXED_K", names[0].c_str(), names[1].c_str(),names[2].c_str());
		        break;
	        }
        case FIXED_N:
	        {
		        for (int times = 0; times < 5; times++) {
        			for (int k = 10; k < 500; k += 10) {
				        constexpr int n = 10000;
				        ListT** lists = generate_k_sorted_lists(n, k);

						Operation op = profiler.createOperation("mergeKFixed", k);

        				ListT* res = merge_k_lists(lists, k, &op);

        				for (int x = 0; x < k; x++) {
        					destroy_list(lists + x);
        				}
        				destroy_list(&res);
        				delete[] lists;
        			}
        		}
        		profiler.divideValues("mergeKFixed", 5);
        		profiler.createGroup("FIXED_N", "mergeKFixed");
		        break;
	        }
        	default:
	        {
        		printf("No such case is supported!\n");
		        break;
	        }
        }
    }

} // namespace lab04
