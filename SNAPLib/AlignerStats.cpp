/*++

Module Name:

    AlignerStats.cpp

Abstract:

    Common statistics for running single & paired alignment.

Authors:

    Ravi Pandya, May, 2012

Environment:
`
    User mode service.

Revision History:

    Integrated from SingleAligner.cpp & PairedAligner.cpp

--*/

#include "stdafx.h"
#include "options.h"
#include "AlignerStats.h"

AbstractStats::~AbstractStats()
{}

AlignerStats::AlignerStats(AbstractStats* i_extra)
:
    totalReads(0),
    usefulReads(0),
    singleHits(0), 
    multiHits(0),
    notFound(0),
    errors(0),
    alignedAsPairs(0),
    extra(i_extra)
{
    for (int i = 0; i <= AlignerStats::maxMapq; i++) {
        mapqHistogram[i] = 0;
        mapqErrors[i] = 0;
    }

    for (int i = 0; i < maxMaxHits; i++) {
        countOfBestHitsByWeightDepth[i] = 0;
        countOfAllHitsByWeightDepth[i] = 0;
        probabilityMassByWeightDepth[i] = 0;
    }

    threadEntry = &localThreadEntry;
    threadEntry->next = NULL;
    threadEntry->nReads = 0;
    threadEntry->threadId = 0;
    threadEntry->lvCalls = 0;
}

AlignerStats::~AlignerStats()
{
    if (extra != NULL) {
        delete extra;
    }
}

    void
AlignerStats::printHistograms(
    FILE* out)
{
    // nothing
    if (extra != NULL) {
        extra->printHistograms(out);
    }
}

    void
AlignerStats::add(
    const AbstractStats* i_other)
{
    AlignerStats* other = (AlignerStats*) i_other;
    totalReads += other->totalReads;
    usefulReads += other->usefulReads;
    singleHits += other->singleHits;
    multiHits += other->multiHits;
    notFound += other->notFound;
    errors += other->errors;
    alignedAsPairs += other->alignedAsPairs;

    if (extra != NULL && other->extra != NULL) {
        extra->add(other->extra);
    }

    for (int i = 0; i <= AlignerStats::maxMapq; i++) {
        mapqHistogram[i] += other->mapqHistogram[i];
        mapqErrors[i] += other->mapqErrors[i];
    }
    for (int i = 0; i < maxMaxHits; i++) {
        countOfBestHitsByWeightDepth[i] += other->countOfBestHitsByWeightDepth[i];
        countOfAllHitsByWeightDepth[i] += other->countOfAllHitsByWeightDepth[i];
        probabilityMassByWeightDepth[i] = other->probabilityMassByWeightDepth[i];
    }

    // NB: This isn't particularly robust.  It knows that the calling pattern is common->add(perThread)
    ThreadPerfEntry *newEntry = new ThreadPerfEntry;
    *newEntry = *other->threadEntry;
    if (threadEntry->threadId != 0) {
        // This is just broken.  Let it be for now._ASSERT(threadEntry->next == NULL);
        newEntry->next = threadEntry;
    }
    threadEntry = newEntry;
}
