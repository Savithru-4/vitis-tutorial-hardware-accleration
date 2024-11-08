#include <chrono>
#include <ctime>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <iostream>
#include <vector>
#include <utility>
#include <random>
#include "xcl2.hpp"
#include "sizes.h"
#include "common.h"

using namespace std;
using namespace std::chrono;

vector<unsigned int, aligned_allocator<unsigned int>> input_doc_words;
vector<unsigned long, aligned_allocator<unsigned long>> profile_weights;
vector<unsigned int, aligned_allocator<unsigned int>> bloom_filter;
vector<unsigned int, aligned_allocator<unsigned int>> starting_doc_id;
vector<unsigned long, aligned_allocator<unsigned long>> fpga_profileScore;
vector<unsigned int, aligned_allocator<unsigned int>> doc_sizes;
vector<unsigned long, aligned_allocator<unsigned long>> cpu_profileScore;

default_random_engine generator;
normal_distribution<double> distribution(3500, 500);

unsigned int total_num_docs;
unsigned size = 0;
unsigned block_size;

unsigned doc_len() {
    unsigned int len = distribution(generator);
    if (len < 100) { len = 100; }
    return len;
}

void setupData() {

    starting_doc_id.resize(total_num_docs);
    fpga_profileScore.resize(total_num_docs);
    cpu_profileScore.resize(total_num_docs);
    doc_sizes.resize(total_num_docs);
    unsigned unpadded_size = 0;

    for (unsigned i = 0; i < total_num_docs; i++) {
        unsigned len_doc = doc_len();
        starting_doc_id[i] = unpadded_size;
        unpadded_size += len_doc;
        doc_sizes[i] = len_doc;
    }

    size = unpadded_size & (~(block_size - 1));
    if (unpadded_size & (block_size - 1)) size += block_size;
    bloom_filter.resize((1L << bloom_size));
    input_doc_words.resize(size);

    printf("Creating documents - total size : %.3f MBytes (%d words)\n", size * sizeof(int) / 1000000.0, size);

    for (unsigned i = 0; i < size; i++) {
        input_doc_words[i] = docTag; // Assuming docTag is defined
    }


    for (unsigned doci = 0; doci < total_num_docs; doci++) {
        unsigned start_dimm1 = starting_doc_id[doci];
        unsigned size_1 = doc_sizes[doci];
        unsigned total_words = 0;
        for (unsigned i = 0; i < size_1; i++) {
            unsigned term = (rand() % ((1L << 24) - 1));
            unsigned freq = (rand() % 254) + 1;
            total_words += freq; // Update the total words count, added not present original
                    if (total_words > size) {
                        // Adjust frequency if needed
                        freq = size - total_words + freq; // Adjust to fit
                    }
            input_doc_words[start_dimm1 + i] = (term << 8) | freq;
        }
    }


    profile_weights.resize((1L << 24));
    fill(bloom_filter.begin(), bloom_filter.end(), 0);
    fill(profile_weights.begin(), profile_weights.end(), 0);

    for (unsigned i = 0; i < 16384; i++) {
        unsigned entry = (rand() % (1 << 24));
        profile_weights[entry] = 10;
        unsigned hash_pu = MurmurHash2(&entry, 3, 1);
        unsigned hash_lu = MurmurHash2(&entry, 3, 5);
        unsigned hash1 = hash_pu & hash_bloom;
        unsigned hash2 = (hash_pu + hash_lu) & hash_bloom;

        bloom_filter[hash1 >> 5] |= 1 << (hash1 & 0x1f);
        bloom_filter[hash2 >> 5] |= 1 << (hash2 & 0x1f);
    }


}

unsigned int MurmurHash2 ( const void * key, int len, unsigned int seed)
{
	// 'm' and 'r' are mixing constants generated offline.
	// They're not really 'magic', they just happen to work well.

	const unsigned int m = 0x5bd1e995;
//	const int r = 24;

	// Initialize the hash to a 'random' value

	unsigned int h = seed ^ len;

	// Mix 4 bytes at a time into the hash

	const unsigned char * data = (const unsigned char *)key;


	switch(len)
	{
	case 3: h ^= data[2] << 16;
	case 2: h ^= data[1] << 8;
	case 1: h ^= data[0];
	        h *= m;
	};

	// Do a few final mixes of the hash to ensure the last few
	// bytes are well-incorporated.

	h ^= h >> 13;
	h *= m;
	h ^= h >> 15;

	return h;
}

int main(int argc, char** argv) {
    int num_iter = 2;
    total_num_docs = 10;

//    switch (argc) {
//        case 2:
//            total_num_docs = atoi(argv[1]);
//            num_iter = 2;
//            break;
//        case 3:
//            total_num_docs = atoi(argv[1]);
//            num_iter = atoi(argv[2]);
//            break;
//        default:
//            cout << "Incorrect number of arguments" << endl;
//            cout << num_iter << endl;
//            return 0;
//    }

//    cout << "Incorrect number of arguments" << endl;
    cout << num_iter << endl;

    cout << "Initializing data" << endl;
    block_size = num_iter * 64;

    cout << block_size << endl;

    setupData();

    runOnCPU(
        doc_sizes.data(),
        input_doc_words.data(),
        bloom_filter.data(),
        profile_weights.data(),
        cpu_profileScore.data(),
        total_num_docs,
        size
    );
    
//    for (long long i=0;i<cpu_profileScore.size();i++)
//    	cout << cpu_profileScore[i] << endl;

    printf("--------------------------------------------------------------------\n");
    cout << "Execution COMPLETE" << endl;

    return 0;
}
