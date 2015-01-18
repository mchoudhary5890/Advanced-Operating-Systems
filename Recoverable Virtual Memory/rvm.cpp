#include "rvm.h"


map<int,rvm_t> transIdToRvmMap;
map<void*,const char *> segBaseToSegNameMap;
char * redoLogFile;  
const char * transBeginMarker = "TRANSACTION START:";
const char * transEndMarker = "TRANSACTION END";
const char * regStartMarker = "REGION START:";
const char * regEndMarker = "REGION END";
const char * segNameMarker = "SEGNAME:";
const char * offSetMarker = "OFFSET:";
const char * sizeMarker = "SIZE:";
const char * dataMarker = "DATA:";


rvm_t rvm_init(const char *directory)
{
        if(mkdir(directory,S_IRWXU|S_IRWXG|S_IRWXO)==-1)
        {
            if(errno != EEXIST)
            {
            	printf("Error in directory formation __rvm_init__");
                exit(0);
            }            
        }
        rvm_t rvm = new rvm_type;
        if(NULL==rvm)
        {
        	printf("Error while allocating memory for rvm");
                exit(0);
        }
        rvm->rvmDirectory = (char*)malloc(strlen(directory)+1);  
        rvm->currentTransactionId = 1;
	if(NULL==rvm->rvmDirectory)
        {
        	printf("Error while allocating memory for rvm directory");
                exit(0);
        }
        strncpy(rvm->rvmDirectory,directory,strlen(directory));
	redoLogFile = getAbsFilePath(rvm->rvmDirectory, "redo.log");
	return rvm;
}

void* rvm_map(rvm_t rvm, const char *segname, int size_to_create)
{
      	FILE* fd;
      	void* segBase=NULL;
      	segment* newSegment=NULL;
   /*	if ( !exists(rvm->rvmDirectory))
	{
		printf("The backing store doesn't Exist. Try calling __init__ first.\n");
		exit(1);
	} */ 
      	rvm_truncate_log(rvm);
       	for(list<segment*>::iterator segmentListItr = rvm->mappedSegmentList.begin();segmentListItr!=rvm->mappedSegmentList.end();segmentListItr++)
       	{
                if((*segmentListItr)->segName==segname)
                {
                  	printf("Segment Already Mapped. Attempt to map the same segment detected.\n"); 
                  	exit(1);
                }
         }
	
      	char* filePath = getAbsFilePath(rvm->rvmDirectory,segname); 
      	fd = fopen(filePath,"ab+"); 
      	if (fd == NULL) 
      	{
      		printf("Error opening file for reading %d  %s \n",errno,filePath);
      		exit(1);
      	}
      	fseek(fd,0,SEEK_END); 
     	long fsize = ftell(fd);
     	if(size_to_create < fsize)
      		size_to_create=fsize;
     	fseek(fd,0,SEEK_SET);
 
     	segBase = (char*)malloc(size_to_create);
	if(NULL==segBase)
	{
		printf("Failure in allocation of memory for segBase in rvm_map.\n");
		exit(1);
	}
     	if(fsize!=0)
	{
     		fread(segBase,fsize,1,fd);
	} 
     	fclose(fd);
      	free(filePath);
      	segBaseToSegNameMap[segBase]=segname;
      	newSegment = createNewSegmentNode(segname,segBase,size_to_create);
      	rvm->mappedSegmentList.push_back(newSegment);
      	return segBase;
}

void rvm_unmap(rvm_t rvm, void *segbase)
{
        for(list<segment*>::iterator segmentListItr = rvm->mappedSegmentList.begin();segmentListItr!=rvm->mappedSegmentList.end();segmentListItr++)
        {
        	if((*segmentListItr)->segBase==segbase)
                {
			{
                                free(segbase);
				free(*segmentListItr);
				rvm->mappedSegmentList.erase(segmentListItr);
                                segBaseToSegNameMap.erase(segbase);
				break;
			}	
                }
        }
}

trans_t rvm_begin_trans(rvm_t rvm, int numsegs, void **segbases)
{
	int iterator, foundSegCount=0;
	for(iterator=0; iterator<numsegs;iterator++)
	{
		for(list<segment*>::iterator segmentListItr = rvm->mappedSegmentList.begin();segmentListItr!=rvm->mappedSegmentList.end();segmentListItr++)
		{
			if((*segmentListItr)->segBase==segbases[iterator])
			{
				if((*segmentListItr)->isUnderTransaction)
				{
					//printf("One of the segments is already under transaction.\n");
					return (trans_t)-1;
				}
				else
				{
					foundSegCount++;
					(*segmentListItr)->isUnderTransaction = 1;
					break;
				}
			}
		}		
	}
	if(foundSegCount!=numsegs)
	{
		printf("The actual count of segbases doesn't match the numsegs value.\n");
		exit(1);
	}
	transaction* newTransaction = createNewTransactionNode(rvm, numsegs, segbases);
	rvm->transactionList.push_back(newTransaction);
        transIdToRvmMap[newTransaction->transactionId]=rvm; 
	return newTransaction->transactionId;
}

void rvm_about_to_modify(trans_t tid, void *segbase, int offset, int size)
{
	if(tid==-1)
	{
		printf("tid passed is -1. Probably call to the trans_begin has failed.\n");
		exit(1);
	}
	if (segBaseToSegNameMap.find(segbase) == segBaseToSegNameMap.end())
	{
		printf("Wrong segbase passed.\n");
		exit(1);
	}
        rvm_t rvm = transIdToRvmMap[tid];
	for(list<transaction*>::iterator transactionListItr = rvm->transactionList.begin(); transactionListItr!=rvm->transactionList.end();transactionListItr++)
	{
		if((*transactionListItr)->transactionId==tid)
                {
			(*transactionListItr)->regionsList.push_back(createNewRegionNode(segbase, offset, size));
			break;
		}
		else
		{
			printf("Wrong transaction Id.\n");
			return;
		}
	}
}

void rvm_abort_trans(trans_t tid)
{
        int flag = 0;
        rvm_t rvm = transIdToRvmMap[tid];
	transIdToRvmMap.erase(tid);
	for(list<transaction*>::iterator transactionListItr = rvm->transactionList.begin(); transactionListItr!=rvm->transactionList.end();transactionListItr++)
        {
                if((*transactionListItr)->transactionId==tid)
                {
			//for(list<region*>::iterator regionsListItr = (*transactionListItr)->regionsList.begin(); regionsListItr!=(*transactionListItr)->regionsList.end();regionsListItr++)
			for(list<region*>::reverse_iterator regionsListItr = (*transactionListItr)->regionsList.rbegin(); regionsListItr!=(*transactionListItr)->regionsList.rend();++regionsListItr)
			{
				memcpy((char*)((*regionsListItr)->segBase)+(*regionsListItr)->offset, (*regionsListItr)->undoCopyBuffer, (*regionsListItr)->size);
				free((*regionsListItr)->undoCopyBuffer);
				delete (*regionsListItr);
			}
			(*transactionListItr)->regionsList.clear();
			updateIsUnderTransaction(*transactionListItr, rvm);
            		flag =1;
			delete (*transactionListItr);
			rvm->transactionList.erase(transactionListItr);
            		break;
		}

	}
        if(flag==0)
        {
                printf("Wrong Transaction Id");
		exit(1);
        }
}


void rvm_commit_trans(trans_t tid)
{
	int flag = 0;
	rvm_t rvm = transIdToRvmMap[tid];
	transIdToRvmMap.erase(tid);
        for(list<transaction*>::iterator transactionListItr = rvm->transactionList.begin(); transactionListItr!=rvm->transactionList.end();transactionListItr++)
        {
                if((*transactionListItr)->transactionId==tid)
                {
                        ofstream redoLogFd;
                        redoLogFd.open (redoLogFile,ios::out|ios::in|ios::app);
                        redoLogFd << transBeginMarker << tid << "\n";
                        for(list<region*>::iterator regionsListItr = (*transactionListItr)->regionsList.begin(); regionsListItr!=(*transactionListItr)->regionsList.end();regionsListItr++)
                        {
                                char * data = (char*)malloc((*regionsListItr)->size +1); 
                                memcpy(data,((char*)((*regionsListItr)->segBase))+((*regionsListItr)->offset),(*regionsListItr)->size);
                                data[(*regionsListItr)->size]='\0'; 
                        	flushRedoRecords(redoLogFd,tid,segBaseToSegNameMap[(*regionsListItr)->segBase],(*regionsListItr)->offset,(*regionsListItr)->size,data);
                                free(data); 
				free((*regionsListItr)->undoCopyBuffer);
				delete (*regionsListItr);
                        }
                        redoLogFd << transEndMarker << "\n";
                        redoLogFd.close();	
			(*transactionListItr)->regionsList.clear();
			updateIsUnderTransaction(*transactionListItr, rvm);
            		flag =1;
			delete (*transactionListItr);
			rvm->transactionList.erase(transactionListItr);
			break;
                }
        }
	if(!flag)
	{
		printf("Wrong tid passed to commit.\n");
		exit(1);
	}
}

void rvm_truncate_log(rvm_t rvm)
{
	ifstream infile(redoLogFile);
	if(!infile.good())
	{ 
		return;
	}
	ifstream redoLogFd;
	redoLogFd.open (redoLogFile,ios::out|ios::in);
	string lineBuffer,currentSegName,currentData;
	int currentOffset,currentSize;

	while(getline(redoLogFd,lineBuffer))
	{
    	    if (boost::starts_with(lineBuffer, regStartMarker))
    	    {	
        	getline(redoLogFd,lineBuffer);
        	if(boost::starts_with(lineBuffer, segNameMarker))
        	{
                	getline(redoLogFd,lineBuffer);
        		currentSegName = lineBuffer;
        	}
        	getline(redoLogFd,lineBuffer);
        	if(boost::starts_with(lineBuffer, offSetMarker))
        	{
                	getline(redoLogFd,lineBuffer);
                	istringstream iss(lineBuffer);
                	currentOffset = 0;
                	iss >> currentOffset;
        	}
        	getline(redoLogFd,lineBuffer);
        	if(boost::starts_with(lineBuffer, sizeMarker))
        	{
                	getline(redoLogFd,lineBuffer);
                	istringstream iss(lineBuffer);
                	currentSize = 0;
                	iss >> currentSize;
        	}
        	getline(redoLogFd,lineBuffer);
        	if(boost::starts_with(lineBuffer, dataMarker))
        	{
                	getline(redoLogFd,lineBuffer);
                 	currentData = lineBuffer;
        	}
        	getline(redoLogFd,lineBuffer);
        	if(boost::starts_with(lineBuffer, regEndMarker))
        	{
                        char* filePath = getAbsFilePath(rvm->rvmDirectory,currentSegName.c_str());
                	updateExternalDataSegment(filePath, currentOffset, currentSize, currentData);
                	continue;
       		}
    	   }
	}
	redoLogFd.close();
       	if( remove(redoLogFile) != 0 )
		cout << "Error in deleting the redoLog file\n";
}


void rvm_destroy(rvm_t rvm, const char *segname)
{
       	for(list<segment*>::iterator segmentListItr = rvm->mappedSegmentList.begin(); segmentListItr!=rvm->mappedSegmentList.end();segmentListItr++)
       	{
                if((*segmentListItr)->segName==segname)
                {
                  	printf("Segment is Mapped. Can't destroy the segment.\n"); 
                  	exit(1);
                }
        }	
	char* filePath = getAbsFilePath(rvm->rvmDirectory,segname);
        ifstream infile(filePath);
        if(!infile.good())
        {
                return;
        }

	if( remove(filePath) != 0 )
		cout << "Error in destroying segment\n";
}	


segment* createNewSegmentNode(const char* segname,void* segBase,int segSize)
{
	segment* newSegment = (segment*)malloc(sizeof(segment));
        if(NULL == newSegment)
        {
        	printf("Memory allocation failed at __createNewSegmentNode__\n");
                exit(0);
        }
        newSegment->segName = segname;
        newSegment->segBase = segBase;
        newSegment->segSize = segSize;
        newSegment->isUnderTransaction = 0;
        newSegment->transactionId = 0;
        return newSegment; 
}

transaction* createNewTransactionNode(rvm_t rvm, int numOfSegments, void** segBases)
{
	transaction* newTransaction = new transaction;
	newTransaction->transactionId = rvm->currentTransactionId++;
        newTransaction->numOfSegments = numOfSegments;
        newTransaction->segBases = segBases;
	return newTransaction;
}

region* createNewRegionNode(void* segBase, int offset, int size)
{	
	region* newRegion = new region;
        newRegion->segBase = segBase;
        newRegion->offset = offset;
        newRegion->size = size;
        newRegion->undoCopyBuffer = (char *)malloc(size+1);
	memcpy(newRegion->undoCopyBuffer, (char*)segBase+offset, size);
	newRegion->undoCopyBuffer[size] = '\0';
	return newRegion;
}

void updateExternalDataSegment(string currentSegName, int currentOffset, int currentSize, string currentData)
{
        int currFileSize=getFileSize((char*)currentSegName.c_str());
        int fd;
        int size_to_create = (currFileSize > (currentOffset + currentData.size())) ?currFileSize :(currentOffset + currentData.size());
        void * segBase = NULL;
        fd = open((char*)currentSegName.c_str(),O_RDWR,S_IRWXU|S_IRWXG|S_IRWXO);
         if (fd == -1)
         {
           printf("Error opening file for reading\n");
           exit(0);
         }
         if((currFileSize < (currentOffset + currentData.size())))
         {
         	if (lseek (fd, currentOffset + currentData.size() -1, SEEK_SET) == -1)
         		printf ("lseek error");
         	/* write a dummy byte at the last location */
         	if (write (fd,"", 1) != 1)
         		printf ("write error");
         }

        segBase = mmap(0,size_to_create , PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
        memcpy((char*)segBase+currentOffset,currentData.c_str(),currentData.size());
        close(fd);
}

void flushRedoRecords(ofstream& redoLogFd ,int tid,const char* segName,int offset,int size,char * data)
{
redoLogFd << regStartMarker << "\n";
redoLogFd << segNameMarker <<"\n" <<segName << "\n";
redoLogFd <<offSetMarker<< "\n" <<offset<< "\n";
redoLogFd <<sizeMarker << "\n" << size<< "\n";
redoLogFd <<dataMarker << "\n" <<data<< "\n";
redoLogFd << regEndMarker << "\n";
}


char* getAbsFilePath(char * directory,const char * segName)
{
      char* filePath = (char*)malloc(strlen(directory)+strlen(segName)+strlen("/")+1);
      strcpy(filePath,directory);
      strcat(filePath,"/");
      strcat(filePath,segName);
      return filePath;
}

int getFileSize( char * fileName)
{
	FILE * fd;
	fd = fopen(fileName,"ab+");
	if (fd == NULL)
        {
                printf("Error opening file for reading %d  %s \n",errno,fileName);
                exit(1);
        }
	fseek(fd,0,SEEK_END);
    	int fsize;
    	fsize = ftell(fd);
        fclose(fd);
        return fsize;
}


void updateIsUnderTransaction(transaction* transPtr, rvm_t rvm)
{
  	int iterator;
        for(iterator=0; iterator<transPtr->numOfSegments;iterator++)
        {
                for(list<segment*>::iterator segmentListItr = rvm->mappedSegmentList.begin();segmentListItr!=rvm->mappedSegmentList.end();segmentListItr++)
                {
                        if((*segmentListItr)->segBase==transPtr->segBases[iterator])
                        {
                              (*segmentListItr)->isUnderTransaction = 0;
                              break;    
                        }
                }
        }
}
		
	
