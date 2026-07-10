#include "scene_client_test.h"

#include <starlight/core/window/input.h>
#include <starlight/core/ui/ui.h>
#include <starlight/core/ui/style.h>
#include <starlight/audio/audio.h>
#include <starlight/utils/logger.h>
#include <starlight/utils/math_utils.h>
#include <starlight/network/client.h>
#include <starlight/network/serialize.h>

#include <memory.h>

static bool button_clicked = false;
static bool client_connected = false;

static NetworkClient* client = NULL;

static void on_network_event(const NetworkEvent* event, void* user_data)
{
	int p = (int)(intptr_t)user_data;

	switch (event->type)
	{
        case NETWORK_EVENT_CONNECT:
            log_debug("Connected to server\n");
            break;
        case NETWORK_EVENT_DISCONNECT:
            log_debug("Disconnected from Server\n");
            break;
        case NETWORK_EVENT_RECEIVE:
		{
			log_debug("Event Received on Client\n");
			if(event->size == sizeof(int32_t))
			{
				int32_t value;
				memcpy(&value, event->data, sizeof(value));
				log_debug("Value from server: %d\n", value);
			}
            break;
        }
    }
}

static void init()
{
}

static void activate()
{
}

static void update()
{
	if(button_clicked && !client_connected)
	{
		client = network_client_create();
		network_client_init(client, on_network_event, NULL);
		client_connected = network_client_connect(client, "localhost", 8000, 50);

		button_clicked = false;
	}

	if(client_connected)
	{
		network_client_service(client, 50);
	}
}

static void build_ui()
{
	ui_begin_container("Container 1", NULL);
	ui_button("Join Server", &button_clicked);
	ui_end_container();
}

static void render()
{
}

static void process_input()
{
}

static void deactivate()
{
}

static void destroy()
{
	network_client_destroy(client);
}

Scene scene_client_test = {"SceneClient", init, destroy, activate, deactivate, update, render, build_ui, process_input};
