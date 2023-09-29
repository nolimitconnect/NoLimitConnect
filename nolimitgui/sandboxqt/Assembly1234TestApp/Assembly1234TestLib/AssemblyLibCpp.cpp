#include "AssemblyLib.h"

#include <QDebug>

const char* getCppText()
{
    qWarning() << "getCppText was called";
    return "This Is Cpp Text";
}
