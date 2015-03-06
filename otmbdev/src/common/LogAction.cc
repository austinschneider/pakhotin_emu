#include "emu/otmbdev/LogAction.h"

namespace emu { namespace otmbdev {

    LogAction::LogAction(emu::pc::Crate * crate)
      : Action(crate)
    {
      /* ... nothing to see here ... */
    }

    void LogAction::respond(xgi::Input * in, std::ostringstream & out) {
      XCEPT_RAISE( xcept::Exception, "Don't use LogActions as Actions." );
    }
  }
}
