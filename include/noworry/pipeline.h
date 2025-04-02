#ifndef PIPELINE_H
#define PIPELINE_H

#include "model.h"

class Pipeline
{
public:
    virtual void enqueue(Model& model) = 0;

private:
};

#endif
