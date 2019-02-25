#include "Log.hpp"

Log::Log()
: CurrentLevel(Level::Trace)
, CutoffLevel(Level::Trace)
, FancyLevels{"   [Trace] ", " [Warning] ", "   [Error] "} {
}
