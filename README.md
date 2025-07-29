# firmware




## 🧩 Bridge en0 and en5 to route packet from Wi-Fi to Nucleo-144

You want to bridge:

* **Wi-Fi (`en0`)** — connected to your router.
* **Ethernet (`en5`)** — connected to the hub, which connects to the Nucleo board.

So that your Mac (on Wi-Fi) can route packets to `192.168.1.80` over `en5`.

---

## 🚨 Caveats

* This configuration will **not persist after reboot**. You can script it or use a launch daemon to automate.
* Some versions of macOS are **quirky about bridging Wi-Fi**, especially newer ones (Monterey, Ventura+). If it fails, let me know what macOS version you’re running.



---

## ✅ Step-by-Step: Create a Bridge Manually via Terminal

### 1. **Check your interface names**

Run this in Terminal:

```bash
ifconfig
```

Look for:

* `en0` – your Wi-Fi
* `en5` – the Ethernet interface that connects to the hub/Nucleo

✅ Confirm that `en5` shows `status: active` and the expected MAC address.

---

### 2. **Create a network bridge**

```bash
sudo ifconfig bridge100 create
```

This creates a new bridge interface (usually `bridge100` by default).

---

### 3. **Add interfaces to the bridge**

Now add both interfaces to it:

```bash
sudo ifconfig bridge100 addm en0
sudo ifconfig bridge100 addm en5
```

Then bring the bridge interface up:

```bash
sudo ifconfig bridge100 up
```

---

### 4. **Assign an IP address to the bridge**

If you're using DHCP:

```bash
sudo ipconfig set bridge100 DHCP
```

Or assign it manually, for example:

```bash
sudo ifconfig bridge100 inet 192.168.1.100 netmask 255.255.255.0
```

You may also want to **disable IPs on the individual members** (en0 and en5) to avoid conflicts:

```bash
sudo ifconfig en0 inet 0.0.0.0
sudo ifconfig en5 inet 0.0.0.0
```

---

### 5. **Test connectivity**

Now try:

```bash
ping 192.168.1.80
```

You should be able to reach the Nucleo board **through the bridge**.

---

## 🧼 Optional: Delete the Bridge (if needed)

If you want to reset to a clean state:

```bash
sudo ifconfig bridge100 destroy
```

Then reboot or re-enable en0 and en5 manually:

```bash
sudo ifconfig en0 up
sudo ifconfig en5 up
```

---


# Scripts


## setup nucleo bridge script 

```bash 

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
```

## remove nucleo bridge script 

```bash 
#!/bin/bash

BRIDGE_IF="bridge100"

echo "🧨 Tearing down $BRIDGE_IF..."

if ifconfig "$BRIDGE_IF" >/dev/null 2>&1; then
  sudo ifconfig "$BRIDGE_IF" destroy
  echo "✅ Bridge removed."
else
  echo "⚠️ $BRIDGE_IF not found."
fi
```

