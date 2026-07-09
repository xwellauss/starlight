#pragma once

#include <starlight/network/network.h>

typedef struct NetworkServer NetworkServer;

NetworkServer* network_server_create();
bool network_server_init(NetworkServer* server, uint16_t port, size_t max_clients, NetworkEventCallback on_event, void* user_data);
void network_server_destroy(NetworkServer* server);

void network_server_service(NetworkServer* server, uint32_t timeout_ms);
void network_server_send(NetworkServer* server, NetworkPeer* peer, uint8_t channel, NetworkSendMode mode, const void* data, size_t size);
void network_server_broadcast(NetworkServer* server, uint8_t channel, NetworkSendMode, const void* data, size_t size);
void network_server_disconnect(NetworkServer* server, NetworkPeer* peer);
