/*Date: 03/03/2020
Class: CS5541
Assignment: Cache Simulator
Author(s): Mohammad Jaminur Islam*/
#include<stdio.h>
#include<getopt.h>
#include<stdlib.h>
#include<math.h>
typedef unsigned long long int memAddress;

int numberOfSetBits,numberOfBlockBits,numberOflinePerset, verbose, hits =0,miss = 0, evict = 0;
char *fileName;
struct line{
    int uses;
    int valid;
    unsigned long long tag;
    char *block;
};
struct set{
    struct line *lines;
};
struct Cache{
    struct set *sets;
};
int isSetFull(struct set set){
    for(int i=0;i<numberOflinePerset;i++){
      //if we find an empty line then the set is not full, so return 1
        if(set.lines[i].valid == 0){
            return 1;
        }
    }
    //no empty lines so the set is full
    return 0;
}
int anyEmptyLines(struct set set){
    struct line line;
    for (int i = 0; i < numberOflinePerset; i++){
        line = set.lines[i];
        //if a line is not valid return its line number
        if(line.valid == 0){
            return i;
        }
    }
    return 0;
}
int findLRU(struct set set){
    int minUses = set.lines[0].uses;
    int LRU = 0;

    //loop through every line in the set, saving the line number of the line with the least uses
    //and its number of uses for comparison with the rest of the set, replacing it if another line
    //is found with less uses
    for(int i = 0; i < numberOflinePerset; i++){
        if(set.lines[i].uses < minUses){
        LRU = i;
        minUses = set.lines[i].uses;
        }
    }
    return LRU;
}

    //returns the line number of the most recently used line (meaning that it belongs in the cache)
int findMRU(struct set set){
    int maxUses = set.lines[0].uses;
    int MRU = 0;

    //loop through every line in the set, saving the line number of the line with the most uses
    //and its number of uses for comparison with the rest of the set, replacing it if another line
    //is found with more uses
    for(int i = 0; i < numberOflinePerset; i++){
        if(set.lines[i].uses > maxUses){
        MRU = i;
        maxUses = set.lines[i].uses;
        }
    }
    return MRU;
}

//Determines if there is a hit
int checkHit(struct line line, memAddress address){
    // printf("Checking line tag = %llx with address = %llx and valid %d\n",line.tag,address,line.valid);
    return line.tag == address && line.valid;
}

void Simulation(struct Cache aCache, 
                memAddress address,
                char instruction,
                int size){
    //calculate the size of the tag by subtratcing the
    //sum of the number of block bits and the number of sets in the cache from 64
    int tagSize = 64-(numberOfBlockBits + numberOfSetBits);
    //calculate the address tag by shifting the address to the right by the sum of the sets and block bits
    // printf("address: %llx %x\n",address,address);
    memAddress addressTag = address >> (numberOfSetBits + numberOfBlockBits);
    //calculate the index number of the set within the cache
    // printf("addressTag: %llx %x\n",addressTag,addressTag);
    unsigned long long setNum = (address << (tagSize)) >> (tagSize + numberOfBlockBits);
    // printf("Address: %llx, addressTag = %llx, setNum = %llx\n",address,addressTag,setNum);
    struct set set = aCache.sets[setNum];
    //will keep track of any hits that happened as we loop through the set
    int hit = 0;
    if(verbose == 1){
        printf( "%c ", instruction );
        printf( "%llx,%d ", address, size );
    }
    for (int i = 0; i < numberOflinePerset; i++){
        struct line line = set.lines[i];
        //there was a hit, increase the hit and usescounter,also change the flag
        // printf("tag = %llx, uses = %d, valid = %d\n",line.tag,line.uses,line.valid);

        if(checkHit(line, addressTag) == 1){
            //increment hits
            hits++;
            hit = 1;
            //increment uses
            if(verbose == 1){
               printf("Hit \n");
            }
            aCache.sets[setNum].lines[i].uses = aCache.sets[setNum].lines[findMRU(set)].uses+1;
            break;
        }
    }

    //if there was a miss and the set is not full than place the miss in the nearest empty line
    if(hit == 0 && isSetFull(set) == 1){
        miss++;
         if(verbose == 1){
               printf("Miss \n");
        }
        //find the next empty line for the miss
        int emptyLine = anyEmptyLines(set);
        //set the address tag
        set.lines[emptyLine].tag = addressTag;
        //mark the line as valid as it now contains something
        set.lines[emptyLine].valid = 1;

        int MRU = findMRU(set);
        //increment uses
        aCache.sets[setNum].lines[emptyLine].uses = aCache.sets[setNum].lines[MRU].uses+1;
    }
    /* otherwise we have to evict */
    else if(hit ==0){
        miss++; //increment misses
        evict++; //increment evictions
        if(verbose == 1){
               printf("Miss Eviction \n");
        }
        //we need to evict the LRU to make space for the miss
        int LRU = findLRU(set);
        //replace the lines tag
        set.lines[LRU].tag = addressTag;

        int mostRecentlyUsed = findMRU(set);
        //increment uses
        aCache.sets[setNum].lines[LRU].uses = aCache.sets[setNum].lines[mostRecentlyUsed].uses+1;
    }
    
}

void readFileAndSimulate(char *fileName,struct Cache cache){
    FILE *file = fopen(fileName, "r");;
    char insType;
    memAddress address;
    int s;

    if (file != NULL) {
        while (fscanf(file, " %c %llx,%d", &insType, &address, &s) == 3) {
            // printf("THis is execution: %c",insType);
            if(insType == 'L'){
                Simulation(cache, address,insType,s);
            }
            else if(insType == 'S'){
                Simulation(cache,  address,insType,s);
            }
            else if(insType == 'M'){
                Simulation(cache, address,insType,s);
                Simulation(cache, address,insType,s);

            }
            else {
                printf("No instruction match here");
                continue;
            }
        }
    }
    fclose(file);
}
struct Cache initCache(long long numberOfsets){
    struct Cache newCache;
    struct set set;
    struct line line;
    // printf("totalSets: %lli, totalLines: %d, blockSize: %d",totalSets,totalLines,blockSize);
    //allocate memory for the new cache based off of how many total sets it will have
    newCache.sets = (struct set*) malloc(sizeof(struct set)* numberOfsets);
    for (int i = 0; i < numberOfsets; i++){
        //alocate memory for each line in each set of the new cache
        set.lines = (struct line*) malloc (sizeof(struct line) * numberOflinePerset);
        //assigns the newly created sets to the cache
        newCache.sets[i] = set;
        //sets the value of every field in each line to 0 and then assigns them to the sets withtin the cache
        for (int j = 0; j < numberOflinePerset; j ++){
            line.uses = 0;
            line.valid = 0;
            line.tag = 0;
            set.lines[j] = line;
        }
    }
    return newCache;
}
int main(int argc, char **argv){
    char c;
    while( (c=getopt(argc,argv,"s:E:b:t:vh")) != -1){
        // printf("%c",c);
        switch(c) {
            case 's':
                numberOfSetBits = atoi(optarg);
                break;
            case 'E':
                numberOflinePerset = atoi(optarg);
                break;
            case 'b':
                numberOfBlockBits = atoi(optarg);
                break;
            case 't':
                fileName = optarg;
                break;
            case 'v':
                verbose = 1;
                break;
            case 'h':
                printf("Check the assignment details about the available options");
                exit(0);
            default:
                printf("Check the assignment details about the available options");
                exit(0);
        }
    }
    // printf("numberOfSetBits = %d, numberofBlockBits = %d, numberOfLineperSet = %d, fileName = %s",numberOfSetBits,numberOfBlockBits,numberOflinePerset,fileName);
    long long numberofSets= pow(2.0, numberOfSetBits);   // get Number of set by 2^s
	// long long blockSize = pow(2.0, numberOfBlockBits);  // get blockSize by 2^b
    struct Cache cache = initCache (numberofSets);
    readFileAndSimulate(fileName,cache);
    for (int i = 0; i < numberofSets; i++){
            struct set set = cache.sets[i];
            if(set.lines != NULL){
                free(set.lines);
            }
    }

    if(cache.sets != NULL){
            free(cache.sets);
    }
    printf("hits = %d, miss = %d, evicts = %d \n",hits,miss,evict);
    return 0;
}