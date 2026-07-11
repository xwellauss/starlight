#include <starlight/network/server.h>
#include <starlight/utils/logger.h>

#if defined(_PLATFORM_WEB)
#include <emscripten.h>
#include <emscripten/websocket.h>
#include <emscripten/threading.h>
#include <emscripten/posix_socket.h>

static EMSCRIPTEN_WEBSOCKET_T bridgeSocket = 0;
#endif

#include "internal.h"

struct NetworkServer
{
	bool is_init;

	ENetHost* host;
	NetworkEventCallback on_event;
	void* user_data;
};

NetworkServer* network_server_create()
{
	return calloc(1, sizeof(NetworkServer));
}

bool network_server_init(NetworkServer* server, uint16_t port, size_t max_clients, NetworkEventCallback on_event, void* user_data)
{
	ENetAddress address = {0};
	address.host = ENET_HOST_ANY;
	address.port = port;

	server->host = enet_host_create(&address, max_clients, 0, 0, 0);

	if(!server->host)
	{
		log_error("Network: Error in creating server!\n");
		server->is_init = false;
		return false;
	}

	server->on_event = on_event;
	server->user_data = user_data;
	server->is_init = true;

	return true;
}

void network_server_destroy(NetworkServer* server)
{
	if(!server) return;

	if(server->is_init)
	{
		enet_host_destroy(server->host);
	}

	free(server);
}

void network_server_service(NetworkServer* server, uint32_t timeout_ms)
{
	ENetEvent enet_event;

	while(enet_host_service(server->host, &enet_event, timeout_ms) > 0)
	{
		NetworkEvent event = {0};
		event.peer = network_peer_from_enet_peer(enet_event.peer);

		switch(enet_event.type)
		{
			case ENET_EVENT_TYPE_CONNECT:
			{
				event.type = NETWORK_EVENT_CONNECT;
				server->on_event(&event, server->user_data);
				break;
			}
			case ENET_EVENT_TYPE_DISCONNECT:
			{
				event.type = NETWORK_EVENT_DISCONNECT;
				server->on_event(&event, server->user_data);
				break;
			}
			case ENET_EVENT_TYPE_RECEIVE:
			{
				event.type = NETWORK_EVENT_RECEIVE;
				event.channel = enet_event.channelID;
				event.data = enet_event.packet->data;
				event.size = enet_event.packet->dataLength;
				server->on_event(&event, server->user_data);
				enet_packet_destroy(enet_event.packet);
				break;
			}

			case ENET_EVENT_TYPE_DISCONNECT_TIMEOUT:
			case ENET_EVENT_TYPE_NONE: break;
		}
	}
}

void network_server_send(NetworkServer* server, NetworkPeer* peer, uint8_t channel, NetworkSendMode mode, const void* data, size_t size)
{
	if(!server || !server->is_init || !peer) return;

	ENetPacket* packet = enet_packet_create(data, size, network_send_mode_to_enet_flags(mode));
	enet_peer_send(network_peer_to_enet_peer(peer), channel, packet);
}

void network_server_broadcast(NetworkServer* server, uint8_t channel, NetworkSendMode mode, const void* data, size_t size)
{
	if(!server || !server->is_init) return;

	ENetPacket* packet = enet_packet_create(data, size, network_send_mode_to_enet_flags(mode));
	enet_host_broadcast(server->host, channel, packet);
}

void network_server_disconnect(NetworkServer* server, NetworkPeer* peer)
{
	if(!server || !server->is_init || !peer) return;

	enet_peer_disconnect(network_peer_to_enet_peer(peer), 0);
}
