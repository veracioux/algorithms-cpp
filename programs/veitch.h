#pragma once

#include "program.h"
#include "minimize.h"

namespace veitch
{

int veitch(const arglist_iter &begin, const arglist_iter &end);

const char *help_synopsis();

const char *help_options();

const char *help_examples();

void help();

}
