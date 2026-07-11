#include <starlight/network/client.h>
#include <starlight/utils/logger.h>

#if defined(_PLATFORM_WEB)
#include <emscripten.h>
#include <emscripten/websocket.h>
#include <emscripten/threading.h>
#include <emscripten/posix_socket.h>

static EMSCRIPTEN_WEBSOCKET_T bridgeSocket = 0;
#endif

#include "internal.h"

struct NetworkClient
{
	bool is_init;

	ENetHost* host;
	ENetPeer* peer;
	NetworkEventCallback on_event;
	void* user_data;
};

NetworkClient* network_client_create()
{
	return calloc(1, sizeof(NetworkClient));
}

bool network_client_init(NetworkClient* client, NetworkEventCallback on_event, void* user_data)
{
	client->host = enet_host_create(NULL, 1, 0, 0, 0);

	if(!client->host)
	{
		log_error("Network: Error in creating client!\n");
		client->is_init = false;
		return false;
	}

	client->peer = NULL;
	client->on_event = on_event;
	client->user_data = user_data;
	client->is_init = true;

	return true;
}

void network_client_destroy(NetworkClient* client)
{
	if(!client) return;

	if(client->is_init)
	{

		network_client_disconnect(client);
		enet_host_destroy(client->host);
	}

	free(client);
}

bool network_client_connect(NetworkClient* client, const char* host, uint16_t port, uint32_t timeout_ms)
{
	ENetAddress address = {0};
	enet_address_set_host(&address, host);
	address.port = port;

	client->peer = enet_host_connect(client->host, &address, 2, 0);
	if(!client->peer)
	{
		log_error("Network: Client could not connect to host!\n");
		return false;
	}

	return true;
}

void network_client_disconnect(NetworkClient* client)
{
	if(!client || !client->is_init || !client->peer) return;
	enet_peer_disconnect(client->peer, 0);
}

void network_client_service(NetworkClient* client, uint32_t timeout_ms)
{
	ENetEvent enet_event;

	while(enet_host_service(client->host, &enet_event, timeout_ms) > 0)
	{
		NetworkEvent event = {0};
		event.peer = network_peer_from_enet_peer(enet_event.peer);

		switch(enet_event.type)
		{
			case ENET_EVENT_TYPE_CONNECT:
			{
				event.type = NETWORK_EVENT_CONNECT;
				client->on_event(&event, client->user_data);
				break;
			}
			case ENET_EVENT_TYPE_DISCONNECT:
			{
				event.type = NETWORK_EVENT_DISCONNECT;
				client->on_event(&event, client->user_data);
				client->peer = NULL;
				break;
			}
			case ENET_EVENT_TYPE_RECEIVE:
			{
				event.type = NETWORK_EVENT_RECEIVE;
				event.channel = enet_event.channelID;
				event.data = enet_event.packet->data;
				event.size = enet_event.packet->dataLength;
				client->on_event(&event, client->user_data);
				enet_packet_destroy(enet_event.packet);
				break;
			}

			case ENET_EVENT_TYPE_DISCONNECT_TIMEOUT:
			case ENET_EVENT_TYPE_NONE: break;
		}
	}
}

void network_client_send(NetworkClient* client, uint8_t channel, NetworkSendMode mode, const void* data, size_t size)
{
	if(!client || !client->is_init || !client->peer) return;

	ENetPacket* packet = enet_packet_create(data, size, network_send_mode_to_enet_flags(mode));
	enet_peer_send(client->peer, channel, packet);
}
