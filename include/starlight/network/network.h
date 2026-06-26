#pragma once

#include <enet.h>

typedef struct
{
	ENetHost* host;

	ENetPeer* peer;
	ENetEvent* event;
} Network_Client;

typedef struct
{
	ENetHost* host;

	Network_Client client;
} Network_Server;

void network_init();
void network_destroy();

void network_disconnect_peer(ENetPeer* peer);

void network_send_packet(ENetHost* host, ENetPeer* peer, int channel, void* data, size_t packet_size);
void network_send_int_packet(ENetHost* host, ENetPeer* peer, int channel, int data);
void network_broadcast_packet(ENetHost* host, int channel_id, ENetPacket* packet);

void network_init_server(Network_Server* server, int port, int max_clients, int channels);
void network_destroy_server(Network_Server* server);

void network_init_client(Network_Client* client, int channels);
void network_client_connect_peer(Network_Client* client, const char* address_str, int port, int channels, ENetPeer** peer, ENetEvent* event);
void network_destroy_client(Network_Client* client);
