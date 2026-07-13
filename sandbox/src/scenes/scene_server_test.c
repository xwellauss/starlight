#include "scene_server_test.h"

#include <starlight/core/window/input.h>
#include <starlight/core/ui/ui.h>
#include <starlight/core/ui/style.h>
#include <starlight/audio/audio.h>
#include <starlight/utils/logger.h>
#include <starlight/utils/math_utils.h>
#include <starlight/network/server.h>
#include <starlight/network/serialize.h>
#include <stdint.h>

static bool button_clicked = false;
static bool server_started = false;

static NetworkServer* server = NULL;

static void on_network_event(const NetworkEvent* event, void* user_data)
{
	int p = (int)(intptr_t)user_data;

	switch (event->type)
	{
        case NETWORK_EVENT_CONNECT:
            log_debug("Connected to client\n");

            NetworkByte buffer[256];
            NetworkWriter writer;
            network_writer_init(&writer, buffer, sizeof(buffer));

            network_write_string(&writer, "xwellauss");
            network_write_vec2(&writer, (vec2s){1.0f, 4.6f});
            network_write_vec3(&writer, (vec3s){1.0f, 4.6f, 9.4f});
            network_write_vec4(&writer, (vec4s){1.0f, 4.6f, 8.55f, 9.67767f});

            network_server_send(server, event->peer, 0, NETWORK_MODE_RELIABLE, writer.buffer, writer.cursor);
            break;
        case NETWORK_EVENT_DISCONNECT:
            log_debug("Client Disconnected\n");
            break;
        case NETWORK_EVENT_RECEIVE:
		{
			log_debug("Event Received on Server\n");
            break;
        }
    }
}

static void init()
{
	server = network_server_create();
}

static void activate()
{
}

static void update()
{
	if(button_clicked && !server_started)
	{
		server_started = network_server_init(server, 8000, 16, on_network_event, NULL);
	}

	if(server_started)
	{
		network_server_service(server, 50);
	}
}

static void build_ui()
{
	ui_container_begin("Container 1", NULL);
	ui_widget_button("Start Server", &button_clicked);
	ui_container_end();
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
	network_server_destroy(server);
}

Scene scene_server_test = {"SceneServer", init, destroy, activate, deactivate, update, render, build_ui, process_input};
