#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

typedef enum
{
	NETWORK_EVENT_CONNECT,
	NETWORK_EVENT_DISCONNECT,
	NETWORK_EVENT_RECEIVE
} NetworkEventType;

typedef enum
{
	NETWORK_MODE_RELIABLE,
	NETWORK_MODE_UNRELIABLE,
	NETWORK_MODE_UNRELIABLE_FRAGMENTED,
	NETWORK_MODE_UNSEQUENCED,
} NetworkSendMode;

typedef struct NetworkPeer NetworkPeer;

typedef struct
{
	NetworkEventType type;
	NetworkPeer* peer;
	uint8_t channel;

	const void* data;
	size_t size;
} NetworkEvent;

typedef void (*NetworkEventCallback)(const NetworkEvent* event, void* user_data);

bool network_init();
void network_destroy();
