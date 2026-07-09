#pragma once

#include <starlight/network/network.h>

typedef struct NetworkClient NetworkClient;

NetworkClient* network_client_create();
bool network_client_init(NetworkClient* client, NetworkEventCallback on_event, void* user_data);
void network_client_destroy(NetworkClient* client);
bool network_client_connect(NetworkClient* client, const char* host, uint16_t port, uint32_t timeout_ms);
void network_client_disconnect(NetworkClient* client);
void network_client_service(NetworkClient* client, uint32_t timeout_ms);
void network_client_send(NetworkClient* client, uint8_t channel, NetworkSendMode mode, const void* data, size_t size);
