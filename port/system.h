#pragma once

namespace vsun::port {

// Request a "power off" / exit on the current platform.
// On the Win32 simulator, this asks the host loop to quit.
void request_shutdown();

} // namespace vsun::port

