#include <starlight/network/network.h>
#include <starlight/utils/logger.h>

#if defined(_PLATFORM_WEB)
#include <emscripten.h>
#include <emscripten/websocket.h>
#include <emscripten/threading.h>
#include <emscripten/posix_socket.h>

static EMSCRIPTEN_WEBSOCKET_T bridgeSocket = 0;
#endif

#include "internal.h"

bool network_init()
{
	if(enet_initialize() != 0)
    {
        log_error("Network: An error occurred while initializing ENet.\n");

		return false;
    }

	return true;
}

void network_destroy()
{
	enet_deinitialize();
}

