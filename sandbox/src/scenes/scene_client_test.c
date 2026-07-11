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
#include <stdint.h>

static bool button_clicked = false;
static bool client_connected = false;

static NetworkClient* client = NULL;

static void on_network_event(const NetworkEvent* event, void* user_data)
{
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

			NetworkReader reader = {.buffer=event->data, .capacity=event->size, .cursor=0};
			char name[64];
			network_read_string(&reader, name, sizeof(name));
			log_debug("name: %s\n", name);

            break;
        }
    }
}

static void init()
{
	client = network_client_create();
}

static void activate()
{
}

static void update()
{
	if(button_clicked && !client_connected)
	{
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
