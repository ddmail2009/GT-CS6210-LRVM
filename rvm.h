#ifndef __LIBRVM__
#define __LIBRVM__

#include "rvm_internal.h"
#include "steque.h"
#include "seqsrchst.h"

#include <stdio.h>

#define __FILE_PREPEND "__RVM_"

typedef int trans_t;

typedef struct{
    int segID;
    int offset;
    int length;
    void *mem;
} log_t;

typedef struct {
    trans_t id;
    int numsegs;
    void **segbases;
    steque_t log;
} RVM_transaction;

typedef struct{
    char *directory; //directory name

    trans_t transID; // last transaction ID
    steque_t transaction; // transaction stack
    seqsrchst_t segnameMap; // (key, value) = (segbase, segname) map
    seqsrchst_t segbaseMap; // (key, value) = (segname, segbase) map
    seqsrchst_t dirtyMap;
    
    unsigned long int log_id;
    unsigned long int log_id_min;
    FILE *log_file;
} RecoverableVM;

typedef RecoverableVM* rvm_t;


/* Formal API */
rvm_t rvm_init(const char *directory);
void *rvm_map(rvm_t rvm, const char *segname, int size_to_create);
void rvm_unmap(rvm_t rvm, void *segbase);
void rvm_destroy(rvm_t rvm, const char *segname);
trans_t rvm_begin_trans(rvm_t rvm, int numsegs, void **segbases);
void rvm_about_to_modify(trans_t tid, void *segbase, int offset, int size);
void rvm_commit_trans(trans_t tid);
void rvm_abort_trans(trans_t tid);
void rvm_truncate_log(rvm_t rvm);

////////////////////////////////////////////////////////////////////
/* Utility function */
void rvm_destructor(rvm_t rvm);


RVM_transaction *getTransaction(trans_t tid);
void freeTransaction(RVM_transaction* transaction);
void commitTransaction(RVM_transaction* transaction, const int index, const int max, FILE* logFile);
void truncateLog(rvm_t rvm, unsigned long int id);

const void *getSegbase(const char* segname);
const char *getSegname(void* segbase);
void removeMapping(void* segbase);

int isDirty(rvm_t rvm, const char *segname);
void setDirty(rvm_t rvm, const char *segname, int dirty);
void clearAllDirty(rvm_t rvm);
void resetLog(rvm_t rvm_);
#endif
