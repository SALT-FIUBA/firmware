# firmware


## 🧩 Bridge en0 and en5 to route packet from Wi-Fi to Nucleo-144

You want to bridge:

* **Wi-Fi (`en0`)** — connected to your router.
* **Ethernet (`en5`)** — connected to the hub, which connects to the Nucleo board.

So that your Mac (on Wi-Fi) can route packets to `192.168.1.80` over `en5`.

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

## 🚨 Caveats

* This configuration will **not persist after reboot**. You can script it or use a launch daemon to automate.
* Some versions of macOS are **quirky about bridging Wi-Fi**, especially newer ones (Monterey, Ventura+). If it fails, let me know what macOS version you’re running.

---

Let me know how far you get and if you’d like a script to automate this setup!
