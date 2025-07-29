#!/bin/bash

BRIDGE_IF="bridge100"
WIFI_IF="en0"
ETH_IF="en5"
STATIC_IP="192.168.1.100"
NETMASK="255.255.255.0"

echo "🔧 Setting up bridge between $WIFI_IF and $ETH_IF..."

# Check if bridge already exists
if ifconfig "$BRIDGE_IF" >/dev/null 2>&1; then
  echo "⚠️ Bridge $BRIDGE_IF already exists. Skipping creation."
else
  echo "✅ Creating $BRIDGE_IF..."
  sudo ifconfig "$BRIDGE_IF" create
fi

# Add interfaces to bridge
echo "🔗 Adding $WIFI_IF and $ETH_IF to $BRIDGE_IF..."
sudo ifconfig "$BRIDGE_IF" addm "$WIFI_IF"
sudo ifconfig "$BRIDGE_IF" addm "$ETH_IF"

# Bring up the bridge
echo "📡 Bringing up $BRIDGE_IF..."
sudo ifconfig "$BRIDGE_IF" up

# Optionally clear IPs from physical interfaces
echo "🚫 Disabling IPs on $WIFI_IF and $ETH_IF to avoid conflicts..."
sudo ifconfig "$WIFI_IF" inet 0.0.0.0
sudo ifconfig "$ETH_IF" inet 0.0.0.0

# Assign static IP to the bridge (you can switch to DHCP if needed)
echo "🌐 Assigning static IP $STATIC_IP to $BRIDGE_IF..."
sudo ifconfig "$BRIDGE_IF" inet "$STATIC_IP" netmask "$NETMASK"

echo "✅ Bridge setup complete!"

