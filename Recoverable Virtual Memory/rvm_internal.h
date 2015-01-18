#ifndef RVMINTERNAL_H
#define RVMINTERNAL_H

#include <errno.h>
#include <map>
#include <list>
#include <fstream>
#include <sys/stat.h>
#include <errno.h>
#include <cstdlib>
#include <stdio.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <fstream>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/lexical_cast.hpp>

using namespace std;

typedef int trans_t;

typedef struct log_t
{
        int tid;
        char * segname;
        int offset;
         int size;
        char * data;

}log;

typedef struct regionInfo{
	void* segBase;
	int offset;
	int size;
	char* undoCopyBuffer;
} region;


typedef struct segmentInfo{
        const char* segName;
	void* segBase;
	size_t segSize;
	int isUnderTransaction;
	trans_t transactionId;
} segment;

typedef struct transactionInfo{
	trans_t transactionId;
	int numOfSegments;
	void** segBases;
	list<region*> regionsList;
} transaction;
	

typedef struct rvmInfo{
	char* rvmDirectory;
	list<segment*> mappedSegmentList;
	int currentTransactionId;
	list<transaction*> transactionList;
}rvm_type;


typedef rvm_type * rvm_t;

extern map<int,rvm_t> transIdToRvmMap;
extern map<void*,const char*> segBaseToSegNameMap;

segment* createNewSegmentNode(const char*,void*,int);
transaction* createNewTransactionNode(rvm_t, int , void**);
region* createNewRegionNode(void*, int, int);
char* getAbsFilePath(char *,const char *);
void flushRedoRecords(ofstream& ,int,const char*,int,int,char *);
void updateExternalDataSegment(string,int,int,string);
int getFileSize(char*);
void updateIsUnderTransaction(transaction*, rvm_t);
#endif
