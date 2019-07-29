#define EOF 0

#include <stdio.h>
#include <assert.h>
#include <string>
#include <cstring>
#include <iostream>
#include <fstream>

enum Types_sortings {
    Sort_empty = 0, Sort_bubble, Sort_selection, Sort_heap, Sort_insertion, Num_types_sortings
};

namespace Settings {
    int NMax_for_arrays = 10000;
    int Max_str_req = 18;
}

void build_arr(int nmax, int array[]);
void copy_array(int ar1[], const int ar2[], int n);

void empty_sort(int array[], int* permutations, int* comparisons, int num_elem) {

}

void bubble_sort(int array[], int* permutations, int* comparisons, int num_elem) {
    assert(0 <= num_elem && num_elem <= Settings::NMax_for_arrays);

    int t = -1;
    for (int i = 0; i < num_elem; i++) {
        for (int j = i + 1; j < num_elem; j++) {
            (*comparisons) ++;
            if (array[j] < array[i]) {
                t = array[j];
                array[j] = array[i];
                array[i] = t;
                (*permutations) ++;
            }
        }
    }
}

void selection_sort(int array[], int* permutations, int* comparisons, int num_elem) {
    assert(0 <= num_elem && num_elem <= Settings::NMax_for_arrays);

    int mn = array[0], ind_mn = 0;
    for (int i = 0; i < num_elem; i++) {
        for (int j = i + 1; j < num_elem; j++) {
            (*comparisons) ++;
            if (array[j] < mn) {
                mn = array[j];
                ind_mn = j;
            }
        }
        array[ind_mn] = array[i];
        array[i] = mn;
        (*permutations) ++;
    }

}

void heapify(int array[], int num_elem, int v, int* permutations, int* comparisons)
{
    int largest = v;
    int left_child = 2 * v + 1;
    int right_child = 2 * v + 2;

    int temp = -1;

    if (left_child < num_elem && array[left_child] > array[largest])
        largest = left_child;

    if (right_child < num_elem && array[right_child] > array[largest])
        largest = right_child;

    (*comparisons) += 3;
    if (largest != v)
    {
        temp = array[v];
        array[v] = array[largest];
        array[largest] = temp;
        (*permutations) ++;

        heapify(array, num_elem, largest, permutations, comparisons);
    }
}


void build_heap(int array[], int num_elem, int* permutations, int* comparisons) {
    int ind = num_elem / 2 - 1;

    for (int i = ind; i >= 0; i --) {
        heapify(array, i, 0, permutations, comparisons);
    }

//    for (int i = 0; i < num_elem; i ++) {
//        printf("%i ", array[i]);
//        if (i == num_elem - 1) printf("\n");
//    }
}

void heap_sort(int array[], int* permutations, int* comparisons, int num_elem) {
    build_heap(array, num_elem, permutations, comparisons);

    int temp = -1;
    for (int i = num_elem - 1; i >= 0; i --) {
        temp = array[0];
        array[0] = array[i];
        array[i] = temp;
        (*permutations) ++;

        heapify(array, i, 0, permutations, comparisons);
    }
}

void insertion_sort(int array[], int* permutations, int* comparisons, int num_elem) {
    int key = 0;
    int i = 0;
    for (int j = 1; j < num_elem; j++) {
        key = array[j];
        i = j - 1;
        if (!(i >= 0 && array[i] > key)) (*comparisons) ++;
        while (i >= 0 && array[i] > key) {
            array[i + 1] = array[i];
            i = i - 1;
            array[i + 1] = key;
            (*comparisons) ++;
            (*permutations) ++;
        }
    }
}


struct Request {
    int num_file;
    int num_sort;
    int n_min;
    int n_max;

    void (*func)(int array[], int* permutations, int* comparisons, int num_element);
};

void read_struct(std::string request, Request* req_data,
                 void (*func[])(int array[], int* permutations, int* comparisons, int num_element)) {
    std::string str_now;
    int now = 0;

    for (int i = 0; i < Settings::Max_str_req; i ++) {
        if (request[i] == ' ' || request[i] == '\n' || request[i] == EOF) {
            switch (now) {
                case 0:
                    req_data->num_file = std::stoi(str_now);
                case 1:
                    req_data->num_sort = std::stoi(str_now);
                case 2:
                    req_data->n_min = std::stoi(str_now);
                case 3:
                    req_data->n_max = std::stoi(str_now);
            }
            str_now = "";
            now ++;
        } else {
            str_now += request[i];
        }
    }

    req_data->func = func[req_data->num_sort];
}

void build_arr(int nmax, int array[]) {
    assert(0 <= nmax && nmax <= Settings::NMax_for_arrays);

    for (int i = 0; i < nmax; i++) {
        array[i] = rand();
    }
}

void copy_array(int ar1[], const int ar2[], int n) {
    assert(0 <= n && n <= Settings::NMax_for_arrays);

    for (int i = 0; i < n; i++) {
        ar1[i] = ar2[i];
    }
}

int main() {
    std::ifstream indatas("ClientSort/request.txt");
//    std::ifstream indatas("../../ClientSort/cmake-build-debug/request.txt");
//    std::ifstream indatas("/home/usr/CLionProjects/ClientServ/ClientSort/cmake-build-debug/request.txt");
//    FILE* indatas = fopen("../../ClientSort/cmake-build-debug/request.txt", "r");

    FILE* out_res = fopen("ServSort/result.txt", "w");
//    FILE* out_res = fopen("result.txt", "w");
    setvbuf(out_res, nullptr, _IONBF, 0);

    char* request = static_cast<char *>(calloc(Settings::Max_str_req, sizeof(*request)));
    Request req_data = {};
    int* arr_for_sort = static_cast<int *>(calloc(Settings::NMax_for_arrays, sizeof(*arr_for_sort)));
    int* arr_for_copy= static_cast<int *>(calloc(Settings::NMax_for_arrays, sizeof(*arr_for_sort)));
    int perm = 0;
    int comp = 0;
    void (*funcs[Num_types_sortings])(int array[], int* permutations, int* comparisons, int num_element) = {
            &empty_sort, &bubble_sort, &selection_sort, &heap_sort, &insertion_sort};
    bool* used = static_cast<bool *>(calloc(Num_types_sortings, sizeof(*used)));

    while(1) {
//        std::cout << "1\n";
        indatas.getline(request, Settings::Max_str_req);
//        std::cout << request << std::endl;
        if (std::strlen(request)) {
            read_struct(request, &req_data, funcs);
            if (used[req_data.num_sort]) {
                continue;
            }
            used[req_data.num_sort] = true;
            printf("%s\t%zu\n", request, std::strlen(request));

            build_arr(req_data.n_max, arr_for_sort);

            int cnt = 0;
            for (int num_element = req_data.n_min; num_element <= req_data.n_max; num_element ++) {
                copy_array(arr_for_copy, arr_for_sort, num_element);
                perm = 0;
                comp = 0;

                req_data.func(arr_for_copy, &perm, &comp, num_element);
                cnt += (bool)(perm) + (bool)(comp);
                fprintf(out_res, "%i %i", perm, comp);
                fprintf(out_res, (num_element == req_data.n_max) ? "\n" : " ");
            }
        }
        else {
            indatas.close();
//            fclose(indatas);
//            indatas.open("ClientSort/request.txt");

//            indatas = fopen("../../ClientSort/cmake-build-debug/request.txt", "r");
            indatas.open("ClientSort/request.txt");
//            indatas.open("/home/usr/CLionProjects/ClientServ/ClientSort/cmake-build-debug/request.txt");
        }
    }
    free(request);
    free(arr_for_sort);
    free(arr_for_copy);
    free(used);
    return 0;
}